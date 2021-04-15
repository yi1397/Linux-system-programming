#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "videodev2.h"
#include "hdmi_api.h"
#include "hdmi_lib.h"
#include "s3c_lcd.h"
#include "font.h"

#define FB_DEV	"/dev/fb0"

typedef struct FrameBuffer {
	int         fd;
	void        *start;
	size_t      length;
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
} FrameBuffer;


// 키보드 이벤트를 처리하기 위한 함수, Non-Blocking 입력을 지원
//  값이 없으면 0을 있으면 해당 Char값을 리턴
static int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

int fb_open(FrameBuffer *fb){
	int fd;
	int ret;

	fd = open(FB_DEV, O_RDWR);
	if(fd < 0){
		perror("FB Open");
		return -1;
	}
	ret = ioctl(fd, FBIOGET_FSCREENINFO, &fb->fix);
	if(ret < 0){
		perror("FB ioctl");
		close(fd);
		return -1;
	}
	ret = ioctl(fd, FBIOGET_VSCREENINFO, &fb->var);
	if(ret < 0){
		perror("FB ioctl");
		close(fd);
		return -1;
	}
	fb->start = (unsigned char *)mmap (0, fb->fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(fb->start == NULL){
		perror("FB mmap");
		close(fd);
		return -1;
	}
	fb->length = fb->fix.smem_len;
	fb->fd = fd;
	return fd;
}

void fb_close(FrameBuffer *fb){
	if(fb->fd > 0)
		close(fb->fd);
	if(fb->start > 0){
		msync(fb->start, fb->length, MS_INVALIDATE | MS_SYNC);
		munmap(fb->start, fb->length);
	}
}

void drawFont(FrameBuffer *gfb, int x, int y, char ch, unsigned int color, unsigned int bgcolor)
{
	int m,n;
	unsigned int *pos = (unsigned int*)gfb->start;

	for(m = 0; m < 16; m++)
	{
		if(720 <= y+n) break;
		for(n = 0; n < 8; n++)
		{
			pos[(y+m)*1280 + x+n] = fontdata_8x16[ch*16 + m]&0x80>>n?color:bgcolor;
		}
	}
}

// Make Function!!
void drawText(FrameBuffer *gfb, int x, int y, char *msg, unsigned int color, unsigned int bgcolor)
{
	int i;
	for(i = 0; *msg != 0; i++)
    {
        if(x + i * 8 < 1272)drawFont(gfb, x + i * 8, y, *msg, color, bgcolor);
		msg++;
	}
}

int main()
{
	int i, j;
	int x, y;
	int ret;
	unsigned int *pos;
	int endFlag = 0;
	int ch;
	unsigned int phyLCDAddr = 0;
	FrameBuffer gfb;

	printf("Font Test Program Start\n");

	ret = fb_open(&gfb);
	if(ret < 0){
		printf("Framebuffer open error");
		perror("");
		return -1;
	}

	// get physical framebuffer address for LCD
	if (ioctl(ret, S3CFB_GET_LCD_ADDR, &phyLCDAddr) == -1)
	{
		printf("%s:ioctl(S3CFB_GET_LCD_ADDR) fail\n", __func__);
		return 0;
	}
	printf("phyLCD:%x\n", phyLCDAddr);

	hdmi_initialize();

	hdmi_gl_initialize(0);
	hdmi_gl_set_param(0, phyLCDAddr, 1280, 720, 0, 0, 0, 0, 1);
	hdmi_gl_streamon(0);

	x = 50;
	y = 100;

	pos = (unsigned int*)gfb.start;

	//Clear Screen(Black)
	memset(pos, 0x00, 1280*720*4);

	// Draw Box
	for (i=x; i<x+100; i++)
	{
		for (j=y; j<(y+100); j++)
		{
			pos[j*1280+i] = 0xFFFF0000; // Red 
		}
	}

	drawText(&gfb, 50, 300, "Hello World!", 0xFFFF0000, 0);
	drawText(&gfb, 50, 320, "Meister HighSchool", 0xFFFFFF00, 0xFF0000FF);
	drawText(&gfb, 50, 340, "Embedded System Class", 0xFFFFFFFF, 0xFFFF00FF);



	printf("'q' is Quit\n");
	while (!endFlag)
	{
		usleep(10*1000);

		if (kbhit())
		{
			ch = getchar();
			switch ( ch )
			{
				case 'q': endFlag = 1;
					break;
			}
		}
	}

	hdmi_gl_streamoff(0);
	hdmi_gl_deinitialize(0);
	hdmi_deinitialize();
	fb_close(&gfb);

	return 0;
}


