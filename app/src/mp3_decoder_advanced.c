#include "mp3_decoder_advanced.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpg123.h>
#include <alsa/asoundlib.h>
#include <pthread.h>

typedef struct {
    mpg123_handle *mh;
    snd_pcm_t *pcm;
    int is_paused;
    int is_playing;
    int volume;
    long current_frame;
    long total_frames;
    pthread_mutex_t mutex;
} playback_state_t;

static playback_state_t state = {
    .mh = NULL,
    .pcm = NULL,
    .is_paused = 0,
    .is_playing = 0,
    .volume = 50,
    .current_frame = 0,
    .total_frames = 0
};

int mp3_decoder_advanced_init(void) {
    int err = mpg123_init();
    if (err != MPG123_OK) {
        printf("Failed to initialize mpg123: %s\n", mpg123_plain_strerror(err));
        return -1;
    }
    state.mh = mpg123_new(NULL, &err);
    if (!state.mh) {
        printf("Failed to create mpg123 handle: %s\n", mpg123_plain_strerror(err));
        return -1;
    }
    pthread_mutex_init(&state.mutex, NULL);
    printf("libmpg123 initialized\n");
    return 0;
}

static int open_alsa(long rate, int channels) {
    int err;
    if ((err = snd_pcm_open(&state.pcm, "plughw:1,0", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Cannot open audio device: %s\n", snd_strerror(err));
        return -1;
    }
    if ((err = snd_pcm_set_params(state.pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                                   channels, rate, 1, 100000)) < 0) {
        printf("Cannot set parameters: %s\n", snd_strerror(err));
        snd_pcm_close(state.pcm);
        return -1;
    }
    printf("ALSA opened: %ld Hz, %d channels\n", rate, channels);
    return 0;
}

int mp3_decoder_load_file(const char *filename) {
    int err;
    long rate;
    int channels, encoding;
    if (!state.mh) return -1;
    err = mpg123_open(state.mh, filename);
    if (err != MPG123_OK) {
        printf("Cannot open file: %s\n", mpg123_strerror(state.mh));
        return -1;
    }
    mpg123_getformat(state.mh, &rate, &channels, &encoding);
    mpg123_format_none(state.mh);
    mpg123_format(state.mh, rate, channels, encoding);
    if (open_alsa(rate, channels) < 0) {
        mpg123_close(state.mh);
        return -1;
    }
    state.total_frames = mpg123_length(state.mh);
    state.current_frame = 0;
    state.is_playing = 0;
    state.is_paused = 0;
    printf("Loaded: %s\n", filename);
    printf("Duration: %.1f seconds\n", (float)state.total_frames / rate);
    return 0;
}

int mp3_decoder_play(void) {
    pthread_mutex_lock(&state.mutex);
    if (!state.mh || !state.pcm) {
        pthread_mutex_unlock(&state.mutex);
        return -1;
    }
    state.is_paused = 0;
    state.is_playing = 1;
    pthread_mutex_unlock(&state.mutex);
    printf("Playing...\n");
    return 0;
}

int mp3_decoder_pause(void) {
    pthread_mutex_lock(&state.mutex);
    state.is_paused = 1;
    pthread_mutex_unlock(&state.mutex);
    printf("Paused\n");
    return 0;
}

int mp3_decoder_stop(void) {
    pthread_mutex_lock(&state.mutex);
    state.is_playing = 0;
    state.is_paused = 0;
    if (state.pcm) {
        snd_pcm_drain(state.pcm);
        snd_pcm_close(state.pcm);
        state.pcm = NULL;
    }
    if (state.mh) {
        mpg123_close(state.mh);
    }
    pthread_mutex_unlock(&state.mutex);
    printf("Stopped\n");
    return 0;
}

int mp3_decoder_set_volume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    pthread_mutex_lock(&state.mutex);
    state.volume = volume;
    pthread_mutex_unlock(&state.mutex);
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "amixer sset Master %d%% 2>/dev/null", volume);
    system(cmd);
    return 0;
}

int mp3_decoder_get_volume(void) {
    int vol;
    pthread_mutex_lock(&state.mutex);
    vol = state.volume;
    pthread_mutex_unlock(&state.mutex);
    return vol;
}

int mp3_decoder_is_playing(void) {
    int playing;
    pthread_mutex_lock(&state.mutex);
    playing = state.is_playing && !state.is_paused;
    pthread_mutex_unlock(&state.mutex);
    return playing;
}

int mp3_decoder_is_paused(void) {
    int paused;
    pthread_mutex_lock(&state.mutex);
    paused = state.is_paused;
    pthread_mutex_unlock(&state.mutex);
    return paused;
}

int mp3_decoder_process(void) {
    unsigned char buffer[8192];
    size_t done = 0;
    int err;
    pthread_mutex_lock(&state.mutex);
    if (state.is_paused || !state.is_playing) {
        pthread_mutex_unlock(&state.mutex);
        struct timespec ts = {0, 10000000};
        nanosleep(&ts, NULL);
        return 0;
    }
    if (!state.mh || !state.pcm) {
        pthread_mutex_unlock(&state.mutex);
        return -1;
    }
    err = mpg123_read(state.mh, buffer, sizeof(buffer), &done);
    if (err == MPG123_OK || err == MPG123_DONE) {
        if (done > 0) {
            short *samples = (short*)buffer;
            size_t sample_count = done / 2;
            float volume_scale = (float)state.volume / 100.0f;
            for (size_t i = 0; i < sample_count; i++) {
                samples[i] = (short)(samples[i] * volume_scale);
            }
            snd_pcm_sframes_t frames = snd_pcm_writei(state.pcm, buffer, done / 4);
            if (frames < 0) {
                frames = snd_pcm_recover(state.pcm, frames, 0);
            }
            state.current_frame = mpg123_tellframe(state.mh);
        }
        if (err == MPG123_DONE) {
            pthread_mutex_unlock(&state.mutex);
            return 1;
        }
    } else {
        printf("Decode error: %s\n", mpg123_strerror(state.mh));
        pthread_mutex_unlock(&state.mutex);
        return -1;
    }
    pthread_mutex_unlock(&state.mutex);
    return 0;
}

int mp3_decoder_get_position(void) {
    int pos = 0;
    pthread_mutex_lock(&state.mutex);
    if (state.total_frames > 0) {
        pos = (int)((state.current_frame * 100) / state.total_frames);
    }
    pthread_mutex_unlock(&state.mutex);
    return pos;
}

void mp3_decoder_advanced_cleanup(void) {
    mp3_decoder_stop();
    if (state.mh) {
        mpg123_delete(state.mh);
        state.mh = NULL;
    }
    mpg123_exit();
    pthread_mutex_destroy(&state.mutex);
    printf("libmpg123 cleaned up\n");
}
