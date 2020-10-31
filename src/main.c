#include <stdio.h>
#include "input-output-device/input-output-device.h"

int callback(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{


	return 0;
}

int main(void)
{
	inputOutputDevice_initDevice(callback);

	inputOutputDevice_startOutputDevice();

	while(1)
	{

	}

	printf("Hello world");

	return 0;
}
