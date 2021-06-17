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
#include "wvpioctl.h"
#include "audiodrv.h"
#include "wvplib.h"

int wvpdev_fd = 0;

int InitWvpDrv(void)
{
	if (wvpdev_fd <= 0)
	{
		wvpdev_fd = open("/dev/wvpdrv", 2);
		if (wvpdev_fd < 0)
		{
			fprintf(stdout, "wooksung device open fail \n");
			return 0;
		}
	}

	AudioInit();

	return wvpdev_fd;
}

int GetAC97Reg(int reg)
{
	WvpIoctlArgs args;
	args.inArgs[0] = reg;
    ioctl( wvpdev_fd, WVPDRV_AC97_READ, &args );
	return args.outArgs[0];
}

void SetAC97Reg(int reg, int value)
{
	WvpIoctlArgs args;
	args.inArgs[0] = reg;
	args.inArgs[1] = value;
    ioctl( wvpdev_fd, WVPDRV_AC97_WRITE, &args );
}

