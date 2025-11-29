#include "lcdManager.h"

enum startSelect startSelector;
enum allPages currentPage;

pthread_mutex_t selectorLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t updateLock = PTHREAD_MUTEX_INITIALIZER;

int selectionChanged = 0;
int musicPlayerScreenUpdated = 0;

int currentPlaylist = 0;

void startScreen(int fd) {
    // creating start screen
    lcd_fill(fd, BLUE);

    fillCircle(fd, 59, 67, 27, BLACK);
    fillCircle(fd, 119, 67, 27, BLACK);
    fillCircle(fd, 179, 67, 27, BLACK);

    musicPlayerIcon(fd, RED);
    alarmClockIcon(fd, WHITE);
    settingsIcon(fd, WHITE);


}

void musicPlayerIcon(int fd, uint16_t color) {

    // music player icon
    fillCircle(fd, 47, 70, 7, color);
    fillCircle(fd, 69, 70, 7, color);
    fillBar(fd, 49, 50, 5, 20, color);
    fillBar(fd, 71, 50, 5, 20, color);
    fillBar(fd, 49, 50, 27, 5, color);
}

void alarmClockIcon(int fd, uint16_t color) {
    // alarm clock icon
    fillCircle(fd, 119, 67, 22, color);
    fillCircle(fd, 119, 67, 20, 0X0);
    fillBar(fd, 118, 49, 3, 18, color);
    fillBar(fd, 118, 67, 16, 1, color);
}

void settingsIcon(int fd, uint16_t color) {
    // settings icon
    fillCircle(fd, 179, 67, 15, color);
    fillCircle(fd, 179, 67, 9, 0x0);
    fillBar(fd, 175, 46, 8, 6, color);
    fillBar(fd, 158, 63, 6, 8, color);
    fillBar(fd, 175, 82, 8, 6, color);
    fillBar(fd, 194, 63, 6, 8, color);
    fillDiamond(fd, 193, 81, 5, 5, color);
    fillDiamond(fd, 165, 81, 5, 5, color);
    fillDiamond(fd, 165, 53, 5, 5, color);
    fillDiamond(fd, 193, 53, 5, 5, color);
}

void musicPlayerScreen(int fd) {
    lcd_fill(fd, BLUE);
    fillBar(fd, 50, 0, 3, 135, BLACK);
    fillBar(fd, 50, 30, 190, 3, BLACK);
    asciiToLcd(fd, "Playlists", 60, 12, 2, BLACK);
    backButton(fd, WHITE);
}

void backButton(int fd, uint16_t color) {
    fillCircle(fd, 25, 25, 22, BLACK);
    fillBar(fd, 15, 24, 20, 3, color);
    for (int i = 0; i < 4; i++) {
        fillDiamond(fd, 15 + 3*i, 25 - 3*i, 3, 3, color);
    }
    for (int i = 0; i < 4; i++) {
        fillDiamond(fd, 15 + 3*i, 25 + 3*i, 3, 3, color);
    }
}

void* selector(void* arg) {
    int fd = *(int*)arg;
    startScreen(fd);
    startSelector = MUSIC_PLAYER;
    currentPage = MAIN_SCREEN;
    while (1) {

        // selecting display screen
        pthread_mutex_lock(&selectorLock);
        int tempSelectionChanged = selectionChanged;
        pthread_mutex_unlock(&selectorLock);
        if (tempSelectionChanged) {
            switch (currentPage) { 
                case MAIN_SCREEN:
                    startScreen(fd);
                    switch (startSelector) {
                        case MUSIC_PLAYER:
                            musicPlayerIcon(fd, RED);
                            alarmClockIcon(fd, WHITE);
                            settingsIcon(fd, WHITE);
                            break;
                        case ALARM_CLOCK:
                            musicPlayerIcon(fd, WHITE);
                            alarmClockIcon(fd, RED);
                            settingsIcon(fd, WHITE);
                            break;
                        case SETTINGS:
                            musicPlayerIcon(fd, WHITE);
                            alarmClockIcon(fd, WHITE);
                            settingsIcon(fd, RED);
                            break;
                    }
                    break;
                case MUSIC_PLAYER_SCREEN:
                    musicPlayerScreen(fd);
                    if (currentPlaylist == -1) {
                        backButton(fd, RED);
                    }
                    break;
                case ALARM_CLOCK_SCREEN:


                    // ruby try to see what i did for the music player screen and copy it however u see fit
                    // u can make new functions in this file however u want, just label accordingly




                    break;
                case SETTINGS_SCREEN:
                    break;
            }
            pthread_mutex_lock(&selectorLock);
            selectionChanged = 0;
            pthread_mutex_unlock(&selectorLock);
        }

        // update music player screen
        pthread_mutex_lock(&updateLock);
        int tempMusicPlayerUpdated = musicPlayerScreenUpdated;
        pthread_mutex_unlock(&updateLock);
        if (tempMusicPlayerUpdated && (currentPage == MUSIC_PLAYER_SCREEN)) { // display all playlists (that can fit on screen)
            int total = getNumPlaylists();
            int window = 6;

            // center window around current selection
            int start = currentPlaylist - window / 2;

            // clamp window
            if (start < 0) start = 0;
            if (start > total - window) start = total - window;
            if (start < 0) start = 0;

            for (int row = 0; row < window && (start + row) < total; row++) {
                int playlistIndex = start + row;

                uint16_t color = (playlistIndex == currentPlaylist) ? RED : BLACK;

                fillBar(fd, 50, row * 32 + 30, 190, 2, BLACK);
                fillBar(fd, 53, row * 32 + 33, 187, 29, BLUE);

                asciiToLcd(fd, getPlaylistName(playlistIndex), 60, row * 32 + 38, 2, color);
                printf("executing...\n");
            }

            pthread_mutex_lock(&updateLock);
            musicPlayerScreenUpdated = 0;
            pthread_mutex_unlock(&updateLock);
        }


        usleep(10000);
    }
    return NULL;
}