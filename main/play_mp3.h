/*
 * play_mp3.h
 *
 *  Created on: Sep 16, 2019
 *      Author: TA QUOC ANH
 */

#ifndef MAIN_PLAY_MP3_HHHH_
#define MAIN_PLAY_MP3_HHHH_





#ifdef __cplusplus
extern "C" {
#endif

#include "esp_log.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_mem.h"
#include "audio_common.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"
#include "filter_resample.h"
#include "board.h"
#include "esp_peripherals.h"
#include "periph_button.h"


typedef enum {
	PLAYING_RING = 1, PLAYING_MUSIC,
} state_runing_t;

typedef enum {
	RING = 1, MUSIC,
} name_mp3_t;

void init_play_mp3(void);

void play_mp3_file(const uint8_t *param_start, const uint8_t *param_end);

void start_music(name_mp3_t name_param);

void pause_music_playing(void);

void resume_music(name_mp3_t name_param);

state_runing_t get_state_playing(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_PLAY_MP3_HHHH_ */
