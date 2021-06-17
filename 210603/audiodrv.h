#ifndef _AUDIODRV_H_
#define _AUDIODRV_H_

#define AUDIO_IN_MIC 0
#define AUDIO_IN_LINEIN 1

#define WM9711_REG_DAC_VOL 0x18 
#define WM9711_REG_HEADPHONE_VOL 0x04 // External Output
#define WM9711_REG_OUT2_VOL 0x02 // Internal Output
#define WM9711_REG_OUT3_VOL 0x16 // Handset Output
#define WM9711_REG_REC_SEL 0x1A
#define WM9711_REG_REC_LINEIN_VOL 0x10 // External INPUT
#define WM9711_REG_REC_PHONE_VOL 0x0C // Handset 
#define WM9711_REG_REC_MIC_VOL	0x0E // Internal mic
#define WM9711_REG_REC_GAIN 0x1C
#define WM9711_REG_AUXDAC 0x12
#define WM9711_REG_ALC_CTL 0x60
#define WM9711_REG_ALC_NGC 0x62

void AudioInit( void );
void AudSetPlayVol( int vol );
void AudSetRecExtVol( int vol );
void AudWM9711RegInit( void );
void AudConfigWM9711Port( void );
void AudMuteExtOut( int onoff );

#endif
