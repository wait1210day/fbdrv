
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>
#include <stdint.h>

typedef struct fb_var_screeninfo vscreeninfo_t;
typedef struct fb_fix_screeninfo fscreeninfo_t;
typedef struct __fbdev_RGB_option
{
	uint32_t r;
	uint32_t g;
	uint32_t b;
	uint32_t transp;
	char *mapped;
	vscreeninfo_t vinfo;
	fscreeninfo_t finfo;
	int fd;
} fbdev_descriptor_t;

int open_fbdev (char *fbdev_dir, fbdev_descriptor_t *prop)
{
	int fp = 0;

	fp = open (fbdev_dir, O_RDWR);
	if (fp < 0)
	{
		perror ("Error : Can not open framebuffer device");
		exit (1);
	}
	if (ioctl (fp, FBIOGET_FSCREENINFO, &prop->finfo))
	{
		perror ("Error reading fixed information");
		exit (2);
	}

	if (ioctl (fp, FBIOGET_VSCREENINFO, &prop->vinfo))
	{
		perror ("Error reading variable information");
		exit (3);
	}

	return fp;
}

long int mapping_fbdev (fbdev_descriptor_t *prop)
{
	long screensize = 0;

	screensize = prop->vinfo.xres * prop->vinfo.yres * prop->vinfo.bits_per_pixel / 8;
	prop->mapped = (char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, prop->fd, 0);

	if (prop->mapped == NULL)
	{
		printf ("Error: failed to map framebuffer device to memory");
		exit (4);
	}

	return screensize;
}

void write_fbdev_mem (int xaddr, int yaddr, fbdev_descriptor_t *prop)
{

	if (xaddr >= prop->vinfo.xres || yaddr >= prop->vinfo.yres)
		return ;

	long location;

	location = xaddr * (prop->vinfo.bits_per_pixel / 8) + yaddr * prop->finfo.line_length;
	*(prop->mapped + location + 0) = prop->b;   /* 蓝色的色深 */
	*(prop->mapped + location + 1) = prop->g;  /* 绿色的色深*/
	*(prop->mapped + location + 2) = prop->r;	/* 红色的色深*/
	*(prop->mapped + location + 3) = prop->transp; /* 是否透明*/

	usleep (1000);
}

int main (int argc, char **argv)
{
	long screensize;
	fbdev_descriptor_t properties;

	properties.fd = open_fbdev ("/dev/fb0", &properties);
	screensize = mapping_fbdev (&properties);

	properties.r = 255;
	properties.g = 0;
	properties.b = 0;

	int y, a, b;

	for (a = properties.vinfo.xres / 2, b = properties.vinfo.xres / 2; a != 10 && b != 10; a -= 10, b -= 10)
	{
		for (int x = (b - a); x != (a + b); x++) {
			y = (int) sqrt (a * a - (x - b) * (x - b));

			write_fbdev_mem (x, y, &properties);
			usleep (2000);
		}
	}

	munmap (properties.mapped, screensize);
	close (properties.fd);
	return 0;
}
