#include "joystickInput.h"

void* getJoystick(void* arg) {
    (void)arg;
    while (1) {
        const char* dir = direction();
        if (!strcmp(dir, "left")) {
            if (currentPage == MAIN_SCREEN) {
                startSelector = (startSelector - 1 + 3) % 3;
            }
            pthread_mutex_lock(&selectorLock);
            selectionChanged = 1;
            pthread_mutex_unlock(&selectorLock);
            usleep(500000);
        }
        else if (!strcmp(dir, "right")) {
            if (currentPage == MAIN_SCREEN) {
                startSelector = (startSelector + 1) % 3;
            }
            pthread_mutex_lock(&selectorLock);
            selectionChanged = 1;
            pthread_mutex_unlock(&selectorLock);
            usleep(500000);
        }
        else {
            usleep(10000);
        }
    }
    return NULL;
}