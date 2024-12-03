#ifndef INPUT_OUTPUT_DEVICE_INPUT_OUTPUT_DEVICE_H_
#define INPUT_OUTPUT_DEVICE_INPUT_OUTPUT_DEVICE_H_

#include "../portaudio/include/portaudio.h"

void inputOutputDevice_initDevice(PaStreamCallback* objPL_streamCallback);

void inputOutputDevice_deinitDevice(void);

#endif /* INPUT_OUTPUT_DEVICE_INPUT_OUTPUT_DEVICE_H_ */
