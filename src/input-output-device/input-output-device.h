/*
 * input-output-device.h
 *
 *  Created on: 13 июн. 2020 г.
 *      Author: ihor
 */

#ifndef INPUT_OUTPUT_DEVICE_INPUT_OUTPUT_DEVICE_H_
#define INPUT_OUTPUT_DEVICE_INPUT_OUTPUT_DEVICE_H_

#include "../third_party/portaudio/include/portaudio.h"

void inputOutputDevice_initDevice(PaStreamCallback* objPL_streamCallback);

void inputOutputDevice_startOutputDevice(void);

void inputOutputDevice_stopOutputDevice(void);

void inputOutputDevice_deinitOutputDevice(void);

#endif /* INPUT_OUTPUT_DEVICE_INPUT_OUTPUT_DEVICE_H_ */
