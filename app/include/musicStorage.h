#ifndef _MUSIC_STORAGE_H
#define _MUSIC_STORAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp3_decoder.h"

typedef struct {
    char* name;
    char* path;
} Song;

typedef struct {
    char* name;
    Song* songs;
    int numSongs;
    int capacity;
} Playlist;

Playlist* findPlaylist(const char* name);
int findPlaylistIndex(const char* name);
Song* findSong(Playlist* playlist, const char* name);
int findSongIndex(Playlist* playlist, const char* name);
void addSong(Playlist* playlist, const char* name, const char* path);
void viewList(Playlist* playlist);
void deleteSong(Playlist* playlist, const char* name);
void addPlaylist(const char* name);
void viewPlaylists(void);
void deletePlaylist(const char* name);
Playlist* openPlaylist(const char* name);
void getInput(void);
void startup(void);
void freeAll(void);

#endif