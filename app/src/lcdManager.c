#include "lcdManager.h"

enum startSelect startSelector;
enum allPages currentPage;
enum songFunctions currentFunction;

pthread_mutex_t selectorLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t updateLock = PTHREAD_MUTEX_INITIALIZER;

int selectionChanged = 0;
int musicPlayerScreenUpdated = 0;
int playlistScreenUpdated = 0;

int currentPlaylist = 0;
int currentSongSelected = 0;
int isPlaying = 0;

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
    fillBar(fd, 50, 0, 3, 134, BLACK);
    fillBar(fd, 50, 30, 190, 3, BLACK);
    asciiToLcd(fd, "Playlists", 60, 12, 2, BLACK);
    backButton(fd, WHITE);
}

void songsScreen(int fd) {
    lcd_fill(fd, BLUE);
    fillBar(fd, 50, 0, 3, 134, BLACK);
    fillBar(fd, 50, 30, 190, 3, BLACK);
    asciiToLcd(fd, getPlaylistName(currentPlaylist), 60, 12, 2, BLACK);
    backButton(fd, WHITE);
}

void playScreen(int fd) {
    lcd_fill(fd, BLUE);
    fillBar(fd, 50, 0, 3, 134, BLACK);
    fillBar(fd, 50, 30, 190, 3, BLACK);
    const char* songPlaying = getSongName(getPlaylistName(currentPlaylist), currentSongSelected);
    char shorted[11];
    strncpy(shorted, songPlaying, 7);
    shorted[7] = '.';
    shorted[8] = '.';
    shorted[9] = '.';
    shorted[10] = '\0';
    asciiToLcd(fd, shorted, 60, 12, 2, BLACK);
    backButton(fd, WHITE);
    playButton(fd, WHITE);
    previousButton(fd, WHITE);
    nextButton(fd, WHITE);
}

void playButton(int fd, uint16_t color) {
    if (isPlaying) {
        fillBar(fd, 141, 50, 3, 15, color);
        fillBar(fd, 148, 50, 3, 15, color);
    } else {
        for (int i = 0; i <= 5; i++) {
            fillBar(fd, 141 + 2*i, 50 + i, 2, 15 - 2*i, color);
        }
    }
}

void previousButton(int fd, uint16_t color) {
    fillBar(fd, 94, 50, 3, 15, color);
    for (int i = 0; i <= 5; i++) {
        fillBar(fd, 97 + 2*i, 57 - i, 2, 1+2*i, color);
    }
}

void nextButton(int fd, uint16_t color) {
    for (int i = 0; i <= 5; i++) {
        fillBar(fd, 189 + 2*i, 50 + i, 2, 15 - 2*i, color);
    }
    fillBar(fd, 199, 50, 3, 15, color);
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
    currentFunction = PAUSING;
    while (1) {

        // selecting display screen
        pthread_mutex_lock(&selectorLock);
        int tempSelectionChanged = selectionChanged;
        pthread_mutex_unlock(&selectorLock);
        if (tempSelectionChanged) {

            // currentPage switches between several pages
            // so mainscreen has the musicplayer icon, alarmclock icon, and settingsicon
            // im using startSelector to say which icon i am currently hovering
            // and i mark it as red
            // then in joystickInput.c if the icon gets pressed/swapped i set selectionChanged to 1
            // so this code knows to swap pages
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
                    musicPlayerScreenUpdated = 1;
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
                case SONGS_SCREEN:
                    songsScreen(fd);
                    playlistScreenUpdated = 1;
                    if (currentSongSelected == -1) {
                        backButton(fd, RED);
                    }
                    break;
                case PLAYING_SONG:
                    playScreen(fd);
                    switch (currentFunction) {
                        case BACK_BUTTON:
                            backButton(fd, RED);
                            break;
                        case PAUSING:
                            playButton(fd, RED);
                            break;
                        case PREVIOUS:
                            previousButton(fd, RED);
                            break;
                        case NEXT:
                            nextButton(fd, RED);
                            break;
                    }
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

            // all this code is to be able to scroll up and down
            int total = getNumPlaylists();
            int window = 3;

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

                char shorted[11];
                strncpy(shorted, getPlaylistName(playlistIndex), 7);
                shorted[7] = '.';
                shorted[8] = '.';
                shorted[9] = '.';
                shorted[10] = '\0';

                asciiToLcd(fd, shorted, 60, row * 32 + 38, 2, color);
            }

            pthread_mutex_lock(&updateLock);
            musicPlayerScreenUpdated = 0;
            pthread_mutex_unlock(&updateLock);
        }

        pthread_mutex_lock(&updateLock);
        int tempPlaylistScreenUpdated = playlistScreenUpdated;
        pthread_mutex_unlock(&updateLock);
        if (tempPlaylistScreenUpdated && (currentPage == SONGS_SCREEN)) { // display all songs (that can fit on screen)
            int total = getNumSongs(getPlaylistName(currentPlaylist));
            int window = 3;

            // center window around current selection
            int start = currentSongSelected - window / 2;

            // clamp window
            if (start < 0) start = 0;
            if (start > total - window) start = total - window;
            if (start < 0) start = 0;

            for (int row = 0; row < window && (start + row) < total; row++) {
                int songIndex = start + row;

                uint16_t color = (songIndex == currentSongSelected ? RED : BLACK);

                fillBar(fd, 50, row * 32 + 30, 190, 2, BLACK);
                fillBar(fd, 53, row * 32 + 33, 187, 29, BLUE);

                char shorted[11];
                strncpy(shorted, getSongName(getPlaylistName(currentPlaylist), songIndex), 7);
                shorted[7] = '.';
                shorted[8] = '.';
                shorted[9] = '.';
                shorted[10] = '\0';

                asciiToLcd(fd, shorted, 60, row * 32 + 38, 2, color);
            }

            pthread_mutex_lock(&updateLock);
            playlistScreenUpdated = 0;
            pthread_mutex_unlock(&updateLock);
        }


        usleep(10000);
    }
    return NULL;
}