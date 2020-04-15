#ifndef __DS1307_IOCTL
#define __DS1307_IOCTL

#define MAGIC_NUMBER                99
#define DS1307_SET_STATUS           _IOW(MAGIC_NUMBER, 0, int *)
#define DS1307_GET_STATUS           _IOR(MAGIC_NUMBER, 1, int *)
#endif