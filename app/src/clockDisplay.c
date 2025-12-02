#include "clockDisplay.h"
#include <stdio.h>

static const char *day_names[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", 
    "Thursday", "Friday", "Saturday"
};

void print_current_time_info() {
    int hour, minute, second;
    
    get_current_time(&hour, &minute, &second);
    
    printf("\n=== Current Time Info ===\n");
    printf("Day: %s\n", get_day_name(get_weekday()));
    printf("Time (24h): %02d:%02d\n", hour, minute);

    printf("========================\n\n");
}

void display_clock_menu(int fd) {
    asciiToLcd(fd, "=== Alarm Clock Menu ===", 10, 10, 1, BLACK);
    asciiToLcd(fd, "  0. Go Back", 10, 20, 1, BLACK);
    asciiToLcd(fd, "Time Management:", 10, 30, 1, BLACK);
    asciiToLcd(fd, "  1. Show current time", 10, 40, 1, BLACK);
    asciiToLcd(fd, "  2. Set time", 10, 50, 1, BLACK);
    asciiToLcd(fd, "Alarm Management:", 10, 60, 1, BLACK);
    asciiToLcd(fd, "  3. Add alarm", 10, 70, 1, BLACK);
    asciiToLcd(fd, "  4. List alarms", 10, 80, 1, BLACK);
    asciiToLcd(fd, "  5. Enable/disable alarm", 10, 90, 1, BLACK);
    asciiToLcd(fd, "  6. Stop ringing alarm", 10, 100, 1, BLACK);
    asciiToLcd(fd, "========================", 10, 110, 1, BLACK);
    asciiToLcd(fd, "Choice: ", 10, 120, 1, BLACK);
}

char alarm_info(int id, int hour, int minute, bool enabled, bool is_ringing, bool repeat[7], char music_path) {
    char arr[6];
    sprintf(arr[0], "Alarm: %d",id);
    sprintf(arr[1], "Time: %02d:%02d", hour, minute);
    sprintf(arr[2], "Status: %s", enabled);
    sprintf(arr[3], "Ringing: %s", is_ringing);
    sprintf(arr[4], "Repeat: %s", repeat);
    sprintf(arr[5], "Music: %s", music_path);

}

void print_message(char* chr){
    printf(chr);
}

const char* get_day_name(int weekday) {
    if (weekday < 0 || weekday > 6) return "Invalid";
    return day_names[weekday];
}
