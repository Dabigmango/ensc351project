#include <stdio.h>
#include <pthread.h>
#include "mp3_decoder.h"
#include "bluetooth_manager.h"
#include "musicStorage.h"
#include "lcdTasks.h"
#include "hal/lcd.h"

int main() {
    startup();
    int fd = spi_init();
    lcd_init(fd);
    pthread_t lcdThread;
    pthread_create(&lcdThread, NULL, sendScreen, &fd); // send fd to thread

    // just some testing
    lcd_fill(fd, 0x1F);
    test(fd);
    usleep(100000);
    lcd_fill(fd, 0xF800);
    test(fd);
    usleep(100000);
    asciiToLcd(fd, "AAAAAAAAAAAAAAAA", 0, 0, 0xFFFF);
    test(fd);

    // real shit
    while (1) {
        getInput();
    }
    pthread_join(lcdThread, NULL);
    lcd_close(fd);
    return 0;
}