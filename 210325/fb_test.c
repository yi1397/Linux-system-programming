#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
    
        printf("123");
    int framebuffer_fd = 0;

    struct fb_var_screeninfo framebuffer_variable_screeninfo;  
    struct fb_fix_screeninfo framebuffer_fixed_screeninfo;

    
    framebuffer_fd = open( "/dev/fb0", O_RDWR );

    if(framebuffer_fd < 0)
    {
        exit(1);
    }
    if (ioctl(framebuffer_fd, FBIOGET_VSCREENINFO, &framebuffer_variable_screeninfo) )  
    {
        exit(1);
    }
    framebuffer_variable_screeninfo.bits_per_pixel=32;
    if(ioctl(framebuffer_fd, FBIOPUT_VSCREENINFO, &framebuffer_variable_screeninfo))
    {
        exit(1);
    }
    if(ioctl(framebuffer_fd, FBIOGET_FSCREENINFO, &framebuffer_fixed_screeninfo))
    {
        exit(1);
    }
    
    int xres = framebuffer_variable_screeninfo.xres;
    int yres = framebuffer_variable_screeninfo.yres;

    unsigned int *framebuffer_pointer = (unsigned int*)mmap(0, xres * yres * 4, PROT_READ|PROT_WRITE, MAP_SHARED, framebuffer_fd, 0);
    if(framebuffer_pointer == MAP_FAILED)
    {
        exit(1);
    }
    
    int x,y;
    for(x = 50; x < 200; x++)
    {
        for(y = 50; y < 200; y++)
        {
            framebuffer_pointer[y*1280+x] = 60000;
        }
    }
    while (1)
    {
        usleep(10000);
        
    }
    
    
    munmap( framebuffer_pointer, xres * yres * 4);
    close(framebuffer_fd); 
}
