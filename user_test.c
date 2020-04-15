#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "my_ds1307_ioctl.h"

#define DEVICE_NODE "/dev/ds1307_dev"

int main(int argc, char *argv[]) {
    int fd = -1;
    char buf[20];

    printf("Open device file\n");

    fd = open(DEVICE_NODE, O_RDWR);
    if(fd < 0) {
        printf("Cannot open the device file\n");
        exit(1);
    }

    int status;

    if(argv[1][0] == 'r' && argc == 2)
    {
        ioctl(fd, DS1307_GET_STATUS, &status);
        printf("Get satus: %d \n", status);
    } else if(argv[1][0] == 'w' && argc == 3)
    {
        status = atoi(argv[2]);
        ioctl(fd, DS1307_SET_STATUS, &status);
        printf("Set satus: %d \n", status);
    } else
    {
        printf("Wrong arguments\n");
    }


    printf("Close device file\n");
    close(fd);

    return 0;
}