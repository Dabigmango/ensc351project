#ifndef _LCD_MANAGER_H
#define _LCD_MANAGER_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include "mp3_decoder.h"
#include "bluetooth_manager.h"
#include "musicStorage.h"
#include "lcdTasks.h"
#include "hal/lcd.h"

// define macros for colors
#define BLACK 0x0
#define WHITE 0xFFFF
#define RED 0xF800
#define YELLOW 0xFFE0
#define PURPLE 0xF81F
#define BLUE 0x001F
#define CYAN 0x07FF
#define GREEN 0x07E0  

enum allPages {
    MAIN_SCREEN,
    MUSIC_PLAYER_SCREEN,
    ALARM_CLOCK_SCREEN,
    SETTINGS_SCREEN
};

enum startSelect {
    MUSIC_PLAYER,
    ALARM_CLOCK,
    SETTINGS
};

extern enum allPages currentPage;
extern enum startSelect startSelector;

extern pthread_mutex_t selectorLock;

extern int selectionChanged;

void startScreen(int fd);
void musicPlayerIcon(int fd, uint16_t color);
void alarmClockIcon(int fd, uint16_t color);
void settingsIcon(int fd, uint16_t color);
void* selector(void* arg);

#endif