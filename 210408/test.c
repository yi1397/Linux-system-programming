
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <time.h>
#include <math.h>

#include "videodev2.h"
#include "hdmi_api.h"
#include "hdmi_lib.h"
#include "s3c_lcd.h"


#define FB_DEV	"/dev/fb0"

static int curX=0, curY=0, curP=0;

void drawLine(int w, int h, int x1, int y1, int x2, int y2, unsigned int *src)
{
	if(x1 < 0 || y1 < 0) return;
	if(x1 == x2 && y1 == y2)
	{
		src[x1 + y1*w] = 0xFFFFFFFF;
		return;
	}
	int dx, dy;
	dx = x1 - x2;
	dx = 0<dx?dx:-dx;
	dy = y1 - y2;
	dy = 0<dy?dy:-dy;
	if(dy < dx)
	{
		if(x2 < x1)
		{
			int tx, ty;
			tx = x1;
			x1 = x2;
			x2 = tx;
			ty = y1;
			y1 = y2;
			y2 = ty;
		}
		int f = 1;
		int i = x1;
		int p = y1;
		if(y2 - y1 < 0) f = -1;
		
		do
		{
			src[i + p*w] = 0xFFFFFFFF;
			i++;
			p = y1 + (dy * (i - x1) / dx) * f;
			//printf("x1 : %d\n", x1);
			//printf("y1 : %d\n", y1);
			//printf("x2 : %d\n", x2);
			//printf("y2 : %d\n", x2);
			//printf("x : %d\n", i);
			//printf("y : %d\n", p);
		}while(i <= x2);
	}
	else
	{
		if(y2 < y1)
		{
			int tx, ty;
			tx = x1;
			x1 = x2;
			x2 = tx;
			ty = y1;
			y1 = y2;
			y2 = ty;
		}
		int f = 1;
		int i = y1;
		int p = x1;
		if(x2 - x1 < 0) f = -1;
		do
		{
			src[p + i*w] = 0xFFFFFFFF;
			i++;
			p = x1 + (dx * (i - y1) / dy) * f;
			//printf("x1 : %d\n", x1);
			//printf("y1 : %d\n", y1);
			//printf("x2 : %d\n", x2);
			//printf("y2 : %d\n", x2);
			//printf("x : %d\n", p);
			//printf("y : %d\n", i);
		}while(i <= y2);
	}
}

void drawCircle(int w, int h, int x, int y, int r, double b, unsigned int color, unsigned int *src)
{
    int i, j;
    if(b == 0)
    {
        for(i = (x-r-b-1<0?0:x-r-b-1); i != x+r+b+1 && i < w; i++)
        {
            for(j = (y-r-b-1<0?0:y-r-b-1); j != y+r+b+1 && j < h; j++)
            {
                if((i-x)*(i-x) + (j-y)*(j-y) <= r*r) src[i + j*w] = color;
            } 
        }
    }
    else
    {
        int min = (int)(((double)r - b/2)*((double)r - b/2));
        int max = (int)(((double)r + b/2)*((double)r + b/2));
        for(i = (x-r-b-1<0?0:x-r-b-1); i != x+r+b+1 && i < w; i++)
        {
            for(j = (y-r-b-1<0?0:y-r-b-1); j != y+r+b+1 && j < h; j++)
            {
                
                int a = (i-x)*(i-x) + (j-y)*(j-y);
                if(min <= abs(a) && abs(a) <= max) src[i + j*w] = color;
            } 
        }
    }
}

int openMouse()
{
	int handle;
	char dev_info[4096];
	char opendevname[256];
	int len;
	int mouse_fd = -1;
	int isHid = 0;
	int eventNum = 1;
	char *find = NULL;

	memset(dev_info, 0x00, 4096);
	handle = open("/proc/bus/input/devices", O_RDONLY );
	len = read(handle, dev_info, 4096);
	if ( len <= 0 )
	{
		printf("[USB HID Info Read Fail!]\n");
	}
	else 
	{
		if ( (find = strstr(dev_info,"Handlers=mouse")) == NULL )
		{
			//printf("[USB HID Not Found!]\n");
		}
		else {
			isHid = 1;
			printf("[USB HID Found!]\n");

			if ( (find = strstr(find, "event")) != NULL )
			{
				sscanf(find+5, "%d", &eventNum);
				printf("Event Num : %d\n", eventNum);
			}
		}
	}

	close(handle);

	if ( isHid == 1)
	{
		printf("[USB HID is Pluged]\n");
		sprintf(opendevname, "/dev/input/event%d", eventNum);
		mouse_fd = open(opendevname, O_RDONLY | O_NONBLOCK);
		//mouse_fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
		printf("open Mouse Event : %s\n", opendevname);
	}
	return mouse_fd;
}

void OnHidEvent(struct input_event *event)
{
    int x = curX, y = curY, p = curP;
	switch (event->code)
	{
		case REL_X:
			x += event->value;
			if (x < 0) x = 0;
			if (x > 1280) x = 1280-1;
			break;
		case REL_Y:
			y += event->value;
			if (y < 0) y = 0;
			if (y > 720) y = 720-1;
			break;
		case BTN_LEFT:
			p = event->value;
			break;
	}
    curX = x; curY = y; curP = p;
}

int main(int argc, char* argv[])
{
	int bg[20][20];
    printf("start\n");
    
    int frame_fd;
    struct fb_var_screeninfo fvs;
    unsigned int *fb;
    unsigned int phyLCDAddr = 0;

    int mouse_fd;
    struct input_event event;
    int len = 0;

    mouse_fd = openMouse();
    
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
    
    hdmi_gl_initialize(0);
	hdmi_gl_set_param(0, phyLCDAddr, 1280, 720, 0, 0, 0, 0, 1);
	hdmi_gl_streamon(0);

    fb = (unsigned int*) mmap(0, fvs.xres * fvs.yres * sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED, frame_fd, 0);
    memset(fb, 0x00, 1280*720*4);
	memset(bg, 0x00, 20*20*4);

    int flag = 0;
    int cnt = 0;
	int x1,x2,y1,y2;
	int p_curX = 0;
	int p_curY = 0;
    unsigned int curC = 0;
    int curC_X;
    int curC_Y;
    drawLine(1280, 720, 100, 100, 200, 200, fb);

    drawCircle(1280, 720, 800, 200, 100, 0, 0xFF0000FF, fb);
    drawCircle(1280, 720, 750, 250, 100, 2, 0xFF00FF00, fb);
    drawCircle(1280, 720, 850, 250, 95, 10, 0xFFFF0000, fb);

    drawCircle(1280, 720, 200, 700, 100, 0, 0xFF00FFFF, fb);
    drawCircle(1280, 720, 900, 500, 50, 4, 0xFFFFFFFF, fb);
    drawCircle(1280, 720, 700, 500, 50, 3, 0xFFFFFFFF, fb);
    drawCircle(1280, 720, 500, 500, 50, 2, 0xFFFFFFFF, fb);
    drawCircle(1280, 720, 300, 500, 50, 1, 0xFFFFFFFF, fb);
    drawCircle(1280, 720, 300, 300, 60, 0, 0xFFFF0000, fb);
    while(1)
    {
        len = read(mouse_fd, &event, sizeof(event));
        if(len > 0)
        {
            fb[curC_X + curC_Y*1280] = curC;
            OnHidEvent(&event);
            if(event.code == 272)
            {
                flag = !flag;
            }
            if(flag) cnt++;
			else
			{
				x1 = -1;
				x2 = -1;
			}
            if(flag&&cnt==3)
            {
				x2 = curX;
				y2 = curY;
				drawLine(1280, 720, x1, y1, x2, y2, fb);
                cnt = 0;
				x1 = curX;
				y1 = curY;
                //fb[curX + curY*1280] = 0xFFFFFFFF;
            }
            curC = fb[curX + curY*1280];
            fb[curX + curY*1280] = 0xFFFFFFFF;
            curC_X = curX;
            curC_Y = curY;
        }
    }

    hdmi_gl_streamoff(0);
	hdmi_gl_deinitialize(0);
	hdmi_deinitialize();

    munmap(fb, fvs.xres * fvs.yres * sizeof(unsigned int));
    close(frame_fd);
    close(mouse_fd);
    return 0;
}
