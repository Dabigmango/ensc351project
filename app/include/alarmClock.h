#ifndef ALARM_CLOCK_H
#define ALARM_CLOCK_H

#include <stdbool.h>
#include <time.h>

#include "clockDisplay.h"
#include "clock.h"

#define MAX_ALARMS 2
#define MAX_MUSIC_PATH 256

// Alarm structure
typedef struct {
    int id;
    int hour;
    int minute;
    bool enabled;
    bool repeat_days[7]; // 0=Sunday, 1=Monday, ..., 6=Saturday
    char music_path[MAX_MUSIC_PATH];
    bool is_ringing;
} Alarm;

// Initilize
void init_alarms();

// Alarm management
void add_alarm(int id, int hour, int minute, bool *repeat_days, const char *music_path);
void enable_alarm(int id, bool enabled);
void update_alarm(int id, int hour, int minute, bool *repeat_days);
void set_alarm_music(int id, const char *music_path);
void stop_alarm(int id);

Alarm* get_alarm(int id);
void list_alarms();

// Alarm monitoring
void start_alarm_monitor();
void stop_alarm_monitor();
bool is_alarm_ringing(int id);
int get_ringing_alarm_id();

// Persistence
void save_alarms_to_file(const char *filename);
void load_alarms_from_file(const char *filename);

#endif /* ALARM_CLOCK_H */
