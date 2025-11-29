#include "mp3_decoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int play_mp3_file(const char *filename) {
    // Truncate filename for LCD
    char short_name[32];
    if (strlen(filename) > 30) {
        strncpy(short_name, filename, 27);
        short_name[27] = '.';
        short_name[28] = '.';
        short_name[29] = '.';
        short_name[30] = '\0';
    } else {
        strncpy(short_name, filename, 31);
    }
    
    printf("\nPlaying: %s\n", short_name);
    
    // Compact controls for LCD
    printf("\n=== Controls ===\n");
    printf("Space - Pause/Play\n");
    printf("+     - Volume up\n");
    printf("-     - Volume down\n");
    printf("q     - Skip/Previous\n");
    printf("================\n\n");
    
    // Try to play using system audio players
    char command[512];
    
    // Method 1: Try mpg123
    if (system("which mpg123 > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), "mpg123 -q \"%s\"", filename);
        return system(command);
    }
    
    // Method 2: Try paplay with sox conversion
    if (system("which paplay > /dev/null 2>&1 && which sox > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), 
                "sox \"%s\" temp_sound.wav 2>/dev/null && paplay temp_sound.wav && rm temp_sound.wav 2>/dev/null", 
                filename);
        return system(command);
    }
    
    // Method 3: Try aplay with sox conversion
    if (system("which aplay > /dev/null 2>&1 && which sox > /dev/null 2>&1") == 0) {
        snprintf(command, sizeof(command), 
                "sox \"%s\" temp_sound.wav 2>/dev/null && aplay temp_sound.wav && rm temp_sound.wav 2>/dev/null", 
                filename);
        return system(command);
    }
    
    printf("No audio players found!\n");
    return -1;
}
