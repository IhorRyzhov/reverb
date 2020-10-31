/*
 * input-output-device.c
 *
 *  Created on: 13 июн. 2020 г.
 *      Author: ihor
 */

#include <stdint.h>
#include <stdio.h>
#include "input-output-device.h"

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (8192*6) // 6

static PaStream* objP_stream = NULL;

static PaError s32_err;

static void StreamFinished( void* userData )
{

}

void inputOutputDevice_initDevice(PaStreamCallback* objPL_streamCallback)
{
	PaStreamParameters outputParameters;
	PaStreamParameters inputParameters;

	s32_err = Pa_Initialize();

	outputParameters.device = Pa_GetDefaultOutputDevice();

	if (outputParameters.device == paNoDevice)
	{
		fprintf(stderr, "Error: No default output device.\n");
	}

	inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */

	if (inputParameters.device == paNoDevice)
	{
		fprintf(stderr, "Error: No default input device.\n");
	}

	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paInt16;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	inputParameters.channelCount = 2; /* stereo input */
	inputParameters.sampleFormat = paInt16;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	s32_err = Pa_OpenStream(
		&objP_stream,
		&inputParameters,
		&outputParameters,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,
		objPL_streamCallback,
		NULL);

	s32_err = Pa_SetStreamFinishedCallback(objP_stream, &StreamFinished);
}

void inputOutputDevice_startOutputDevice(void)
{
	s32_err = Pa_StartStream(objP_stream);
}

void inputOutputDevice_stopOutputDevice(void)
{
	s32_err = Pa_StopStream(objP_stream);
}

void inputOutputDevice_deinitDevice(void)
{
	s32_err = Pa_CloseStream(objP_stream);

	Pa_Terminate();
}

