#include "lcdManager.h"

enum startSelect startSelector;
enum allPages currentPage;

pthread_mutex_t selectorLock = PTHREAD_MUTEX_INITIALIZER;

int selectionChanged = 0;

void startScreen(int fd) {
    // creating start screen
    lcd_fill(fd, BLUE);

    fillCircle(fd, 59, 67, 27, BLACK);
    fillCircle(fd, 119, 67, 27, BLACK);
    fillCircle(fd, 179, 67, 27, BLACK);

    musicPlayerIcon(fd, RED);
    alarmClockIcon(fd, WHITE);
    settingsIcon(fd, WHITE);

    startSelector = MUSIC_PLAYER;
    currentPage = MAIN_SCREEN;
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

void* selector(void* arg) {
    int fd = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&selectorLock);
        int tempSelectionChanged = selectionChanged;
        pthread_mutex_unlock(&selectorLock);
        if (tempSelectionChanged) {
            switch (currentPage) {
                case MAIN_SCREEN:
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
                    break;
                case ALARM_CLOCK_SCREEN:
                    break;
                case SETTINGS_SCREEN:
                    break;
            }
            pthread_mutex_lock(&selectorLock);
            selectionChanged = 0;
            pthread_mutex_unlock(&selectorLock);
            usleep(10000);
        }
    }
    return NULL;
}