#ifndef LCD_TASKS_H
#define LCD_TASKS_H

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "hal/lcd.h"

void asciiToLcd(int fd, const char* inputStr, int x, int y, uint16_t color);
void fillCircle(int fd, int x, int y, int radius, uint16_t color);
void fillBar(int fd, int x, int y, int xLen, int yLen, uint16_t color);
void fillDiamond(int fd, int x, int y, int xDis, int yDis, uint16_t color);

#endif