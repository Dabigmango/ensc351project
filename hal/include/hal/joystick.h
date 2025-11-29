#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

int xPos(void);
int yPos(void);
const char* direction(void);

#endif