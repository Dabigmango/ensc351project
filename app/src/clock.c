#include "clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "clockDisplay.h"

// Global variables for time display
static pthread_t display_thread;
static volatile bool display_thread_running = false;

void *time_display_thread(void *arg);

// Internal clock offset (seconds difference from system time)
static time_t time_offset = 0;
static pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;

// Day of the week (0=Sunday, 1=Monday, ..., 6=Saturday)
static int day_of_week = 0;
static pthread_mutex_t day_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_time_system() {
    pthread_mutex_lock(&time_mutex);
    time_offset = 0; // Start with no offset (synced to system time)
    pthread_mutex_unlock(&time_mutex);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    pthread_mutex_lock(&day_mutex);
    day_of_week = t->tm_wday;
    pthread_mutex_unlock(&day_mutex);

    display_thread_running = true;
    pthread_create(&display_thread, NULL, time_display_thread, NULL);
}

time_t get_internal_time() {
    pthread_mutex_lock(&time_mutex);
    time_t internal = time(NULL) + time_offset;
    pthread_mutex_unlock(&time_mutex);
    return internal;
}

void get_current_time(int *hour, int *minute, int *second) {
    time_t now = get_internal_time();
    struct tm *t = localtime(&now);
    
    if (hour) *hour = t->tm_hour;
    if (minute) *minute = t->tm_min;
    if (second) *second = t->tm_sec;
}

int get_weekday() {
    pthread_mutex_lock(&day_mutex);
    int day = day_of_week;
    pthread_mutex_unlock(&day_mutex);
    return day;
}

void set_clock_time(int weekday, int hour, int minute) {
    if (weekday < 0 || weekday > 6 ||
        hour < 0 || hour > 23 ||
        minute < 0 || minute > 59) {
        print_message("Error: Invalid date/time values");
        return;
    }

    // Get current system time
    time_t system_time = time(NULL);
    struct tm *sys_tm = localtime(&system_time);
    
    // Create target time
    struct tm target_tm = *sys_tm;
    target_tm.tm_hour = hour;
    target_tm.tm_min = minute;
    target_tm.tm_sec = 0;
    
    time_t target_time = mktime(&target_tm);
    if (target_time == -1) {
        print_message("Error: Invalid time");
        return;
    }

    pthread_mutex_lock(&day_mutex);
    day_of_week = weekday;
    pthread_mutex_unlock(&day_mutex);
    
    pthread_mutex_lock(&time_mutex);
    time_offset = target_time - system_time;
    pthread_mutex_unlock(&time_mutex);

    char str[64];
    sprintf(str, "Clock set to: %s %d:%d", get_day_name(weekday), hour, minute);
    print_message(str);
}

void *time_display_thread(void *arg) {
    (void)arg;
    int last_hour = -1, last_minute = -1;
    
    while (display_thread_running) {
        int hour, minute, second;
        get_current_time(&hour, &minute, &second);
        
        // Display when the minute changes
        if (hour != last_hour || minute != last_minute) {
            last_hour = hour;
            last_minute = minute;
            print_current_time_info();
        }
        
        sleep(1);
    }
    return NULL;
}
