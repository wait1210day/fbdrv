#ifndef __VIRTUALFBDRV_H__
#define __VIRTUALFBDRV_H__ 1

#include "FrameBuffer.h"

namespace sora {
    /**
     * VirtualFbdrv 是与 BasicFbdrv 相对的一个概念，它是
     * 配合 BasicFbdrv 高效完成渲染工作的得力助手。
     * VirtualFbdrv 进一步对帧缓冲设备进行高级抽象，使其从 Linux / Nouveau...
     * 中被抽离出来，完全成为了用户空间的一种数据结构。
     * 
     * BasicFbdrv 的成员函数 emulating_fbmap 便是用于构造 VirtualFbdrv 的。
    */
    class VirtualFbdrv : public FrameBuffer
    {
    public:
        VirtualFbdrv (vscreeninfo_t& vinfo, fscreeninfo_t& finfo, long screensize);
        ~VirtualFbdrv ();

        void write ();
        char* read ();
        void protect_vfbmap ();
        void release_vfbmap ();

        bool vfbmap_was_protected ();

    private:
        void requesting_vfb_memories ();
        void free_vfb_memories ();
        void giveup_data ();
    };
}

#endif