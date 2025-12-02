#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

#include <stdbool.h>
#include <time.h>
#include "lcdTasks.h"
#include "lcdManager.h"

void print_current_time_info();
void display_clock_menu(int fd);
char* alarm_info(int id, int hour, int minute, bool enabled, bool is_ringing, char music_path);
void print_message(char* ch);

const char* get_day_name(int weekday);

#endif