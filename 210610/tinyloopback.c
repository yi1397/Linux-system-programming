#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "wvplib.h"

int check_param(struct pcm_params *params, unsigned int param, unsigned int value,
                 char *param_name, char *param_unit)
{
    unsigned int min;
    unsigned int max;
    int is_within_bounds = 1;

    min = pcm_params_get_min(params, param);
    if (value < min) {
        fprintf(stderr, "%s is %u%s, device only supports >= %u%s\n", param_name, value,
                param_unit, min, param_unit);
        is_within_bounds = 0;
    }

    max = pcm_params_get_max(params, param);
    if (value > max) {
        fprintf(stderr, "%s is %u%s, device only supports <= %u%s\n", param_name, value,
                param_unit, max, param_unit);
        is_within_bounds = 0;
    }

    return is_within_bounds;
}

int sample_is_playable(unsigned int card, unsigned int device, unsigned int channels,
                        unsigned int rate, unsigned int bits, unsigned int period_size,
                        unsigned int period_count)
{
    struct pcm_params *params;
    int can_play;

    params = pcm_params_get(card, device, PCM_OUT);
    if (params == NULL) {
        fprintf(stderr, "Unable to open PCM device %u.\n", device);
        return 0;
    }

    can_play = check_param(params, PCM_PARAM_RATE, rate, "Sample rate", "Hz");
    can_play &= check_param(params, PCM_PARAM_CHANNELS, channels, "Sample", " channels");
    can_play &= check_param(params, PCM_PARAM_SAMPLE_BITS, bits, "Bitrate", " bits");
    can_play &= check_param(params, PCM_PARAM_PERIOD_SIZE, period_size, "Period size", "Hz");
    can_play &= check_param(params, PCM_PARAM_PERIODS, period_count, "Period count", "Hz");

    pcm_params_free(params);

    return can_play;
}

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

struct pcm *pcmInit(unsigned int rate, unsigned int bits, unsigned int flag)
{
    unsigned int device = 0;
    unsigned int card = 0;
    unsigned int period_size = 160*2*2; // 10ms

    struct pcm_config config;
    struct pcm *pcm;
    char *buffer;
	char *samples;
    int size;

	int channels = 2;
	int period_count = flag == PCM_IN ? 2 : 8;

    config.channels = channels;
    config.rate = rate;
    config.period_size = period_size;
    config.period_count = period_count;
    if (bits == 32) config.format = PCM_FORMAT_S32_LE;
    else if (bits == 16) config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    if (!sample_is_playable(card, device, channels, rate, bits, period_size, period_count)) {
        return;
    }

    pcm = pcm_open(card, device, flag, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device %u (%s)\n",
                device, pcm_get_error(pcm));
        return;
    }

	return pcm;
}


void loopback(struct pcm *pcmIn, struct pcm *pcmOut, unsigned int rate, unsigned int bits)
{
    
	// To do!!!
    unsigned int size = pcm_frames_to_bytes(pcmIn, pcm_get_buffer_size(pcmIn));;
    char *buffer;
    buffer = malloc(size);
    printf("size = %d", size);

    while (1)
    {
        pcm_read(pcmIn, buffer, size);

        pcm_write(pcmOut, buffer, size);
    }
}

int main(int argc, char **argv)
{
	int i;
	struct pcm *pcm_in, *pcm_out;

	pcm_in = pcmInit(16000, 16, PCM_IN);
	pcm_out = pcmInit(16000, 16, PCM_OUT);
	InitWvpDrv(); // Init Audio Device Init

	loopback(pcm_in, pcm_out, 16000, 16);

    pcm_close(pcm_out);
    pcm_close(pcm_in);

    return 0;
}
