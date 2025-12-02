#ifndef MP3_DECODER_ADVANCED_H
#define MP3_DECODER_ADVANCED_H

int mp3_decoder_advanced_init(void);
int mp3_decoder_load_file(const char *filename);
int mp3_decoder_play(void);
int mp3_decoder_pause(void);
int mp3_decoder_stop(void);
int mp3_decoder_set_volume(int volume);
int mp3_decoder_get_volume(void);
int mp3_decoder_is_playing(void);
int mp3_decoder_is_paused(void);
int mp3_decoder_get_position(void);
int mp3_decoder_process(void);
void mp3_decoder_advanced_cleanup(void);

#endif
