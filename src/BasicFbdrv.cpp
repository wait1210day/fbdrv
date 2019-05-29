#include "FrameBuffer.h"
#include "FbErrno.h"
#include "FbProperties.h"
#include "BasicFbdrv.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <iomanip>

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace std;
using namespace sora;

BasicFbdrv::BasicFbdrv (string& path)
{
    fbdev_path = path;

    release_vfbmap ();
    try {
        init_data_struct ();
        open_fbdev ();
        mapping_graphics_memories ();
        flush_screen ();
    } catch (int e) {
        throw e;
    }

    device_released = false;
}

BasicFbdrv::BasicFbdrv ()
{
    fbdev_path = "/dev/fb0";
    release_vfbmap ();

    try {
        init_data_struct ();
        open_fbdev ();
        mapping_graphics_memories ();
        flush_screen ();
    } catch (int e) {
        throw e;
    }

    device_released = false;
}

BasicFbdrv::~BasicFbdrv ()
{
    if (!device_released) {
        /**
        * 清理屏幕并取消映射
        * 最后关闭打开的设备文件
        */
        flush_screen ();

        if (mapped != NULL)
            munmap (mapped, screensize);
        if (fd > 0)
            close (fd);
    }

    delete vinfo;
    delete finfo;
    delete addr_data;
}

void BasicFbdrv::request_signalval_interrupt (int sigval) {
    signal (sigval, this->sigval_interrupted);
}

void BasicFbdrv::sigval_interrupted (int sigval) {
    release_vfbmap ();
    flush_screen ();
    if (mapped != NULL) munmap (mapped, screensize);
    if (fd > 0) close (fd);

    protect_vfbmap ();

    std::cout
        << "FrameBuffer::BasicFbdrv::sigval_interrupted: signal ("
        << sigval
        << "), Framebuffer Device will be closed"
        << std::endl;
}

void BasicFbdrv::init_data_struct ()
{
    // 设置默认颜色属性
    set_RGB_option (0, 0, 0);
    set_transp_option (0);

    // 初始化相应变量
    screensize = 0;
    mapped = NULL;

    vinfo = new vscreeninfo_t;
    finfo = new fscreeninfo_t;
    addr_data = new addr_data_t;

    addr_data->ox = 0;
    addr_data->oy = 0;
}

void BasicFbdrv::open_fbdev ()
{
    fd = open (fbdev_path.data(), O_RDWR);
    if (fd < 0)
        throw -FBDRV_EOPEN;

    // 获取 fbdev 的全部属性
    if (ioctl (fd, FBIOGET_FSCREENINFO, finfo) < 0)
    {
        throw -FBDRV_EGETSTAT;
    }

	if (ioctl (fd, FBIOGET_VSCREENINFO, vinfo) < 0)
        throw -FBDRV_EGETSTAT;
}

void BasicFbdrv::mapping_graphics_memories ()
{
    screensize = vinfo->xres * vinfo->yres * vinfo->bits_per_pixel / 8;
	mapped = (char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0);

	if (mapped == NULL)
		throw -FBDRV_EMMAP;
}

addr_data_t& BasicFbdrv::addr_convert (unsigned int x, unsigned int y)
{
    // 判断是否是无效的坐标
    if (x >= vinfo->xres || y >= vinfo->yres)
        throw -FBDRV_EINVALIDADDR;
    
    addr_data->x = x;
    addr_data->y = vinfo->yres - y - 1;

    return *addr_data;
}

void BasicFbdrv::set_RGB_option (uint32_t red, uint32_t green, uint32_t blue)
{
    if (device_locked)
        throw -FBDRV_EDEVBUSY;

    if (red > 255 || green > 255 || blue > 255)
        throw -FBDRV_EINVALIDPROP;

    this->red = red;
    this->green = green;
    this->blue = blue;
}

void BasicFbdrv::set_transp_option (uint32_t transp)
{
    if (device_locked)
        throw -FBDRV_EDEVBUSY;

    if (transp > 255)
        throw -FBDRV_EINVALIDPROP;

    this->transp = transp;
}

uint32_t BasicFbdrv::get_RGB_option (ushort color)
{
    switch (color)
    {
        case RGB_R: return this->red; break;
        case RGB_G: return this->green; break;
        case RGB_B: return this->blue; break;
        default:
            return -FBDRV_EINVALIDPROP;
    }

    return 0;
}

uint32_t BasicFbdrv::get_transp_option ()
{
    return this->transp;
}

void BasicFbdrv::write_graphics_mem (addr_data_t& addrs) {
    write_graphics_mem (addrs.x, addrs.y);
}

void BasicFbdrv::write_graphics_mem (unsigned int x, unsigned int y)
{
    if (device_locked)
        throw -FBDRV_EDEVBUSY;

    if (x >= vinfo->xres || y >= vinfo->yres)
        throw -FBDRV_EINVALIDADDR;

    /**
     * 最核心也是最基本的部分，FrameBuffer 的所有绘图操作都需要调用此段
     * 代码。之前通过 mmap (...) 映射显存到 mapped 指针，
     * 现在只需要写入指针指向的内存块，即可绘制一个点。
    */

    long location;
	location = x * (vinfo->bits_per_pixel / 8) + y * finfo->line_length;

    /* mapped 指针的每一个像素都包含了四个字节 */
	*(mapped + location + 0x0) = blue;    /* 蓝色的色深 */
	*(mapped + location + 0x1) = green;   /* 绿色的色深 */
	*(mapped + location + 0x2) = red;	  /* 红色的色深 */
	*(mapped + location + 0x3) = transp;  /* 是否透明   */
}

void BasicFbdrv::flush_screen ()
{
    if (device_locked)
        throw -FBDRV_EDEVBUSY;

    /*
    set_RGB_option (0, 0, 0);
    set_transp_option (0);

    for (int i = 0; i != vinfo->xres; i++) {
        for (int j = 0; j != vinfo->yres; j++) {
            write_fbdev_address (i, j);
        }
    }
    */

    /**
     * 上述清屏方法看起来很易懂，但是不够快速，1960x1080 的屏幕需要耗费 0.3s 的时间
     * 因此这里使用一种更为暴力的方法：直接清除指针内存区块
    */
    memset (mapped, '\0', screensize);
}

void BasicFbdrv::protect_vfbmap () {
    if (device_locked)
        throw -FBDRV_EDEVBUSY;
    this->device_locked = true;
}

void BasicFbdrv::release_vfbmap () {
    this->device_locked = false;
}

bool BasicFbdrv::vfbmap_was_protected () {
    return device_locked;
}