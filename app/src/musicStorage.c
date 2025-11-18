#include "musicStorage.h"

typedef struct {
    Playlist* playlists;
    int numPlaylists;
    int capacity;
} Library;

static Library* lib;

Playlist* pCurrent = NULL;

Playlist* findPlaylist(const char *name) {
    for (int i = 0; i < lib->numPlaylists; i++) {
        if (strcmp(lib->playlists[i].name, name) == 0) {
            return &lib->playlists[i];
        }
    }
    return NULL;
}

int findPlaylistIndex(const char *name) {
    for (int i = 0; i < lib->numPlaylists; i++) {
        if (strcmp(lib->playlists[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

Song* findSong(Playlist* p, const char *name) {
    for (int i = 0; i < p->numSongs; i++) {
        if (strcmp(p->songs[i].name, name) == 0) {
            return &p->songs[i];
        }
    }
    return NULL;
}

int findSongIndex(Playlist* p, const char *name) {
    for (int i = 0; i < p->numSongs; i++) {
        if (strcmp(p->songs[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}


void addSong(Playlist* playlist, const char* name, const char* path) {
    if (playlist->numSongs >= playlist->capacity) {
        int newCap = playlist->capacity == 0 ? 4 : playlist->capacity * 2;
        playlist->songs = realloc(playlist->songs, newCap * sizeof(Song));
        playlist->capacity = newCap;
    }

    if (findSong(playlist, name) != NULL) {
        printf("Song already added.\n");
        return;
    }
    playlist->songs[playlist->numSongs].name = strdup(name);
    playlist->songs[playlist->numSongs].path = strdup(path);
    playlist->numSongs++;
}

void viewList(Playlist* playlist) {
    if (playlist == NULL) {
        printf("Please open a playlist first. \n");
    }

    for (int i = 0; i < playlist->numSongs; i++) {
        printf("%s\n", playlist->songs[i].name);
    }
}

void deleteSong(Playlist* playlist, const char* name) {
    if (findSong(playlist, name) == NULL) {
        printf("Song does not exist.");
        return;
    }
    int index = findSongIndex(playlist, name);
    free(playlist->songs[index].name);
    free(playlist->songs[index].path);

    for (int i = index; i < playlist->numSongs - 1; i++) {
        playlist->songs[i] = playlist->songs[i + 1];
    }
    playlist->numSongs--;
}

void addPlaylist(const char* nameIn) {
    // double capacity if full
    if (lib->numPlaylists >= lib->capacity) {
        int newCap = lib->capacity == 0 ? 4 : lib->capacity * 2;
        lib->playlists = realloc(lib->playlists, newCap * sizeof(Playlist));
        lib->capacity = newCap;
    }

    if (findPlaylist(nameIn) != NULL) {
        printf("That playlist already exists. \n");
        return;
    }

    // initialize playlist
    lib->playlists[lib->numPlaylists].name = strdup(nameIn);
    lib->playlists[lib->numPlaylists].songs = NULL;
    lib->playlists[lib->numPlaylists].numSongs = 0;
    lib->playlists[lib->numPlaylists].capacity = 0;
    lib->numPlaylists++;
}

void viewPlaylists(void) {
    for (int i = 0; i < lib->numPlaylists; i++) {
        printf("%s\n", lib->playlists[i].name);
    }
}

void deletePlaylist(const char* name) {
    Playlist* p = findPlaylist(name);
    int index = findPlaylistIndex(name);
    if (p != NULL) {
        // free songs in playlist
        for (int j = 0; j < p->numSongs; j++) {
            free(p->songs[j].name);
            free(p->songs[j].path);
        }

        // free playlist
        free(p->name);
        free(p->songs);

        // shift other elements down
        for (int i = index; i < lib->numPlaylists - 1; i++) {
            lib->playlists[i] = lib->playlists[i + 1];
        }

        lib->numPlaylists--;
    }
    else {
        printf("Playlist does not exist.\n");
    }
}

Playlist* openPlaylist(const char* name) {
    return findPlaylist(name);
}

void freeAll(void) {
    if (!lib) return;

    // free all playlists in library
    for (int i = 0; i < lib->numPlaylists; i++) {
        for (int j = 0; j < lib->playlists[i].numSongs; j++) {
            free(lib->playlists[i].songs[j].name);
            free(lib->playlists[i].songs[j].path);
        }
        free(lib->playlists[i].name);
        free(lib->playlists[i].songs);
    }
    free(lib->playlists);
    free(lib);
    lib = NULL;
}

void getInput(void) {
    int inputNum;
    char inputName[100];
    char inputPath[100];
    printf("What would you like to do?\n");
    scanf("%d", &inputNum);
    switch (inputNum) {
        case 0: // add playlist
            printf("What would you like to name the playlist? \n");
            scanf("%99s", inputName);
            if (findPlaylist(inputName) == NULL) {
                addPlaylist(inputName);
                break;
            }
            else {
                printf("That name already exists. \n");
                break;
            }
        case 1: // view playlists
            viewPlaylists();
            break;
        case 2: // open playlist
            printf("Which playlist would you like to open? \n");
            scanf("%99s", inputName);
            if (findPlaylist(inputName) == NULL) {
                printf("That playlist does not exist. \n");
                break;
            }
            else {
                pCurrent = findPlaylist(inputName);
                break;
            }
        case 3: // delete playlist
            printf("Which playlist would you like to delete? \n");
            scanf("%99s", inputName);
            deletePlaylist(inputName);
            break;
        case 4: // view playlist
            viewList(pCurrent);
            break;
        case 5: // add song
            printf("What song would you like to add? \n");
            printf("Name: ");
            scanf("%99s", inputName);
            printf("Path: ");
            scanf("%99s", inputPath);
            addSong(pCurrent, inputName, inputPath);
            break;
        case 6: // delete song
            printf("What song would you like to delete? \n");
            scanf("%99s", inputName);
            deleteSong(pCurrent, inputName);
            break;
        case 9:
            freeAll();
            exit(0);
    }

} 

void startup(void) {
    printf("These are the controls for the music player: \n");
    printf("Press 0 to create a new playlist\n");
    printf("Press 1 to see your playlists\n");
    printf("Press 2 to open a playlist\n");
    printf("Press 3 to delete a playlist\n");
    printf("Press 4 to view the current playlist\n");
    printf("Press 5 to add a song to the playlist\n");
    printf("Press 6 to delete a song from the playlist\n");
    printf("Press 9 to quit\n");
    lib = malloc(sizeof(Library));
    lib->numPlaylists = 0;
    lib->capacity = 10;
    lib->playlists = malloc(lib->capacity * sizeof(Playlist));
}