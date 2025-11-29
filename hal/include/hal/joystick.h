#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <gpiod.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

// note: using GPIO4 for select

void joystickInit(void);
void joystickClose(void);
int xPos(void);
int yPos(void);
const char* direction(void);
int joystickPressed(void);

#endif