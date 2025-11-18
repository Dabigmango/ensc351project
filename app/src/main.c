#include <stdio.h>
#include "musicStorage.h"
#include "lcdTasks.h"
#include "hal/lcd.h"

int main() {
    startup();
    int fd = spi_init();
    lcd_init(fd);
    lcd_fill(fd, 0x1F);
    usleep(100000);
    lcd_fill(fd, 0xF800);
    asciiToLcd(fd, "AAAAAAAAAAAAAAAA", 0, 0, 0xFFFF);
    while (1) {
        getInput();
    }
    lcd_close(fd);
    return 0;
}