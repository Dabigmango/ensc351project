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

enum allPages { // add songs screen, add current song screen
    MAIN_SCREEN,
    MUSIC_PLAYER_SCREEN,
    ALARM_CLOCK_SCREEN,
    SETTINGS_SCREEN,
    SONGS_SCREEN,
    PLAYING_SONG
};

enum startSelect {
    MUSIC_PLAYER,
    ALARM_CLOCK,
    SETTINGS
};

enum songFunctions { // maybe add shuffle & replay if have time
    BACK_BUTTON,
    PREVIOUS,
    PAUSING,
    NEXT
};

extern enum allPages currentPage;
extern enum startSelect startSelector;
extern enum songFunctions currentFunction;

extern pthread_mutex_t selectorLock;
extern pthread_mutex_t updateLock;

extern int selectionChanged;
extern int musicPlayerScreenUpdated;
extern int playlistScreenUpdated;

extern int currentPlaylist;
extern int currentSongSelected;
extern int isPlaying;

void startScreen(int fd);
void musicPlayerIcon(int fd, uint16_t color);
void alarmClockIcon(int fd, uint16_t color);
void settingsIcon(int fd, uint16_t color);
void musicPlayerScreen(int fd);
void songsScreen(int fd);
void playScreen(int fd);
void backButton(int fd, uint16_t color);
void playButton(int fd, uint16_t color);
void previousButton(int fd, uint16_t color);
void nextButton(int fd, uint16_t color);


// 
void* selector(void* arg);

#endif