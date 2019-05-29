#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__ 1

#include <cstdint>
#include <linux/fb.h>
#include <iostream>

#include "FbErrno.h"

namespace sora
{
    typedef struct fb_var_screeninfo vscreeninfo_t;
    typedef struct fb_fix_screeninfo fscreeninfo_t;
    typedef struct addr_data_struct
    {
        int x;
        int y;
        int ox;
        int oy;
    } addr_data_t;

    class FrameBuffer {
    public:
        virtual addr_data_t& addr_convert (unsigned int x, unsigned int y) = 0;
        virtual void write_graphics_mem (unsigned int x, unsigned int y) = 0;
        virtual void write_graphics_mem (addr_data_t& addrs) = 0;
        virtual void set_RGB_option (uint32_t red, uint32_t green, uint32_t blue) = 0;
        virtual void set_transp_option (uint32_t transp) = 0;
        virtual uint32_t get_RGB_option (ushort color) = 0;
        virtual uint32_t get_transp_option () = 0;
        virtual void flush_screen () = 0;

        virtual void protect_vfbmap () = 0;
        virtual void release_vfbmap () = 0;
        virtual bool vfbmap_was_protected () = 0;
    };
}

#endif