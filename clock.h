#ifndef CLOCK_H
#define CLOCK_H

#include <stdbool.h>
#include <time.h>

// Initialize time
void init_time_system();

// Get Functions
time_t get_internal_time();
void get_current_time(int *hour, int *minute, int *second);
int get_weekday();

// Set time
void set_clock_time(int weekday, int hour, int minute);

#endif
