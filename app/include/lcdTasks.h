#ifndef LCD_TASKS_H
#define LCD_TASKS_H

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "hal/lcd.h"

// note: find out fd from looking at main

// each character 7x5, 1 space between (so 8x6), top right coordinate (x,y), use colors defined in lcdManager.h
void asciiToLcd(int fd, const char* inputStr, int x, int y, int fontsize, uint16_t color);

// center is x,y
void fillCircle(int fd, int x, int y, int radius, uint16_t color);

// start coordinate is top right (x, y)
void fillBar(int fd, int x, int y, int xLen, int yLen, uint16_t color);

// center is x,y
void fillDiamond(int fd, int x, int y, int xDis, int yDis, uint16_t color);

#endif