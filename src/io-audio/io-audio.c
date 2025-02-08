#include "io-audio.h"
#include "../libwave/include/wave.h"
#include "../portaudio/include/portaudio.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <semaphore.h>
#include <stdbool.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 256

static WaveFile* objP_file = NULL;
static WaveFile* objP_fileCapture = NULL;

static PaStream* objP_stream = NULL;
static PaError s32_err;

static uint8_t u8_exit = 0;

static sem_t semaphoreBuffersReady;

static int8_t u8P_bufferOutput[BUFFER_SIZE * 4];
static int8_t u8P_bufferInput[BUFFER_SIZE * 4];

static int callback(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{
	memcpy(output, u8P_bufferOutput, BUFFER_SIZE * 4);	
	memcpy(u8P_bufferInput, input, BUFFER_SIZE * 4);

	sem_post(&semaphoreBuffersReady);

	if (statusFlags & paInputUnderflow)
	{
		printf("paInputUnderflow\n");
	}

	if (statusFlags & paInputOverflow)
	{
		printf("paInputOverflow\n");
	}

	if (statusFlags & paOutputUnderflow)
	{
		printf("paOutputUnderflow\n");
	}

	if (statusFlags & paOutputOverflow)
	{
		printf("paOutputOverflow\n");
	}

	if (statusFlags & paPrimingOutput)
	{
		printf("paPrimingOutput\n");
	}

	return paContinue;
}

static void* processData(void* param)
{
	while (0 == u8_exit)
	{
		sem_wait(&semaphoreBuffersReady);

		wave_read(objP_file, u8P_bufferOutput, BUFFER_SIZE);
		wave_write(objP_fileCapture, u8P_bufferInput, BUFFER_SIZE);
	}

	return NULL;
}

void ioAudio_init(void)
{
	const WaveErr* objPL_err = wave_err();
	objP_file = wave_open("sed.wav", WAVE_OPEN_READ);	
	WaveU16 u16L_format = wave_get_format(objP_file);
 	WaveU16 u16L_channnelsQuantity = wave_get_num_channels(objP_file);
 	WaveU32 u16L_sampleRate = wave_get_sample_rate(objP_file);
 	WaveU16 u16L_bitsPerSample = wave_get_valid_bits_per_sample(objP_file);
    size_t s32L_sampleSize = wave_get_sample_size(objP_file);
	
    objP_fileCapture = wave_open("capt.wav", WAVE_OPEN_WRITE);
    wave_set_format(objP_fileCapture, WAVE_FORMAT_PCM);
    wave_set_num_channels(objP_fileCapture, 2);
    wave_set_sample_rate(objP_fileCapture, SAMPLE_RATE);
    wave_set_valid_bits_per_sample(objP_fileCapture, 16);
    wave_set_sample_size(objP_fileCapture, 2);

	sem_init(&semaphoreBuffersReady, 0, 0);

	pthread_t thread;
	pthread_create(&thread, NULL, processData, NULL);

	s32_err = Pa_Initialize();

	PaStreamParameters outputParameters;
	PaStreamParameters inputParameters;
	
	outputParameters.device = Pa_GetDefaultOutputDevice();
	inputParameters.device = Pa_GetDefaultInputDevice();

	const PaDeviceInfo* outputDeviceInfo = Pa_GetDeviceInfo(outputParameters.device);
	const PaDeviceInfo* inputDeviceInfo = Pa_GetDeviceInfo(inputParameters.device);

	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paInt16;
	outputParameters.suggestedLatency = 0.01;//outputDeviceInfo->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	inputParameters.channelCount = 2;
	inputParameters.sampleFormat = paInt16;
	inputParameters.suggestedLatency = 0.01;//inputDeviceInfo->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	s32_err = Pa_OpenStream(
		&objP_stream,
		&inputParameters,
		&outputParameters,
		SAMPLE_RATE,
		BUFFER_SIZE,
		paNoFlag,
		callback,
		NULL);

    s32_err = Pa_StartStream(objP_stream);
}

void ioAudio_deinit(void)
{
	u8_exit = 1;

    s32_err = Pa_StopStream(objP_stream);
    s32_err = Pa_CloseStream(objP_stream);
	Pa_Terminate();

    wave_close(objP_fileCapture);
    wave_close(objP_file);
}