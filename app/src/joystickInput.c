#include "joystickInput.h"

void* getJoystick(void* arg) {
    (void)arg;
    while (1) {
        const char* dir = direction();
        if (joystickPressed()) {
            pthread_mutex_lock(&selectorLock);
            switch (currentPage) {
                case MAIN_SCREEN:
                    switch (startSelector) {
                        case MUSIC_PLAYER:
                            currentPage = MUSIC_PLAYER_SCREEN;
                            break;
                        case ALARM_CLOCK:
                            currentPage = ALARM_CLOCK_SCREEN;
                            break;
                        case SETTINGS:
                            currentPage = SETTINGS_SCREEN;
                            break;
                    }
                    break;
                case MUSIC_PLAYER_SCREEN:
                    if (currentPlaylist == -1) {
                        currentPage = MAIN_SCREEN;
                    }
                    break;
                case ALARM_CLOCK_SCREEN:



                    // ruby








                    break;
                case SETTINGS_SCREEN:
                    break;
            }
            selectionChanged = 1;
            pthread_mutex_unlock(&selectorLock);
            usleep(500000);
        } 
        else if (!strcmp(dir, "left")) {
            switch (currentPage) {
                case MAIN_SCREEN:
                    startSelector = (startSelector - 1 + 3) % 3;
                    break;
                case MUSIC_PLAYER_SCREEN: 
                    if (currentPlaylist >= 0) {
                        currentPlaylist = -1;
                    }
                    break;
                case ALARM_CLOCK_SCREEN:



                    // ruby








                    break;
                case SETTINGS_SCREEN:
                    break;
            }
            pthread_mutex_lock(&selectorLock);
            selectionChanged = 1;
            pthread_mutex_unlock(&selectorLock);
            usleep(500000);
        }
        else if (!strcmp(dir, "right")) {
            switch (currentPage) {
                case MAIN_SCREEN:
                    startSelector = (startSelector + 1) % 3;
                    break;
                case MUSIC_PLAYER_SCREEN: 
                    if (currentPlaylist < 0) {
                        currentPlaylist = 0;
                    }
                    break;
                case ALARM_CLOCK_SCREEN:




                    // ruby











                    break;
                case SETTINGS_SCREEN:
                    break;
            }
            pthread_mutex_lock(&selectorLock);
            selectionChanged = 1;
            pthread_mutex_unlock(&selectorLock);
            usleep(500000);
        }
        else if (!strcmp(dir, "up")) {
            if (currentPage == MUSIC_PLAYER_SCREEN) {
                pthread_mutex_lock(&selectorLock);
                if (currentPlaylist > 0) {
                    currentPlaylist--;
                    selectionChanged = 1;
                }
                pthread_mutex_unlock(&selectorLock);
            }
            usleep(500000);
        }
        else if (!strcmp(dir, "down")) {
            if (currentPage == MUSIC_PLAYER_SCREEN) {
                pthread_mutex_lock(&selectorLock);
                if (currentPlaylist < getNumPlaylists() - 1) {
                    currentPlaylist++;
                    selectionChanged = 1;
                }
                pthread_mutex_unlock(&selectorLock);
            }
            usleep(500000);
        }

        usleep(10000);
    }
    return NULL;
}