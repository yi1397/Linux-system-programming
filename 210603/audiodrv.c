#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/soundcard.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "audiodrv.h"
#include "wvplib.h"

static int audioInPort 	= AUDIO_IN_MIC;
static int audioRecExtVol = 4;
static int audioPlayVol = 3;
static int audioRecExtVolTable[8] = { 0x8600, 0x8E00, 0x9600, 0x9E00, 0xA600, 0xAE00, 0xB600, 0xBE00 }; // ALC Version
static int audioExtMute = 0;

void AudioInit( void )
{
	AudWM9711RegInit();
	AudSetPlayVol( 3 );
}

void AudMuteExtOut( int onoff )
{
	int val = 0;

    audioExtMute = onoff;

	val = GetAC97Reg( WM9711_REG_HEADPHONE_VOL );
	val = (val & 0x7FFF) | ( (onoff & 0x01) << 15);
	if ( onoff == 0 ) val = 0;
	SetAC97Reg( WM9711_REG_HEADPHONE_VOL, val ); 

    // JEONG also set DAC mute  20141212
    if ( onoff )
    {
        SetAC97Reg( WM9711_REG_DAC_VOL, 0xE000 );   // Mute
    }
    else
    {
        SetAC97Reg( WM9711_REG_DAC_VOL, 0x6108 );   // On
    }
}


void AudWM9711RegInit( void )
{
    SetAC97Reg( WM9711_REG_DAC_VOL, 0xE108 ); 

	SetAC97Reg( WM9711_REG_AUXDAC, 0x8880 );
    SetAC97Reg( WM9711_REG_ALC_CTL, 0xD032 ); 

    SetAC97Reg( WM9711_REG_ALC_NGC, 0xBE9F ); 

	AudConfigWM9711Port();
}

void AudConfigWM9711Port( void )
{
	int tmp; 

    if ( audioExtMute )
    {
        AudMuteExtOut( 1 ); // Ext Mute JEONG 20141212
    }
    else
    {
	    AudMuteExtOut( 0 ); // Ext ON
    }

	if ( audioInPort == AUDIO_IN_LINEIN)
	{
		SetAC97Reg( WM9711_REG_REC_SEL, 0x0404 ); 
		SetAC97Reg( WM9711_REG_ALC_NGC, 0x3E00); 
		tmp = (audioRecExtVol*2)-1;
		tmp = (tmp << 8) | 0x00; 
		SetAC97Reg( WM9711_REG_REC_GAIN , tmp ); 
		printf("<<<<Audio Input LineIn Setting>>>>\n");
	}
	else 
	{
		SetAC97Reg( WM9711_REG_REC_SEL, 0x4404 ); 
        SetAC97Reg( WM9711_REG_ALC_NGC, 0xBE9F ); 
		AudSetRecExtVol( audioRecExtVol );
	}
}

void AudSetPlayVol( int vol )
{
	int val = 0;

	audioPlayVol = vol;

	val = GetAC97Reg( 0x04 ) & 0x8000; 

	switch (vol)
	{
		case 0: val = 0x1F1F; break;
		case 1: val = 0x1A1A; break;
		case 2: val = 0x1616; break;
		case 3: val = 0x1010; break;
		case 4: val = 0x0808; break;
		case 5: val = 0x0000; break;
	}

	SetAC97Reg( 0x04, val ); 
}

void AudSetRecExtVol( int vol )
{
	int val;
	audioRecExtVol = vol;
	
	if ( vol < 1 || vol > 8 ) 
	{
		printf("Invalid vol\n");
		return;
	}

	val = GetAC97Reg( WM9711_REG_ALC_NGC );
	val = (val & 0x0700) | audioRecExtVolTable[vol-1];
	val = (val & 0xFF00) | 0x009F; // Noise Gate Control 

	SetAC97Reg( WM9711_REG_ALC_NGC, val ); 
}

