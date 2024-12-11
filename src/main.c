/** 
 * @file main.c
 * @brief
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "portaudio/include/portaudio.h"
#include "liquid-dsp/include/liquid.h"
#include "libwave/include/wave.h"
#include "input-output-device/input-output-device.h"

static uint8_t u8_exit = 0;

void signalHandler(int arg)
{
	printf("signalHandler()\n");
	u8_exit = 1;
}

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (8192)

static PaStream* objP_stream = NULL;
static WaveFile* objP_file = NULL;

static int paStreamCallback(
    const void *input, 
	void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
	int16_t *out = (int16_t*)output;

	size_t s32L_samlesRead = wave_read(objP_file, out, frameCount);

	return paContinue;
}

static void paStreamFinishedCallback(void* userData)
{

}

static void initIo(void)
{
	int32_t s32L_err = Pa_Initialize();

	PaStreamParameters objL_outParam = 
	{
		.channelCount = 2,
		.sampleFormat = paInt16,
		.hostApiSpecificStreamInfo = NULL
	};

	objL_outParam.device = Pa_GetDefaultOutputDevice();
	objL_outParam.suggestedLatency = Pa_GetDeviceInfo(objL_outParam.device)->defaultLowOutputLatency;

	if (objL_outParam.device == paNoDevice)
	{
		fprintf(stderr, "Error: No default output device.\n");
	}

	s32L_err = Pa_OpenDefaultStream(
		&objP_stream,
		0,
		2,
		paInt16,
		44100,
		FRAMES_PER_BUFFER,
		paStreamCallback,
		NULL);

	s32L_err = Pa_SetStreamFinishedCallback(objP_stream, paStreamFinishedCallback);
	s32L_err = Pa_StartStream(objP_stream);

	printf("initIo()\n");
}

static void deinitIo(void)
{
	int32_t s32L_err = Pa_StopStream(objP_stream);
	s32L_err = Pa_CloseStream(objP_stream);
	Pa_Terminate();
	printf("deinitIo()\n");
}


int main(void)
{
	signal(SIGINT, signalHandler);

	const WaveErr* objPL_err = wave_err();

	objP_file = wave_open("sed.wav", WAVE_OPEN_READ);	

	WaveU16 u16L_format = wave_get_format(objP_file);
 	WaveU16 u16L_channnelsQuantity = wave_get_num_channels(objP_file);
 	WaveU32 u16L_sampleRate = wave_get_sample_rate(objP_file);
 	WaveU16 u16L_bitsPerSample = wave_get_valid_bits_per_sample(objP_file);

	initIo();

	while(0 == u8_exit)
	{
		sleep(1);
	}

	wave_close(objP_file);

	deinitIo();

	return 0;
}
