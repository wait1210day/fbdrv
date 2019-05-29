#ifndef __BASICFBDRV_H__
#define __BASICFBDRV_H__

#include "FrameBuffer.h"
#include <signal.h>

namespace sora {
    /**
     * BasicFbdrv 是本项目中最底层的用户空间 api。
     * BasicFbdrv 全名是 Basic FrameBuffer Driver，它将帧缓冲
     * 设备高度抽象，并在底层提供了操作对象的接口。BasicFbdrv 具体提供
     * 了这些功能：
     * 设备的打开与关闭
     * 系统资源的分配
     * 帧缓冲显存的映射
     * 绘制一个像素点、一块像素块（需配合 VirtualFbdrv）的基本方法
     * 
     * 开发者可以在此之上建立诸如函数几何演算绘制、图片显示、视频播放
     * 甚至是取代 X11(X.Org) 开发新的图形系统。
    */
    class BasicFbdrv : public FrameBuffer
    {
    public:
        BasicFbdrv (std::string & path);
        BasicFbdrv ();
        ~BasicFbdrv ();

        /**
         * 此函数是 BasicFbdrv 类独有的，向系统注册一个针对某特定进程信号量
         * 的中断。系统将会中断到类的析构函数上，同时正确地清理屏幕并释放设备。
        */
        void request_signalval_interrupt (int sigval);

        /**
         * 转换 fbdrv 坐标为平面直角坐标系坐标
         * 大部分图形卡驱动程序（以下简称 fbdrv）规定屏幕左上角为 (0, 0) 坐标，
         * 向右侧延伸为递增的 x 坐标，向下延伸为递增的 y 坐标。但是这很不符合
         * 我们平时数学上的使用习惯，尤其是使用 FrameBuffer 绘制函数图像测试时。
         * 因此，addr_convert 函数将会根据屏幕分辨率自动转换平面直角坐标系的
         * 坐标标准（即以左下角为原点，整个屏幕为第一象限）为 fbdrv 坐标标准。
         * 
         * 参数：平面直角坐标系的 x、y 坐标
         * 返回：复合类型 addr_data_struct 的引用，包含转换后的 x、y 坐标
         * 
        */
        addr_data_t& addr_convert (unsigned int x, unsigned int y);

        /**
         *  在指定坐标位置绘制一个点
        */
        void write_graphics_mem (unsigned int x, unsigned int y);
        void write_graphics_mem (addr_data_t& addrs);

        /**
         * 设置颜色属性
        */
        void set_RGB_option (uint32_t red, uint32_t green, uint32_t blue);
        void set_transp_option (uint32_t transp);
        uint32_t get_RGB_option (ushort color);
        uint32_t get_transp_option ();

        /**
         * 进行清屏操作
        */
        void flush_screen ();

        /**
         * 锁定、解锁、查看锁状态。用于多线程运行时的资源抢占问题
        */
        void protect_vfbmap ();
        void release_vfbmap ();
        bool vfbmap_was_protected () ;

    private:
        /* init_data_struct 负责初始化一些数据结构 */
        void init_data_struct ();
        /* 打开设备（可能需要 root） */
        void open_fbdev ();
        /**
         * 映射 framebuffer 数据到进程内存空间。Linux 运行于
         * x86 保护模式，为了绕开 CPU 的内存保护，Linux 内核采取
         * mmap 的方式映射显存地址空间到用户内存空间
         **/
        void mapping_graphics_memories ();

        void sigval_interrupted (int sigval);

    private:
        bool device_locked;
        bool device_released;

        uint32_t red;
	    uint32_t green;
	    uint32_t blue;
	    uint32_t transp;
	    char *mapped;
        long screensize;
	    vscreeninfo_t *vinfo;
	    fscreeninfo_t *finfo;
	    int fd;

        std::string fbdev_path;

        addr_data_t *addr_data;
    };
}

#endif // __BASICFBDRB_H__