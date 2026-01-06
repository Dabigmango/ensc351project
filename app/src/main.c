#include <stdio.h>
#include <pthread.h>
#include "udpServer.h"
#include "mp3_decoder.h"
#include "mp3_decoder_advanced.h"
#include "bluetooth_manager.h"
#include "musicStorage.h"
#include "lcdTasks.h"
#include "lcdManager.h"
#include "joystickInput.h"
#include "hal/lcd.h"
#include "hal/joystick.h"
#include "alarmClock.h"

int main() {
    startup();
    joystickInit();
    mp3_decoder_advanced_init();
    int fd = spi_init();
    lcd_init(fd);
    init_time_system();
    init_alarms();
    load_alarms_from_file("alarms.dat");
    start_alarm_monitor();
    pthread_t lcdThread;
    pthread_t lcdManagerThread;
    pthread_t joystickThread;
    pthread_create(&lcdThread, NULL, sendScreen, &fd); // send fd to thread
    pthread_create(&lcdManagerThread, NULL, selector, &fd);
    pthread_create(&joystickThread, NULL, getJoystick, NULL);
    udpServer_start(12345);


    udpServer_stop();
    mp3_decoder_advanced_cleanup();
    pthread_join(joystickThread, NULL);
    pthread_join(lcdManagerThread, NULL);
    pthread_join(lcdThread, NULL);
    save_alarms_to_file("alarms.dat");
    stop_alarm_monitor();
    lcd_close(fd);
    joystickClose();
    freeAll();
    return 0;

}
