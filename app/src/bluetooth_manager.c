#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bluetooth_manager.h"

int setup_bluetooth_audio() {
    printf("Setting up Bluetooth audio...\n");
    
    // Run Bluetooth setup commands
    system("sudo systemctl start bluetooth 2>/dev/null");
    sleep(2);
    system("sudo hciconfig hci0 piscan 2>/dev/null");
    system("pulseaudio --start 2>/dev/null");
    system("pactl load-module module-bluetooth-discover 2>/dev/null");
    
    printf("Bluetooth ready. Please connect your Bluetooth speaker manually.\n");
    printf("Use: bluetoothctl\n");
    printf("Then: scan on, pair [MAC], connect [MAC], trust [MAC]\n");
    
    return 0;
}

int set_bluetooth_output() {
    printf("Setting Bluetooth as default audio output...\n");
    system("pactl set-default-sink $(pactl list sinks short | grep bluez | cut -f1) 2>/dev/null");
    return 0;
}

void display_menu() {
    printf("\n=== Audio Output ===\n");
    printf("1. Local speakers\n");
    printf("2. Bluetooth speaker\n");
    printf("3. Setup Bluetooth only\n");
    printf("====================\n");
    printf("Choose (1-3): ");
}

void outputDevice(int audio_choice) {
    display_menu();
    
    if (audio_choice == 2 || audio_choice == 3) {
        setup_bluetooth_audio();
        if (audio_choice == 2) {
            set_bluetooth_output();
            printf("BT audio selected.\n");
        } else {
            printf("BT setup complete.\n");
            return;
        }
    } else if (audio_choice == 1) {
        printf("Local speakers selected.\n");
    } else {
        printf("Invalid. Using local.\n");
    }
    
}