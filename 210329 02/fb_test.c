#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/fb.h>
#include <time.h>

#include "videodev2.h"
#include "hdmi_api.h"
#include "hdmi_lib.h"
#include "s3c_lcd.h"

#define FB_DEV	"/dev/fb0"

void swap(int *swapa, int *swapb) {
    int temp;
    if(*swapa > *swapb)
    {
        temp = *swapb;
        *swapb = *swapa;
        *swapa = temp;
    }
} 

int main(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));
    printf("start\n");
    int frame_fd;
    struct fb_var_screeninfo fvs;
    unsigned int *fb;
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

    printf("init\n");
    fb = (unsigned int*) mmap(0, fvs.xres * fvs.yres * sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED, frame_fd, 0);
    memset(fb, 0x00, 1280*720*4);
    int x, y;
    int cnt = 100;
    while(cnt--)
    {
        int px1 = (int)((fvs.xres*1.0*rand())/(RAND_MAX+1.0));
        int px2 = (int)((fvs.xres*1.0*rand())/(RAND_MAX+1.0));
        int py1 = (int)((fvs.yres*1.0*rand())/(RAND_MAX+1.0));
        int py2 = (int)((fvs.yres*1.0*rand())/(RAND_MAX+1.0));
        
        swap(&px1, &px2);
        swap(&py1, &py2);
        unsigned int color = (unsigned int)(rand());
        for(y = py1; y < py2; y++)
        {
            for(x = px1;  x < px2; x++)
            {
                *(fb + x + y * fvs.xres) = color;
            }
        }
    }
    scanf("%d");

    hdmi_gl_streamoff(0);
	hdmi_gl_deinitialize(0);
	hdmi_deinitialize();

    munmap(fb, fvs.xres * fvs.yres * sizeof(unsigned int));
    close(frame_fd);
    return 0;
}
