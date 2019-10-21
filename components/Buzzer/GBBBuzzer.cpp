/*
 * GBBBuzzer.cpp
 *
 *  Created on: Mar 6, 2019
 *      Author: TA QUOC ANH
 */

#include <freertos/FreeRTOS.h>

#include <freertos/task.h>
#include "freertos/semphr.h"
#include "GBBBuzzer.h"

static void runTaskSound(void *param);

SemaphoreHandle_t isCompletedSound;

xQueueHandle Queue_Recive_Midi;

GBBBuzzer::GBBBuzzer() {
	// TODO Auto-generated constructor stub
	Queue_Recive_Midi = xQueueCreate(2, sizeof(Recive_Midi));
	isCompletedSound = xSemaphoreCreateBinary();
	xTaskCreate(runTaskSound, "runtask", 6000, NULL, 4, NULL);

}

PWM speaker(GPIO_NUM_13); // backup

void GBBBuzzer::playFile(Recive_Midi *param) {

	for (int i = 0; i < param->num_of_note; i++) {
		playNote(param->p_midi_data[i].note, param->p_midi_data[i].length_note);
	}
}

void GBBBuzzer::playMidiSound(midi_data *param, int length) {
	static Recive_Midi Var;
	Var.p_midi_data = param;
	Var.num_of_note = length;
	xQueueSend(Queue_Recive_Midi, &Var, 0);

}

void GBBBuzzer::playNote(notefreqs note, length speed) {
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	if (note == mute) {
		speaker.stop(0);
		vTaskDelayUntil(&xLastWakeTime, 4 * speed);
	} else {
		speaker.setDuty(1000);
		speaker.setFrequency(note);
		vTaskDelayUntil(&xLastWakeTime, 4 * speed);
		speaker.stop(0);
	}
}

void GBBBuzzer::playNoteIndex(int index) {

	switch (index) {
	case 1:
		playMidiSound(note_C, 2);
		break;
	case 2:
		playMidiSound(note_D, 2);
		break;
	case 3:
		playMidiSound(note_E, 2);
		break;
	case 4:
		playMidiSound(note_F, 2);
		break;
	case 5:
		playMidiSound(note_G, 2);
		break;
	case 6:
		playMidiSound(note_A, 2);
		break;
	case 7:
		playMidiSound(note_B, 2);
		break;
	}
}

void GBBBuzzer::playNoteLength(int note, int lenght) {

	static midi_data param_note[2];

	param_note[1].note = mute;

	param_note[1].length_note = not_nua_don;

	switch (note) {
	case 1:
		param_note[0].note = C3;
		break;
	case 2:
		param_note[0].note = D3;
		break;
	case 3:
		param_note[0].note = E3;
		break;
	case 4:
		param_note[0].note = F3;
		break;
	case 5:
		param_note[0].note = G3;
		break;
	case 6:
		param_note[0].note = A3;
		break;
	case 7:
		param_note[0].note = B3;
		break;
	case 8:
		param_note[0].note = C4;
		break;
	default:
		break;
	}

	switch (lenght) {
	case 0:
		param_note[0].length_note = not_don;
		break;
	case 1:
		param_note[0].length_note = not_den;
		break;
	case 2:
		param_note[0].length_note = not_trang;
		break;
	default:
		param_note[0].length_note = not_trang;
		break;
	}

	playMidiSound(param_note, 2);

}

void GBBBuzzer::waitComplete(void) {
	xSemaphoreTake(isCompletedSound, portMAX_DELAY);
}

midi_data happy[24] = { { G3, not_don }, { G3, not_don },

{ A3, not_den }, { G3, not_den }, { C4, not_den },

{ B3, not_trang }, { G3, not_don }, { G3, not_don },

{ A3, not_den }, { G3, not_den }, { D4, not_den },

{ C4, not_trang },

{ G3, not_don }, { G3, not_don }, { F4, not_den }, { E4, not_den }, { C4,
		not_den }, { B3, not_den },

{ G3, not_den },

{ G3, not_don }, { mute, not_den },

{ C4, not_den }, { C4, not_den }, { G3, not_den },

};

midi_data bipbip[8] = { { C4, not_don }, { mute, not_nua_don }, { C4, not_don },
		{ C4, not_den }, { E3, not_den }, { C4, not_den }, { E3, not_den }, {
				C4, not_den }, };
//
//midi_data start[10] = { { E3, not_nua_don }, { mute, not_nua_nua_don }, { G3,
//		not_nua_don }, { mute, not_nua_nua_don }, { C4, not_nua_don }, { mute,
//		not_nua_nua_don }, { C4, not_nua_don }, { mute, not_nua_nua_don }, { C4,
//		not_den}, { mute, not_nua_nua_don } };

midi_data start[12] = { { G4, not_nua_don }, { mute, not_nua_nua_don }, { A4,
		not_nua_don }, { mute, not_nua_nua_don }, { G4, not_don }, { mute,
		not_nua_nua_don }, { G4, not_nua_don }, { mute, not_nua_nua_don }, { E4,
		not_den }, { mute, not_nua_nua_don }, { G4, not_don }, { mute,
		not_nua_nua_don }, };

midi_data connected[5] = { { D4, not_nua_don }, { mute, not_nua_nua_don }, { F4,
		not_nua_don }, { mute, not_nua_nua_don }, { D4, not_nua_don }, };

midi_data disconnected[7] = { { C4, not_nua_don }, { mute, not_nua_nua_don }, {
		C4, not_nua_don }, { mute, not_nua_nua_don }, { C4, not_nua_don }, {
		mute, not_nua_nua_don }, { G3_, not_trang }, };

midi_data chicken_dacne[50] = { { G3, not_nua_don }, { mute, not_nua_nua_don },

{ G3, not_nua_don }, { mute, not_nua_nua_don },

{ A3, not_nua_don }, { mute, not_nua_nua_don },

{ A3, not_nua_don }, { mute, not_nua_nua_don },

{ E3, not_nua_don }, { mute, not_nua_nua_don },

{ E3, not_nua_don }, { mute, not_nua_nua_don },

{ G3, not_don }, { mute, not_nua_nua_don },

{ G3, not_nua_don }, { mute, not_nua_nua_don },

{ G3, not_nua_don }, { mute, not_nua_nua_don },

{ A3, not_nua_don }, { mute, not_nua_nua_don },

{ A3, not_nua_don }, { mute, not_nua_nua_don },

{ E3, not_nua_don }, { mute, not_nua_nua_don },

{ E3, not_nua_don }, { mute, not_nua_nua_don },

{ G3, not_don }, { mute, not_nua_nua_don },

{ G3, not_nua_don }, { mute, not_nua_nua_don },

{ G3, not_nua_don }, { mute, not_nua_nua_don },

{ A3, not_nua_don }, { mute, not_nua_nua_don },

{ A3, not_nua_don }, { mute, not_nua_nua_don },

{ C4, not_nua_don }, { mute, not_nua_nua_don },

{ C4, not_nua_don }, { mute, not_nua_nua_don },

{ B3, not_don }, { mute, not_nua_nua_don },

{ B3, not_don }, { B3, not_nua_nua_don },

{ A3, not_don }, { A3, not_nua_nua_don },

{ G3, not_don }, { G3, not_nua_nua_don },

{ F3, not_don }, { F3, not_nua_nua_don },

};

midi_data good_batt[] = {

};

midi_data bad_batt[] = {

};

midi_data note_C[2] = { { C3, not_don }, { mute, not_nua_nua_don } };
midi_data note_D[2] = { { D3, not_don }, { mute, not_nua_nua_don } };
midi_data note_E[2] = { { E3, not_don }, { mute, not_nua_nua_don } };
midi_data note_F[2] = { { F3, not_don }, { mute, not_nua_nua_don } };
midi_data note_G[2] = { { G3, not_don }, { mute, not_nua_nua_don } };
midi_data note_A[2] = { { A3, not_don }, { mute, not_nua_nua_don } };
midi_data note_B[2] = { { B3, not_don }, { mute, not_nua_nua_don } };

midi_data battery[8] = { { D4, not_nua_don }, { mute, not_nua_nua_don }, { F4,
		not_nua_don }, { mute, not_nua_nua_don }, { D4, not_nua_don }, };

midi_data ota_sound[5] = { { C3, not_nua_nua_don }, { D3, not_nua_nua_don }, { E3,
		not_nua_nua_don }, { F3, not_nua_nua_don }, { G3, not_nua_nua_don }, };

midi_data start_sound[7] =  { { A3, not_don }, { mute, not_don },{ A3, not_nua_don }, { G3,
		not_nua_don }, { A3, not_nua_don }, { B3, not_nua_don },{ D4, not_nua_don } };


GBBBuzzer::~GBBBuzzer() {
	// TODO Auto-generated destructor stub
}

// task run file midi
static void runTaskSound(void *param) {
	Recive_Midi Var;
	for (;;) {

		xQueueReceive(Queue_Recive_Midi, &Var, portMAX_DELAY);
		RobotBuzzer.playFile(&Var);
		xSemaphoreGive(isCompletedSound);
	}
}

GBBBuzzer RobotBuzzer;

