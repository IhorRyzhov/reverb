#ifndef CONVOLVER_CONVOLVER_H_
#define CONVOLVER_CONVOLVER_H_

#include <complex.h>
#include <stdint.h>

#define FFT_MAX_LENGTH ((uint64_t)(262144)) // transform size
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (8192*6) // 6
#define TABLE_SIZE   (8192*6) // 6
#define IMPULSE_RESPONSE_LENGTH_MAX (FFT_MAX_LENGTH - TABLE_SIZE + 1)

void reverb_init(const char* cPL_nameOfFile);

void reverb_loadImpulseResponse(const char* cPL_nameOfFile);

float complex* reverb_unloadImpulseResponse(uint64_t* u64PL_samplesRead);

float complex* reverb_getPointerToInputBuffer(void);

void reverb_processChunkOfSamples(float complex* cfL_outputSamples, uint32_t* u32PL_samplesProcessed);

void reverb_setReverbAmount(uint8_t u8L_reverbAmount);

uint8_t reverb_getReverbAmount(void);

#endif
