#include <stdio.h>
#include <pthread.h>
#include "mp3_decoder.h"
#include "bluetooth_manager.h"
#include "musicStorage.h"
#include "lcdTasks.h"
#include "lcdManager.h"
#include "joystickInput.h"
#include "hal/lcd.h"
#include "hal/joystick.h"

int main() {
    startup();
    joystickInit();
    int fd = spi_init();
    lcd_init(fd);
    pthread_t lcdThread;
    pthread_t lcdManagerThread;
    pthread_t joystickThread;
    pthread_create(&lcdThread, NULL, sendScreen, &fd); // send fd to thread
    pthread_create(&lcdManagerThread, NULL, selector, &fd);
    pthread_create(&joystickThread, NULL, getJoystick, NULL);

    // real shit
    while (1) {
        getInput();
    }
    pthread_join(joystickThread, NULL);
    pthread_join(lcdManagerThread, NULL);
    pthread_join(lcdThread, NULL);
    lcd_close(fd);
    joystickClose();
    freeAll();
    return 0;
}