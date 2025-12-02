#include "joystickInput.h"

void* getJoystick(void* arg) {
    (void)arg;
    while (1) {
        const char* dir = direction();
        if (joystickPressed()) {
            pthread_mutex_lock(&selectorLock);
            switch (currentPage) {

                // tell lcdManager which screen we want to go to
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
                    } else {
                        currentPage = SONGS_SCREEN;
                    }
                    break;
                case ALARM_CLOCK_SCREEN:



                    // ruby








                    break;
                case SETTINGS_SCREEN:
                    break;
                case SONGS_SCREEN:
                    if (currentSongSelected == -1) {
                        currentPage = MUSIC_PLAYER_SCREEN;
                    } else {
                        currentPage = PLAYING_SONG;
                        isPlaying = 1;
                    }
                    break;
                case PLAYING_SONG:
                    switch (currentFunction) { // need function to stop/pause
                        case BACK_BUTTON:
                            currentPage = SONGS_SCREEN;
                            break;
                        case PREVIOUS:
                            isPlaying = 0;
                            currentSongSelected--;
                            break;
                        case NEXT:
                            isPlaying = 0;
                            currentSongSelected++;
                            break;
                        case PAUSING:
                            break;
                    }
                    break;
            }
            selectionChanged = 1;
            pthread_mutex_unlock(&selectorLock);
            usleep(500000);
        } 

        // if move joystick left, do this
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
                case SONGS_SCREEN:
                    if (currentSongSelected >= 0) {
                        currentSongSelected = -1;
                    }
                    break;
                case PLAYING_SONG:
                    currentFunction = (currentFunction - 1 + 4) % 4;
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
                    startSelector = (startSelector + 1) % 4;
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
                case SONGS_SCREEN:
                    if (currentSongSelected < 0) {
                        currentSongSelected = 0;
                    }
                    break;
                case PLAYING_SONG:
                    currentFunction = (currentFunction + 1) % 3;
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
            else if (currentPage == SONGS_SCREEN) {
                pthread_mutex_lock(&selectorLock);
                if (currentSongSelected > 0) {
                    currentSongSelected--;
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
            else if (currentPage == SONGS_SCREEN) {
                pthread_mutex_lock(&selectorLock);
                if (currentSongSelected < getNumSongs(getPlaylistName(currentPlaylist)) - 1) {
                    currentSongSelected++;
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