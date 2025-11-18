#ifndef LCD_TASKS_H
#define LCD_TASKS_H

#include <string.h>
#include <unistd.h>
#include "hal/lcd.h"

void asciiToLcd(int fd, const char* inputStr, int x, int y, uint16_t color);

#endif