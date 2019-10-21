/*
 * SensorIMU.cpp
 *
 *  Created on: Oct 10, 2019
 *      Author: TA QUOC ANH
 */

#include "SensorIMU.h"
#define ADD_MPU6050 0x68
MPU6050 gyro(ADD_MPU6050);
int count;

static void task_imu(void *param) {
	bool dmpReady = false;  // set true if DMP init was successful
	uint8_t gyroIntStatus;   // holds actual interrupt status byte from gyro
	uint8_t devStatus; // return status after each device operation (0 = success, !0 = error)
	uint16_t packetSize = 0;   // expected DMP packet size (default is 42 bytes)
	uint16_t fifoCount = 0;     // count of all bytes currently in FIFO
	uint8_t fifoBuffer[64]; // FIFO storage buffer
	volatile bool mpuInterrupt = false;
	uint8_t mpuIntStatus;
	// orientation/motion vars
	Quaternion q;           // [w, x, y, z]         quaternion container
	VectorInt16 aa;         // [x, y, z]            accel sensor measurements
	VectorInt16 aaReal; // [x, y, z]            gravity-free accel sensor measurements
	VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
	VectorFloat gravity;    // [x, y, z]            gravity vector
	float euler[3];         // [psi, theta, phi]    Euler angle container
	float ypr[3]; // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

	// packet structure for InvenSense teapot demo
	uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00,
			'\r', '\n' };
	gyro.initialize();
	printf(
			gyro.testConnection() ?
					"gyro6050 connection successful\n" :
					"gyro6050 connection failed\n");
	devStatus = gyro.dmpInitialize();
	gyro.setXGyroOffset(220);
	gyro.setYGyroOffset(76);
	gyro.setZGyroOffset(-85);
	gyro.setZAccelOffset(1788); // 1688 factory default for my test chip
	if (devStatus == 0) {
		// Calibration Time: generate offsets and calibrate our gyro6050
		gyro.CalibrateAccel(6);
		gyro.CalibrateGyro(6);
		gyro.PrintActiveOffsets();
		// turn on the DMP, now that it's ready
		Serial.println(F("Enabling DMP..."));
		gyro.setDMPEnabled(true);

		// set our DMP Ready flag so the main loop() function knows it's okay to use it
		dmpReady = true;

		// get expected DMP packet size for later comparison
		packetSize = gyro.dmpGetFIFOPacketSize();
	} else {
		// ERROR!
		// 1 = initial memory load failed
		// 2 = DMP configuration updates failedh
		// (if it's going to break, usually the code will be 1)
	}
	packetSize = gyro.dmpGetFIFOPacketSize();
	fifoCount = gyro.getFIFOCount();
	printf("size %d \n", packetSize);
	while (1) {
		mpuInterrupt = false;
		mpuIntStatus = gyro.getIntStatus();
		fifoCount = gyro.getFIFOCount();

		if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT))
				|| fifoCount >= 1024) {
			// reset so we can continue cleanly
			gyro.resetFIFO();
			fifoCount = gyro.getFIFOCount();
//			printf("FIFO overflow!");

			// otherwise, check for DMP data ready interrupt (this should happen frequently)
		}

		else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
			// wait for correct available data length, should be a VERY short wait
			while (fifoCount < packetSize)
				fifoCount = gyro.getFIFOCount();

			// read a packet from FIFO
			gyro.getFIFOBytes(fifoBuffer, packetSize);

			// track FIFO count here in case there is > 1 packet available
			// (this lets us immediately read more without waiting for an interrupt)
			fifoCount -= packetSize;
			gyro.dmpGetQuaternion(&q, fifoBuffer);
			gyro.dmpGetAccel(&aa, fifoBuffer);
			gyro.dmpGetGravity(&gravity, &q);
			gyro.dmpGetLinearAccel(&aaReal, &aa, &gravity);
			if (((aaReal.x / 100) * (aaReal.x / 100)
					+ (aaReal.y / 100) * (aaReal.y / 100)) > 120000) {
				count++;

			}

		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

}

Sensor_IMU::Sensor_IMU() {
// TODO Auto-generated constructor stub

}

Sensor_IMU::~Sensor_IMU() {
// TODO Auto-generated destructor stub
}

void Sensor_IMU::init(void) {
	xTaskCreate(task_imu, "IMU task", 4 * 1024, NULL, 1, NULL);

}

int Sensor_IMU::getCount(void) {
	return count;
}
