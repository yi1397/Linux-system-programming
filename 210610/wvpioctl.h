#ifndef _WVPIOCTL_H_
#define _WVPIOCTL_H_

#define WVPDRV_NAME     "wvpdrv"
#define WVPDRV_MAJOR	130

#define WVPDRV_IOCTL_MAGIC 'P'
//----------------------------------------------------------
// WVP Genernal
#define WVPDRV_GET_BOARD_VERSION		_IOWR(WVPDRV_IOCTL_MAGIC, 0, WvpIoctlArgs*)
#define WVPDRV_GET_BOARD_ID				_IOWR(WVPDRV_IOCTL_MAGIC, 1, WvpIoctlArgs*)
#define WVPDRV_GET_IO_STAT				_IOWR(WVPDRV_IOCTL_MAGIC, 2, WvpIoctlArgs*)
#define WVPDRV_SET_IO_STAT				_IOWR(WVPDRV_IOCTL_MAGIC, 3, WvpIoctlArgs*)


//----------------------------------------------------------
// WVP AC97
#define WVPDRV_AC97_READ			_IOWR(WVPDRV_IOCTL_MAGIC, 60, WvpIoctlArgs*)
#define WVPDRV_AC97_WRITE			_IOWR(WVPDRV_IOCTL_MAGIC, 61, WvpIoctlArgs*)

#define WVPDRV_SET_EXT_CAMERA_RESOLUTION _IOWR(WVPDRV_IOCTL_MAGIC, 70, WvpIoctlArgs*)

#define WVPDRV_SET_TDA9955_POWER		_IOWR(WVPDRV_IOCTL_MAGIC, 80, WvpIoctlArgs*)
#define WVPDRV_SET_HDMI_POWER			_IOWR(WVPDRV_IOCTL_MAGIC, 81, WvpIoctlArgs*)
#define WVPDRV_SET_UART_TXMODE          _IOWR(WVPDRV_IOCTL_MAGIC, 82, WvpIoctlArgs*)
#define WVPDRV_SET_EXT_ALARM            _IOWR(WVPDRV_IOCTL_MAGIC, 83, WvpIoctlArgs*)
#define WVPDRV_SET_EMERGENCY_LAMP       _IOWR(WVPDRV_IOCTL_MAGIC, 84, WvpIoctlArgs*)

#define WVP_IO_ADDR_LCD_POWER		(2)
#define WVP_IO_ADDR_7113_ENABLE		(3)
#define WVP_IO_ADDR_RING_LED		(4)
#define WVP_IO_ADDR_SPK_BT_LED		(5)
#define WVP_IO_ADDR_SPK_MIC_MUTE	(6)
#define WVP_IO_ADDR_SEND_BT_LED		(7)
#define WVP_IO_ADDR_12K_BT_LED		(8)
#define WVP_IO_ADDR_HOOK_STAT		(9)
#define WVP_IO_ADDR_ETH_LINK_STAT	(10)
#define WVP_IO_ADDR_INT_STAT		(11)
#define WVP_IO_ADDR_AUDIO_MUTE		(13)

#define WVP_IO_MIN_ADDR	WVP_IO_ADDR_LCD_POWER
#define WVP_IO_MAX_ADDR	WVP_IO_ADDR_AUDIO_MUTE

#define MAX_PLAY_REC_DATA_SIZE 4096

#ifdef __KERNEL__
int wvpdrv_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
unsigned char ioctl_get_board_ver( void );
unsigned char ioctl_get_board_id( void );
unsigned int ioctl_get_io_stat( unsigned int ioaddr );
void ioctl_set_io_stat( unsigned int ioaddr, unsigned int ioValue );
#endif

typedef struct wvp_ioctl_args
{
	unsigned int inArgs[5];	
	unsigned int outArgs[5];	
} WvpIoctlArgs;

#endif
