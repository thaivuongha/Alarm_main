/*
 * main.c
 *
 *  Created on: Sep 16, 2019
 *      Author: TA QUOC ANH
 */

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
#include "driver/gpio.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "SensorIMU.h"
#include "MPU6050.h"
#include "DS3231.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include "lcd.h"

#include "play_mp3.h"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4
#define NVS_SETTING_ALARM "setting_space"
#define NVS_INFO			"setting_info"

typedef struct {
	int status;
	int hour;
	int minute;
	int config;
} setting_alarm_t;

//#include "MPU6050_6Axis_MotionApps_V6_12.h"
#ifdef __cplusplus
extern "C" {
#endif

extern "C" {
void app_main(void);
}

#define GPIO_INPUT_IO_0     GPIO_NUM_18
#define GPIO_INPUT_IO_1     GPIO_NUM_19
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define SCL_RTC 17
#define SDA_RTC 16

int buffer_status = 0;
int count = 0;

void test_imu(void);
void test_imu_2(void);
static void test_ds3231(void *param);

static void interrupt_alarm(void *param);

static void task_check_alarm(void *param);

static void task_display_time(void *param);

void init_alarm(void *param);

static void task_simalate_button(void *param);

static void task_minus_bufer(void *param);

static void task_control_alarm(void *param);

static void test_play_music(void *param);
void app_main(void) {
	esp_err_t err;
	nvs_handle store_handle;

	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
		printf("erase_flah");
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);
//
//	init_alarm(NULL);
//	xTaskCreate(task_check_alarm, "check alarm", 4 * 1024, NULL, 1, NULL);

//	printf("Button is przxczczssed\n");
//	vTaskDelay(1000 / portTICK_PERIOD_MS);
//	test_play_music(NULL);
	init_play_mp3();

	xTaskCreate(task_minus_bufer, "minus", 2 * 2048, NULL, 1, NULL);
	xTaskCreate(task_control_alarm, "control", 2 * 2048, NULL, 1, NULL);
	xTaskCreate(task_simalate_button, "button", 4 * 2048, NULL, 1, NULL);


//	test_ds3231(NULL);
//	Sensor_IMU imu;
//	imu.init();
//	while (1) {
//		printf(" value %d\n", imu.getCount());
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//	}
//	xTaskCreate(interrupt_alarm, " Check interrupt", 4 * 1024, NULL, 1, NULL);
//	xTaskCreate(test_ds3231, " Alarm", 4 * 1024, NULL, 1, NULL);

//	test_lcd();
//	vTaskDelay(2000 / portTICK_PERIOD_MS);
}

void test_play_music(void *param) {
	init_play_mp3();
	start_music(RING);

	gpio_config_t io_conf;
	io_conf.intr_type = (gpio_int_type_t) GPIO_PIN_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_up_en = (gpio_pullup_t) 1;
	gpio_config(&io_conf);
	while (1) {
		if (gpio_get_level((gpio_num_t) GPIO_INPUT_IO_0) == 0) { // will play music
			vTaskDelay(100 / portTICK_PERIOD_MS);
			if (get_state_playing() != PLAYING_MUSIC) {
				pause_music_playing();
				vTaskDelay(100 / portTICK_PERIOD_MS);
				resume_music(MUSIC);
			}
		} else {
			vTaskDelay(100 / portTICK_PERIOD_MS);
			if (get_state_playing() != PLAYING_RING) {
				pause_music_playing();
				vTaskDelay(100 / portTICK_PERIOD_MS);
				resume_music(RING);
			}
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DS3231 clock_main;

void init_alarm(void *param) {
	esp_err_t err;
	const esp_partition_t *update_partition = NULL;
	size_t size_require;
	nvs_handle my_handle;
	setting_alarm_t setting;
	char *setting_info;
	clock_main.begin();

	clock_main.armAlarm1(false);

	clock_main.clearAlarm1();
	err = nvs_open(NVS_SETTING_ALARM, NVS_READWRITE, &my_handle);

	if (err != ESP_OK) {
		printf("Error Open init alarm;\n");

	} else {
		printf("Open success \n");
		nvs_get_str(my_handle, NVS_INFO, NULL, &size_require);
		printf("%d\n", size_require);
		setting_info = (char*) calloc(size_require, sizeof(char));
		err = nvs_get_str(my_handle, NVS_INFO, setting_info, &size_require);
		printf("%s\n", setting_info);
		nvs_close(my_handle);
		sscanf(setting_info, "%d;%d;%d;%d", &setting.status, &setting.hour,
				&setting.minute, &setting.config);
		clock_main.setAlarm1(0, setting.hour, setting.minute, 30,
				DS3231_MATCH_H_M_S);
	}
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
	display.clearDisplay();
	display.display();
}

void task_simalate_button(void *param) {
	printf("Button is pressed\n");
	gpio_config_t io_conf;
	io_conf.intr_type = (gpio_int_type_t) GPIO_PIN_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
	gpio_config(&io_conf);
	printf("Button is pressed\n");
	while (1) {
		if (gpio_get_level(GPIO_INPUT_IO_0) == 0) { // will play music
			vTaskDelay(100 / portTICK_PERIOD_MS);
			buffer_status++;
			if (buffer_status > 4)
				buffer_status = 4;
			count++;
		} else {
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void task_minus_bufer(void *param) {
	while (1) {
		buffer_status--;
		if (buffer_status < 0)
			buffer_status = 0;
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void task_control_alarm(void *param) {

	// wait semaphore
	while (buffer_status == 0) {
		printf("ring is playing\n");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	printf("ring is suspended\n");
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	while (1) {
		if (buffer_status == 0) {
			printf("ring is playing\n");
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		} else {
			printf("ring is suspended\n");
			if (count > 20) {
				printf("completed\n");
				vTaskSuspend(NULL);
			}
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}

	}
}

void task_check_alarm(void *param) {
	RTCDateTime dt;
	clock_main.clearAlarm1();
	printf("clear alram1\n");
	while (1) {
		dt = clock_main.getDateTime();

		display.clearDisplay();
		display.setTextSize(2);
		display.setTextColor(WHITE);
		display.setCursor(1, 1);
		display.print(dt.hour);
		display.print(":");
		display.print(dt.minute);
		display.print(":");
		display.print(dt.second);
		display.display();
		printf("%d: %d: %d: %d: %d: %d\n", dt.year, dt.month, dt.day, dt.hour,
				dt.minute, dt.second);
		if (clock_main.isAlarm1(true)) {
			printf("ALARM 1 TRIGGERED!\n");
			buffer_status = 3;
			// trigger alarm here
		}
		delay(1000);
	}
}

void task_monitor_action(void *param) {
	while (1) {
		// recive queue from task check alarm

	}
}

#ifdef __cplusplus
}
#endif
