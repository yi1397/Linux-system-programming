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

#include "videodev2.h"
#include "hdmi_api.h"
#include "hdmi_lib.h"
#include "s3c_lcd.h"

#define FB_DEV	"/dev/fb0"

static int curX=0, curY=0, curP=0;

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

    int flag = 0;
    int cnt = 0;
    while(1)
    {
        len = read(mouse_fd, &event, sizeof(event));
        if(len > 0)
        {
            OnHidEvent(&event);
            if(event.code == 272)
            {
                flag = !flag;
            }
            if(flag) cnt++;
            if(flag&&cnt==3)
            {
                cnt = 0;
                fb[curX + curY*1280] = 0xFFFFFFFF;
            }
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
