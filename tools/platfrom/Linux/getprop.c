
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/ioctl.h>
 
int main (int argc, char **argv)
{
	int fp = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	fp = open ("/dev/fb0", O_RDWR);
 
	if (fp < 0)
	{
		 printf("Error : Can not open framebuffer device/n");
		 exit(1);
	}

	if (ioctl(fp,FBIOGET_FSCREENINFO, &finfo))
	{
		 printf("Error reading fixed information/n");
		 exit(2);
	}
	if (ioctl (fp,FBIOGET_VSCREENINFO, &vinfo))
	{
		 printf("Error reading variable information/n");
		 exit(3);
	}


	puts ("fb_var_screeninfo : {");
	printf (
		"	xres = %d\n\
	yres = %d\n\
	xres_virtual = %d\n\
	yres_virtual = %d\n\
	xoffset = %d\n\
	yoffset = %d\n\
	bits_per_pixel = %d\n\
	grayscale = %d\n\
	\033[31mred : {\n\
		offset = %d\n\
		length = %d\n\
		msb_right = %d\n\
	}\033[0m\n\
	\033[32mgreen : {\n\
		offset = %d\n\
		length = %d\n\
		msb_right = %d\n\
	}\033[0m\n\
	\033[34mblue : {\n\
		offset = %d\n\
		length = %d\n\
		msb_right = %d\n\
	}\033[0m\n\
	\033[33mtransp : {\n\
		offset = %d\n\
		length = %d\n\
		msb_right = %d\n\
	}\033[0m\n",
		vinfo.xres,
		vinfo.yres,
		vinfo.xres_virtual,
		vinfo.yres_virtual,
		vinfo.xoffset,
		vinfo.yoffset,
		vinfo.bits_per_pixel,
		vinfo.grayscale,
		vinfo.red.offset,
		vinfo.red.length,
		vinfo.red.msb_right,
		vinfo.green.offset,
		vinfo.green.length,
		vinfo.green.msb_right,
		vinfo.blue.offset,
		vinfo.blue.length,
		vinfo.blue.msb_right,
		vinfo.transp.offset,
		vinfo.transp.length,
		vinfo.transp.msb_right
	);

	printf (
		"	nonstd = %d\n\
	activate = %d\n\
	height = %d\n\
	width = %d\n\
	accel_flags = %d\n\
	pixclock = %d\n\
	left_margin = %d\n\
	right_margin = %d\n\
	upper_margin = %d\n\
	lower_margin = %d\n\
	hsync_len = %d\n\
	vsync_len = %d\n\
	sync = %d\n\
	vmode = %d\n\
	rotate = %d\n\
	colorspace = %d\n\
	reserved[0] = %d\n\
	reserved[1] = %d\n\
	reserved[2] = %d\n\
	reserved[3] = %d\n\
}\n",
		vinfo.nonstd,
		vinfo.activate,
		vinfo.height,
		vinfo.width,
		vinfo.accel_flags,
		vinfo.pixclock,
		vinfo.left_margin,
		vinfo.right_margin,
		vinfo.upper_margin,
		vinfo.lower_margin,
		vinfo.hsync_len,
		vinfo.vsync_len,
		vinfo.sync,
		vinfo.vmode,
		vinfo.rotate,
		vinfo.colorspace,
		vinfo.reserved[0],
		vinfo.reserved[1],
		vinfo.reserved[2],
		vinfo.reserved[3]
	);

	printf ("\nfb_fix_screeninfo : {\n");
	printf (
		"	accel = %d\n\
	capabilities = %d\n\
	id = %s\n\
	line_length = %d\n\
	mmio_len = %d\n\
	mmio_start = %ld\n\
	smem_len = %d\n\
	smem_start = %ld\n\
	type = %d\n\
	type_aux = %d\n\
	visual = %d\n\
	xpanstep = %d\n\
	ypanstep = %d\n\
	ywrapstep = %d\n\
	reverved[0] = %d\n\
	reverved[1] = %d\n}\n",

		finfo.accel,
		finfo.capabilities,
		finfo.id,
		finfo.line_length,
		finfo.mmio_len,
		finfo.mmio_start,
		finfo.smem_len,
		finfo.smem_start,
		finfo.type,
		finfo.type_aux,
		finfo.visual,
		finfo.xpanstep,
		finfo.ypanstep,
		finfo.ywrapstep,
		finfo.reserved[0],
		finfo.reserved[1]
	);

	close (fp);
	return 0;
}
