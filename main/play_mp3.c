/* Play mp3 file by audio pipeline

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */

#include "play_mp3.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

static const char *TAG = "PLAY_MP3_FLASH";
/*
 To embed it in the app binary, the mp3 file is named
 in the component.mk COMPONENT_EMBED_TXTFILES variable.
 */

extern const uint8_t ring_mp3_start[] asm("_binary_ring_mp3_start");
extern const uint8_t ring_mp3_end[] asm("_binary_ring_mp3_end");
extern const uint8_t music_mp3_start[] asm("_binary_music_mp3_start");
extern const uint8_t music_mp3_end[] asm("_binary_music_mp3_end");

uint8_t *mp3_start;
uint8_t *mp3_end;
audio_pipeline_handle_t pipeline;
audio_element_handle_t i2s_stream_writer, mp3_decoder;
state_runing_t state_runing;

static int mp3_pos_ring;
static int mp3_pos_music;
static int mp3_pos;
static void play_mp3_task(void);
void play_mp3_file(const uint8_t *param_start, const uint8_t *param_end);

int mp3_music_read_cb(audio_element_handle_t el, char *buf, int len,
		TickType_t wait_time, void *ctx) {

	int read_size = mp3_end - mp3_start - mp3_pos;
	if (read_size == 0) {
		return AEL_IO_DONE;
	} else if (len < read_size) {
		read_size = len;
	}
	memcpy(buf, mp3_start + mp3_pos, read_size);
	mp3_pos += read_size;
	return read_size;
}

void start_ring_alarm(void) {
//	mp3_start = ring_mp3_start;
//	mp3_end = ring_mp3_end;
//	audio_pipeline_run(pipeline);
	printf("start ring \n");
	mp3_pos = 0;
	play_mp3_file(ring_mp3_start, ring_mp3_end);
}

void start_music(name_mp3_t name_param) {

	switch (name_param) {
	case RING:
		mp3_pos = 0;
		state_runing = PLAYING_RING;
		play_mp3_file(ring_mp3_start, ring_mp3_end);
		break;
	case MUSIC:
		mp3_pos = 0;
		state_runing = PLAYING_MUSIC;
		play_mp3_file(music_mp3_start, music_mp3_end);
		break;
	}

}

void pause_music_playing(void) {

	if (state_runing == PLAYING_RING) {
		mp3_pos_ring = mp3_pos;
		printf("pos pause ring  %d \n", mp3_pos_ring);
	} else if (state_runing == PLAYING_MUSIC) {
		mp3_pos_music = mp3_pos;
		printf("pos pause music  %d \n", mp3_pos_music);
	}
	audio_pipeline_pause(pipeline);
}

void resume_music(name_mp3_t name_param) {

	switch (name_param) {

	case RING:
		if (state_runing == PLAYING_RING) {
			return;
		}
		mp3_start = ring_mp3_start;
		mp3_end = ring_mp3_end;
		mp3_pos = mp3_pos_ring;
		state_runing = PLAYING_RING;
		printf("pos resume ring %d \n", mp3_pos);
		break;

	case MUSIC:
		if (state_runing == PLAYING_MUSIC) {
			return;
		}
		mp3_start = music_mp3_start;
		mp3_end = music_mp3_end;
		mp3_pos = mp3_pos_music;
		state_runing = PLAYING_MUSIC;
		printf("pos resume music %d \n", mp3_pos);
		break;
	}

	audio_pipeline_resume(pipeline);
}

state_runing_t get_state_playing(void) {
	return state_runing;
}

void play_mp3_file(const uint8_t *param_start, const uint8_t *param_end) {
	mp3_start = param_start;
	printf("Address start :%d \n", (int) mp3_start);
	mp3_end = param_end;
	printf("Address end: %d \n", (int) mp3_end);
	audio_pipeline_run(pipeline);
}

void init_play_mp3(void) {
	xTaskCreate(play_mp3_task, "task music", 2 * 2048, NULL, 4, NULL);
}

static void play_mp3_task(void) {

	esp_log_level_set("*", ESP_LOG_WARN);
	esp_log_level_set(TAG, ESP_LOG_INFO);
//    ESP_LOGI(TAG, "[ 1 ] Start audio codec chip");
//    audio_board_handle_t board_handle = audio_board_init();
//    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

	ESP_LOGI(TAG,
			"[ 2 ] Create audio pipeline, add all elements to pipeline, and subscribe pipeline event");
	audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
	pipeline = audio_pipeline_init(&pipeline_cfg);
	mem_assert(pipeline);

	ESP_LOGI(TAG,
			"[2.1] Create mp3 decoder to decode mp3 file and set custom read callback");
	mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
	mp3_decoder = mp3_decoder_init(&mp3_cfg);
	audio_element_set_read_cb(mp3_decoder, mp3_music_read_cb, NULL);

	ESP_LOGI(TAG, "[2.2] Create i2s stream to write data to codec chip");
	i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
	i2s_cfg.type = AUDIO_STREAM_WRITER;
	i2s_cfg.i2s_config.sample_rate = 48000;
	i2s_stream_writer = i2s_stream_init(&i2s_cfg);

	i2s_pin_config_t pin_config = { .bck_io_num = 5, .ws_io_num = 25,
			.data_out_num = 26, .data_in_num = -1                    //Not used
			};
	i2s_set_pin(0, &pin_config);

	ESP_LOGI(TAG, "[2.3] Register all elements to audio pipeline");
	audio_pipeline_register(pipeline, mp3_decoder, "mp3");
	audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

	ESP_LOGI(TAG,
			"[2.4] Link it together [mp3_music_read_cb]-->mp3_decoder-->i2s_stream-->[codec_chip]");

	audio_pipeline_link(pipeline, (const char *[] ) { "mp3", "i2s" }, 2);

//	ESP_LOGI(TAG, "[2.5] Init button peripheral");
//	esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
//	esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
//
//	ESP_LOGI(TAG, "[2.6] Init button peripheral");
//	periph_button_cfg_t button_cfg = { .gpio_mask = GPIO_SEL_17 | GPIO_SEL_16
//			| GPIO_SEL_19 | GPIO_SEL_18, .long_press_time_ms = 1000, };
//	esp_periph_handle_t button_periph = periph_button_init(&button_cfg);

//	ESP_LOGI(TAG, "[2.7] Start peripherals");
//	esp_periph_start(set, button_periph);

	ESP_LOGI(TAG, "[ 3 ] Set up  event listener");
	audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
	audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

	ESP_LOGI(TAG, "[3.1] Listening event from all elements of pipeline");
	audio_pipeline_set_listener(pipeline, evt);

//	ESP_LOGI(TAG, "[3.2] Listening event from peripherals");
//	audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

	ESP_LOGI(TAG, "[ 4 ] Start audio_pipeline");

	int temp = 0;
	while (1) {
		audio_event_iface_msg_t msg;
		esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
		printf("have a event \n");
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
			continue;
		}
		if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
				&& msg.source == (void *) mp3_decoder
				&& msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
			audio_element_info_t music_info = { 0 };
			audio_element_getinfo(mp3_decoder, &music_info);

			ESP_LOGI(TAG,
					"[ * ] Receive music info from mp3 decoder, sample_rates=%d, bits=%d, ch=%d",
					music_info.sample_rates, music_info.bits,
					music_info.channels);

			audio_element_setinfo(i2s_stream_writer, &music_info);
			i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates,
					music_info.bits, music_info.channels);
			continue;
		}

		/* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
		if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT
				&& msg.source == (void *) i2s_stream_writer
				&& msg.cmd == AEL_MSG_CMD_REPORT_STATUS
				&& (((int) msg.data == AEL_STATUS_STATE_STOPPED)
						|| ((int) msg.data == AEL_STATUS_STATE_FINISHED))) {
			continue;
		}
	}

	ESP_LOGI(TAG, "[ 5 ] Stop audio_pipeline");
	audio_pipeline_terminate(pipeline);

	audio_pipeline_unregister(pipeline, mp3_decoder);
	audio_pipeline_unregister(pipeline, i2s_stream_writer);

	/* Terminate the pipeline before removing the listener */
	audio_pipeline_remove_listener(pipeline);

	/* Make sure audio_pipeline_remove_listener is called before destroying event_iface */
	audio_event_iface_destroy(evt);

	/* Release all resources */
	audio_pipeline_unregister(pipeline, i2s_stream_writer);
	audio_pipeline_unregister(pipeline, mp3_decoder);
#if (CONFIG_ESP_LYRATD_MSC_V2_1_BOARD || CONFIG_ESP_LYRATD_MSC_V2_2_BOARD)
	audio_pipeline_unregister(pipeline, filter);
	audio_element_deinit(filter);
#endif
	audio_pipeline_deinit(pipeline);
	audio_element_deinit(i2s_stream_writer);
	audio_element_deinit(mp3_decoder);
}
