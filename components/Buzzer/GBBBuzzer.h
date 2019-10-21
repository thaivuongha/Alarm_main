/*
 * GBBBuzzer.h
 *
 *  Created on: Mar 6, 2019
 *      Author: TA QUOC ANH
 */

#ifndef COMPONENTS_BUZZER_GBBBUZZER_H_
#define COMPONENTS_BUZZER_GBBBUZZER_H_
#include "PWM.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"
//
//typedef enum {
//	mute = 3,
//	C3   = 130,
//	C3_  = 138,
//	D3   = 146,
//	D3_  = 155,
//	E3   = 165,
//	F3   = 174,
//	F3_  = 185,
//	G3   = 196,
//	G3_  = 207,
//	A3   = 220,
//	A3_  = 233,
//	B3   = 247,
//	C4   = 264,
//	C4_  = 277,
//	D4   = 293,
//	D4_  = 311,
//	E4   = 329,
//	F4   = 349,
//	F4_  = 370,
//	G4   = 392,
//	G4_  = 415,
//	A4   = 440,
//	A4_  = 466,
//
//
//
//}  notefreqs;

typedef enum {
	mute = 3,
	C3 = 1046,
	C3_ = 1108,
	D3 = 1174,
	D3_ = 1244,
	E3 = 1318,
	F3 = 1396,
	F3_ = 1479,
	G3 = 1567,
	G3_ = 1661,
	A3 = 1760,
	A3_ = 1864,
	B3 = 1975,
	C4 = 2093,
	C4_ = 2217,
	D4 = 2349,
	D4_ = 2489,
	E4 = 2637,
	F4 = 2793,
	F4_ = 2959,
	G4 = 3135,
	G4_ = 3322,
	A4 = 3520,
	A4_ = 3729,

} notefreqs;

typedef enum {
	not_nua_nua_don = 1,
	not_nua_don = 2,
	not_don = 4,
	not_den = 8,
	not_trang = 16,
	not_tron = 32
} length;

typedef struct {
	notefreqs note;
	length length_note;
} midi_data;

typedef struct {
	midi_data *p_midi_data;
	int num_of_note;
} Recive_Midi;

class GBBBuzzer {
public:
	GBBBuzzer();

	/**
	 * @brief  Grenerate a sound with tone and length
	 *
	 * @param[in] note Sound tone . It is masured by frequency
	 * @param[in] speed Length of Note
	 *
	 * @return NULL
	 */
	void playNote(notefreqs note, length speed);

	/**
	 * @brief Play file sound with notes
	 *
	 * @param[in] param The midi file to play
	 *
	 * @return NULL
	 */
	void playFile(Recive_Midi *param);

	/**
	 * @brief Play a note with length
	 *
	 * @param[in] param Note to play
	 * @param[in] length Length of note
	 *
	 * @return NULL
	 *
	 */
	void playMidiSound(midi_data *param, int length);

	/**
	 * @brief Play note at index
	 *
	 * @param[in] index Index to play note
	 *
	 * @return NULL
	 */
	void playNoteIndex(int index);

	/**
	 * @brief Play note with length. (Dont use this funtion as a lib function)
	 *
	 */
	void playNoteLength(int note, int lengt);

	/**
	 * @brief Wait until play sound is completed
	 *
	 * @param NULL
	 *
	 * @return NULL
	 */
	void waitComplete(void);

	virtual ~GBBBuzzer();

};

extern GBBBuzzer RobotBuzzer;

extern midi_data happy[24];
extern midi_data bipbip[8];
extern midi_data start[12];
extern midi_data connected[5];
extern midi_data disconnected[7];
extern midi_data battery[8];
extern midi_data chicken_dacne[50];
extern midi_data note_C[2];
extern midi_data note_D[2];
extern midi_data note_E[2];
extern midi_data note_F[2];
extern midi_data note_G[2];
extern midi_data note_A[2];
extern midi_data note_B[2];
extern midi_data ota_sound[5];
extern midi_data start_sound[7];

#endif /* COMPONENTS_BUZZER_GBBBUZZER_H_ */
