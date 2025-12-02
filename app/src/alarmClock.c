#include "alarmClock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

static Alarm alarms[MAX_ALARMS];
static pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t monitor_thread;
static volatile bool alarm_thread_running = false;

// Internal function prototypes
static bool should_alarm_ring(Alarm *alarm, struct tm *current_time);
static void trigger_alarm(Alarm *alarm);
static void play_alarm_sound(const char *music_path);
static void *alarm_monitor_thread(void *arg);

void init_alarms() {
    pthread_mutex_lock(&alarm_mutex);
    for (int i = 0; i < MAX_ALARMS; i++) {
        alarms[i].id = -1;
        alarms[i].enabled = false;
        alarms[i].is_ringing = false;
        memset(alarms[i].repeat_days, 0, sizeof(alarms[i].repeat_days));
        memset(alarms[i].music_path, 0, sizeof(alarms[i].music_path));
    }
    pthread_mutex_unlock(&alarm_mutex);
}

// --- Alarm Management

void add_alarm(int id, int hour, int minute, bool *repeat_days, const char *music_path) {
    pthread_mutex_lock(&alarm_mutex);
    
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        pthread_mutex_unlock(&alarm_mutex);
        print_message("Error: Invalid time format");
    }
    
    alarms[id].id = id;
    alarms[id].hour = hour;
    alarms[id].minute = minute;
    alarms[id].enabled = true;
    alarms[id].is_ringing = false;
    
    if (repeat_days != NULL) {
        memcpy(alarms[id].repeat_days, repeat_days, sizeof(alarms[id].repeat_days));
    }

    if (music_path != NULL && strlen(music_path) > 0) {
        strncpy(alarms[id].music_path, music_path, MAX_MUSIC_PATH - 1);
    } 
    pthread_mutex_unlock(&alarm_mutex);
    
    print_message("Alarm added");
}

void enable_alarm(int id, bool enabled) {
    pthread_mutex_lock(&alarm_mutex);
    
    alarms[id].enabled = enabled;
    pthread_mutex_unlock(&alarm_mutex);
    if (alarms[id].enabled) {
        print_message("Alarm enabled");
    }
}

void update_alarm(int id, int hour, int minute, bool *repeat_days) {
    pthread_mutex_lock(&alarm_mutex);
    
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        pthread_mutex_unlock(&alarm_mutex);
        print_message("Error: Invalid time format");
        return;
    }
    
    alarms[id].hour = hour;
    alarms[id].minute = minute;
    
    if (repeat_days != NULL) {
        memcpy(alarms[id].repeat_days, repeat_days, sizeof(alarms[id].repeat_days));
    }
    
    pthread_mutex_unlock(&alarm_mutex);
    print_message("Alarm updated");
}

void set_alarm_music(int id, const char *music_path) {
    pthread_mutex_lock(&alarm_mutex);
    
    strncpy(alarms[id].music_path, music_path, MAX_MUSIC_PATH - 1);
    print_message("Music set for alarm");
    
    pthread_mutex_unlock(&alarm_mutex);
}

void stop_alarm(int id) {
    pthread_mutex_lock(&alarm_mutex);
    
    if (id < 0 || id >= MAX_ALARMS || alarms[id].id == -1) {
        pthread_mutex_unlock(&alarm_mutex);
        return;
    }
    
    alarms[id].is_ringing = false;
       
    print_message("Alarm stopped");
    
    bool has_repeat = false;
    for (int i = 0; i < 7; i++) {
        if (alarms[id].repeat_days[i]) {
            has_repeat = true;
            break;
        }
    }
    
    pthread_mutex_unlock(&alarm_mutex);
}

void list_alarms() {
    pthread_mutex_lock(&alarm_mutex);
    
    print_message("=== Alarms ===");
    
    int i = 0;
    alarm_info(alarms[i].id, alarms[i].hour, alarms[i].minute, alarms[i].enabled,
                   alarms[i].is_ringing, alarms[i].repeat_days, alarms[i].music_path);
    int j = 1;
    alarm_info(alarms[j].id, alarms[j].hour, alarms[j].minute, alarms[j].enabled,
                   alarms[j].is_ringing, alarms[j].repeat_days, alarms[j].music_path);               
    
    pthread_mutex_unlock(&alarm_mutex);
}

Alarm* get_alarm(int id) {
    if (id < 0 || id >= MAX_ALARMS || alarms[id].id == -1) {
        return NULL;
    }
    return &alarms[id];
}

/* Alarm Monitering - Internal Functions*/

static bool should_alarm_ring(Alarm *alarm, struct tm *current_time) {
    if (!alarm->enabled || alarm->is_ringing) {return false;}
    if (alarm->hour != current_time->tm_hour || alarm->minute != current_time->tm_min) {return false;}
    
    bool has_repeat = false;
    for (int i = 0; i < 7; i++) {
        if (alarm->repeat_days[i]) {
            has_repeat = true;
            break;
        }
    }
    if (has_repeat) {
        return alarm->repeat_days[current_time->tm_wday];
    }   
    return true;
}

static void trigger_alarm(Alarm *alarm) {
    print_message("ALARM RINGING");
    alarm->is_ringing = true;
    play_alarm_sound(alarm->music_path);
}

static void play_alarm_sound(const char *music_path) {
    char command[512];
    snprintf(command, sizeof(command), "mpg123 -q '%s' &", music_path);
    print_message("Playing alarm music");
}

static void *alarm_monitor_thread(void *arg) {
    (void)arg;
    time_t last_check = 0;
    
    while (alarm_thread_running) {
        time_t now = get_internal_time();
        struct tm *current_time = localtime(&now);
        
        if (current_time->tm_sec <= 1 && now != last_check) {
            last_check = now;
            
            pthread_mutex_lock(&alarm_mutex);
            for (int i = 0; i < MAX_ALARMS; i++) {
                if (alarms[i].id != -1 && should_alarm_ring(&alarms[i], current_time)) {
                    trigger_alarm(&alarms[i]);
                }
            }
            pthread_mutex_unlock(&alarm_mutex);
        }
        sleep(1);
    }
    return NULL;
}

// Alarm Monitering

void start_alarm_monitor() {
    if (alarm_thread_running) {return;}
    
    alarm_thread_running = true;
    if (pthread_create(&monitor_thread, NULL, alarm_monitor_thread, NULL) != 0) {
        print_message("Error: Failed to create alarm monitor thread");
        alarm_thread_running = false;
        return;
    }
}

void stop_alarm_monitor() {
    if (!alarm_thread_running) {return;}
    alarm_thread_running = false;
    pthread_join(monitor_thread, NULL);
}

bool is_alarm_ringing(int id) {
    pthread_mutex_lock(&alarm_mutex);
    
    bool ringing = false;
    if (id >= 0 && id < MAX_ALARMS && alarms[id].id != -1) {
        ringing = alarms[id].is_ringing;
    }
    pthread_mutex_unlock(&alarm_mutex);
    return ringing;
}

int get_ringing_alarm_id() {
    pthread_mutex_lock(&alarm_mutex);
    
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (alarms[i].id != -1 && alarms[i].is_ringing) {
            pthread_mutex_unlock(&alarm_mutex);
            return i;
        }
    }
    
    pthread_mutex_unlock(&alarm_mutex);
    return -1;
}

/*--------- Saving and loading alarms ---------- */

void save_alarms_to_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        print_message("Error: Cannot open file for writing");
        return;
    }
    
    pthread_mutex_lock(&alarm_mutex);
    
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (alarms[i].id != -1) {
            fprintf(file, "%d,%d,%d,%d,", alarms[i].id, alarms[i].hour, 
                    alarms[i].minute, alarms[i].enabled);
            
            for (int j = 0; j < 7; j++) {
                fprintf(file, "%d", alarms[i].repeat_days[j]);
                if (j < 6) fprintf(file, ",");
            }
            
            fprintf(file, ",%s\n", alarms[i].music_path);
        }
    }
    
    pthread_mutex_unlock(&alarm_mutex);
    fclose(file);
    print_message("Alarms saved");
}

void load_alarms_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        print_message("No saved alarms found");
        return;
    }
    
    pthread_mutex_lock(&alarm_mutex);
    
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        int id, hour, min, enabled;
        int repeat[7];
        char music_path[MAX_MUSIC_PATH];
        
        sscanf(line, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%[^\n]",
               &id, &hour, &min, &enabled,
               &repeat[0], &repeat[1], &repeat[2], &repeat[3],
               &repeat[4], &repeat[5], &repeat[6], music_path);
        
        if (id >= 0 && id < MAX_ALARMS) {
            alarms[id].id = id;
            alarms[id].hour = hour;
            alarms[id].minute = min;
            alarms[id].enabled = enabled;
            alarms[id].is_ringing = false;
            
            for (int i = 0; i < 7; i++) {
                alarms[id].repeat_days[i] = repeat[i];
            }
            
            strncpy(alarms[id].music_path, music_path, MAX_MUSIC_PATH - 1);
        }
    }
    
    pthread_mutex_unlock(&alarm_mutex);
    fclose(file);
    print_message("Alarms loaded");
}