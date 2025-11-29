#ifndef _JOYSTICK_INPUT_H
#define _JOYSTICK_INPUT_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "mp3_decoder.h"
#include "bluetooth_manager.h"
#include "musicStorage.h"
#include "lcdTasks.h"
#include "lcdManager.h"
#include "hal/lcd.h"
#include "hal/joystick.h"

void* getJoystick(void* arg);

#endif