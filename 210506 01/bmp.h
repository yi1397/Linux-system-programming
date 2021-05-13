#ifndef _BMP_H_
#define _BMP_H_

#define FH_ERROR_OK 0
#define FH_ERROR_FILE 1		/* read/access error */
#define FH_ERROR_FORMAT 2	/* file format error */

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

extern int fh_bmp_id(char *name);
extern int fh_bmp_load(char *name,unsigned char *buffer, int x, int y, unsigned char alpha);
extern int fh_bmp_getsize(char *name,int *x,int *y);

#ifdef __cplusplus
}
#endif

#endif
