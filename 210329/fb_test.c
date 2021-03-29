#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/fb.h>

#include "libhdmi/include/videodev2.h"
#include "libhdmi/hdmi_api.h"
#include "libhdmi/include/hdmi_lib.h"
#include "libhdmi/include/s3c_lcd.h"

#define FB_DEV	"/dev/fb0"

int main(int argc, char* argv[])
{
    int frame_fd;
    struct fb_var_screeninfo fvs;
    int *fb;
    unsigned int phyLCDAddr = 0;
    
    if((frame_fd = open("/dev/fb0", O_RDWR)) < 0)
    {
        perror("open error");
        exit(1);
    }

    if(ioctl(frame_fd, FBIOGET_VSCREENINFO, &fvs) == -1)
    {
        perror("ioctl error");
        exit(1);
    }

    if(ioctl(frame_fd, S3CFB_GET_LCD_ADDR, &phyLCDAddr) == -1)
    {
        perror("ioctl error");
        exit(1);
    }

    printf("x-resolution : %d\n", fvs.xres);
    printf("y-resolution : %d\n", fvs.yres);
    printf("x-resolution(virtual) : %d\n", fvs.xres_virtual);
    printf("y-resolution(virtual) : %d\n", fvs.yres_virtual);
    printf("bpp : %d\n", fvs.bits_per_pixel);

    hdmi_gl_initialize(0);
	hdmi_gl_set_param(0, phyLCDAddr, 1280, 720, 0, 0, 0, 0, 1);
	hdmi_gl_streamon(0);

    fb = (int*) mmap(0, fvs.xres * fvs.yres * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, frame_fd, 0);
    
    int x, y;
    for(y = 100; y < 200; y++)
    {
        for(x = 100;  x < 200; x++)
        {
            *(fb + x + y * fvs.xres) = 30000;
        }
    }
    scanf("%d");

    hdmi_gl_streamoff(0);
	hdmi_gl_deinitialize(0);
	hdmi_deinitialize();

    munmap(fb, fvs.xres * fvs.yres * sizeof(int));
    close(frame_fd);
    return 0;
}
