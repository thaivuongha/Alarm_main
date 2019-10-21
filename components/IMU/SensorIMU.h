/*
 * SensorIMU.h
 *
 *  Created on: Oct 10, 2019
 *      Author: TA QUOC ANH
 */

#ifndef MAIN_SENSORIMU_H_
#define MAIN_SENSORIMU_H_
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

#include "Wire.h"
#include "MPU6050.h"

#define SCL_IMU 22
#define SDA_IMU 21

class Sensor_IMU {
public:



	Sensor_IMU();
	virtual ~Sensor_IMU();
	int getCount(void);
	void init(void);


};

#endif /* MAIN_SENSORIMU_H_ */
