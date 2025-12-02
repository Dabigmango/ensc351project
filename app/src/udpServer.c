// udp_server.c
#include "udpServer.h"

#define BUF_SIZE 256

static int g_sockfd = -1;
static int g_port = 0;
static pthread_t g_thread;
static bool g_running = false;

// ---------- These will later call your real music code ----------

static void handlePlayCommand(const char *filename)
{
    // TODO: replace printf with your real "play this file" function.
    // e.g. project_play_file(filename);
    printf("[UDP] PLAY %s\n", filename);
}

static void handleStopCommand(void)
{
    // TODO: call your real stop function.
    printf("[UDP] STOP\n");
}

static void handleSetVolCommand(int vol)
{
    // TODO: call your real volume function.
    printf("[UDP] SET_VOL %d\n", vol);
}

// ----------------------------------------------------------------

static void *udpServer_thread(void *arg)
{
    (void)arg;

    struct sockaddr_in cliaddr;
    socklen_t cli_len = sizeof(cliaddr);
    char buf[BUF_SIZE];

    printf("[UDP] Server listening on port %d\n", g_port);

    while (g_running) {
        ssize_t n = recvfrom(g_sockfd, buf, sizeof(buf) - 1, 0,
                             (struct sockaddr *)&cliaddr, &cli_len);
        if (n <= 0) {
            if (!g_running) {
                break; // shutting down
            }
            perror("[UDP] recvfrom");
            continue;
        }

        buf[n] = '\0';

        // strip trailing whitespace
        while (n > 0 && (buf[n-1] == '\n' || buf[n-1] == '\r' ||
                         buf[n-1] == ' '  || buf[n-1] == '\t')) {
            buf[--n] = '\0';
        }

        printf("[UDP] Received: '%s'\n", buf);

        if (strncmp(buf, "PLAY ", 5) == 0) {
            handlePlayCommand(buf + 5);

        } else if (strcmp(buf, "STOP") == 0) {
            handleStopCommand();

        } else if (strncmp(buf, "SET_VOL ", 8) == 0) {
            int vol = 0;
            if (sscanf(buf + 8, "%d", &vol) == 1) {
                handleSetVolCommand(vol);
            } else {
                printf("[UDP] Bad SET_VOL command\n");
            }

        } else if (strncmp(buf, "ADD_PLAYLIST ", 13) ==0){
            const char* playlistName = buf + 13;
            addPlaylist(playlistName);
        } else if (strncmp(buf, "DELETE_PLAYLIST ", 16) ==0){
            const char* playlistName = buf + 16;
            deletePlaylist(playlistName);
        } else if (strncmp(buf, "ADD_SONG ", 9) ==0){
            char temp[256];
            strncpy(temp, buf + 9, sizeof(temp));
            temp[255] = '\0';

            char *playlist = strtok(temp, " ");
            char *song     = strtok(NULL, "");

            printf("%s", song);

            char songName[99];
            strncpy(songName, song, sizeof(songName));
            songName[98] = '\0'; 
            size_t len = strlen(song);
            if (len > 4 && strcmp(song + len - 4, ".mp3") == 0) {
                songName[len - 4] = '\0';
            }
            printf("%s\n", songName);

            if (playlist && song) {
                addSong(findPlaylist(playlist), songName, song);
            }
        } else if (strncmp(buf, "DELETE_SONG ", 12) ==0){
            char temp[256];
            strncpy(temp, buf + 9, sizeof(temp));
            temp[255] = '\0';

            char *playlist = strtok(temp, " ");
            char *song     = strtok(NULL, "");

            printf("%s", song);

            char songName[99];
            strncpy(songName, song, sizeof(songName));
            songName[98] = '\0'; 
            size_t len = strlen(song);
            if (len > 4 && strcmp(song + len - 4, ".mp3") == 0) {
                songName[len - 4] = '\0';
            }
            printf("%s\n", songName);

            if (playlist && song) {
                deleteSong(findPlaylist(playlist), songName);
            }
        } else {
            printf("[UDP] Unknown command\n");
        }
    }

    printf("[UDP] Thread exiting\n");
    return NULL;
}

bool udpServer_start(int port)
{
    if (g_running) {
        return true;
    }

    g_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_sockfd < 0) {
        perror("[UDP] socket");
        return false;
    }

    g_port = port;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port);

    if (bind(g_sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[UDP] bind");
        close(g_sockfd);
        g_sockfd = -1;
        return false;
    }

    g_running = true;
    if (pthread_create(&g_thread, NULL, udpServer_thread, NULL) != 0) {
        perror("[UDP] pthread_create");
        g_running = false;
        close(g_sockfd);
        g_sockfd = -1;
        return false;
    }

    return true;
}

void udpServer_stop(void)
{
    if (!g_running) {
        return;
    }

    g_running = false;
    shutdown(g_sockfd, SHUT_RDWR);   // wake up recvfrom
    pthread_join(g_thread, NULL);
    close(g_sockfd);
    g_sockfd = -1;
}
