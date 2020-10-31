
#include <stddef.h>
#include "reverb.h"
#include "../liquidDsp/liquid.h"
#include "../wav-reader/wav-reader.h"

#define cmulf(c1, c2)  ((crealf(c1)*crealf(c2) - cimagf(c1)*cimagf(c2)) + (crealf(c1)*cimagf(c2) + cimagf(c1)*crealf(c2)) * I)

/**
 * Is used to load impulse response in reverb first time.
 * Then is used to receive input data chunk.
 */
float complex cfP_buff1[FFT_MAX_LENGTH];

float complex cfP_buff2[FFT_MAX_LENGTH];

float complex cfP_buff3[FFT_MAX_LENGTH];

/**
 * Impulse response in frequency domain
 */
float complex cfP_irL[FFT_MAX_LENGTH];

/**
 * Impulse response in frequency domain
 */
float complex cfP_irR[FFT_MAX_LENGTH];


static fftplan objP_pf = NULL;

static fftplan objP_pfw = NULL;

static float complex cf_reverbAmount = 0.5; // 50% reverb


void reverb_init(const char* cPL_nameOfFile)
{
	for (uint64_t i = 0; i < FFT_MAX_LENGTH; i++)
	{
		cfP_buff1[i] = 0;
		cfP_buff2[i] = 0;
		cfP_buff3[i] = 0;
		cfP_irL[i] = 0;
		cfP_irR[i] = 0;
	}

	reverb_loadImpulseResponse(cPL_nameOfFile);

	if (objP_pf == NULL)
	{
		objP_pf = fft_create_plan(FFT_MAX_LENGTH, cfP_buff1, cfP_buff2, LIQUID_FFT_FORWARD, 0); // static is needed
	}

	if (objP_pfw == NULL)
	{
		objP_pfw = fft_create_plan(FFT_MAX_LENGTH, cfP_buff3, cfP_buff2, LIQUID_FFT_BACKWARD, 0); //static
	}
}

void reverb_loadImpulseResponse(const char* cPL_nameOfFile)
{
	wawReader_openStream(cPL_nameOfFile, eStreamChannelModeStereo);

	wawReader_readStream(cfP_buff1, IMPULSE_RESPONSE_LENGTH_MAX);

	wawReader_closeStream();

	/* Transform impulse response from time domain to frequency domain and store it to irFreqL, irFreqR arrays
	 * Using tempBuff buffer as temporarily buffer
	 */

	fftplan pf_t = fft_create_plan((unsigned int)FFT_MAX_LENGTH, cfP_buff1, cfP_buff2, LIQUID_FFT_FORWARD, 0);
	fft_execute(pf_t);
	fft_destroy_plan(pf_t);

	cfP_irR[0] = crealf(cfP_buff2[0]) + 0 * I;
	cfP_irL[0] = cimagf(cfP_buff2[0]) + 0 * I;

	for(uint64_t i = 1; i < FFT_MAX_LENGTH; i++)
	{
		cfP_irR[i] = (crealf(cfP_buff2[FFT_MAX_LENGTH-i]) + crealf(cfP_buff2[i])) + (cimagf(cfP_buff2[i]) - cimagf(cfP_buff2[FFT_MAX_LENGTH-i])) * I; // right
		cfP_irR[i] /= 2;
		cfP_irL[i] = (cimagf(cfP_buff2[FFT_MAX_LENGTH-i]) + cimagf(cfP_buff2[i])) + (crealf(cfP_buff2[FFT_MAX_LENGTH-i]) - crealf(cfP_buff2[i])) * I; // left
		cfP_irL[i] /= 2;
	}

	for(uint64_t i = 0; i < FFT_MAX_LENGTH; i++)
	{
		cfP_buff1[i] = 0 + 0 * I;
		cfP_buff2[i] = 0 + 0 * I;
		cfP_irR[i] /= FFT_MAX_LENGTH;
		cfP_irL[i] /= FFT_MAX_LENGTH;
	}
}

float complex* reverb_unloadImpulseResponse(uint64_t* u64PL_samplesRead)
{
	for (uint64_t i = 0; i < FFT_MAX_LENGTH; i++)
	{
		cfP_buff1[i] = (crealf(cfP_irR[i]) - cimagf(cfP_irL[i])) + (cimagf(cfP_irR[i]) + crealf(cfP_irL[i])) * I;
	}

	fftplan objPL_pbw = fft_create_plan(FFT_MAX_LENGTH, cfP_buff1, cfP_buff2, LIQUID_FFT_BACKWARD, 0); //static
	fft_execute(objPL_pbw);
	fft_destroy_plan(objPL_pbw);

	(*u64PL_samplesRead) = FFT_MAX_LENGTH;

	return cfP_buff2;
}

float complex* reverb_getPointerToInputBuffer(void)
{
	return &cfP_buff1[IMPULSE_RESPONSE_LENGTH_MAX - 1];
}

void reverb_processChunkOfSamples(float complex* cfL_outputSamples, uint32_t* u32PL_samplesProcessed)
{
	float complex cfL_tempLeft, cfL_tempRight;
	uint64_t u64L_arg = FFT_MAX_LENGTH - 1;

	// load input samples in cfP_buff1 to position &irStereo[impulseResponseSize - 1]

	fft_execute(objP_pf);

	for(uint64_t i = 0; i < FFT_MAX_LENGTH; i++)
	{
		cfP_buff2[i] /= (FFT_MAX_LENGTH * 3);
	}

	cfL_tempRight = crealf(cfP_buff2[0]) + 0 * I;
	cfL_tempLeft = cimagf(cfP_buff2[0]) + 0 * I;

	cfL_tempRight = cmulf(cfL_tempRight, cfP_irR[0]);
	cfL_tempLeft = cmulf(cfL_tempLeft, cfP_irL[0]);

	cfP_buff3[0] = (crealf(cfL_tempRight) - cimagf(cfL_tempLeft)) + (cimagf(cfL_tempRight) + crealf(cfL_tempLeft)) * I;

	for (uint64_t i = 1; i < FFT_MAX_LENGTH; i++)
	{
		cfL_tempRight = (crealf(cfP_buff2[FFT_MAX_LENGTH-i]) + crealf(cfP_buff2[i])) + (cimagf(cfP_buff2[i]) - cimagf(cfP_buff2[FFT_MAX_LENGTH-i])) * I; //right
		cfL_tempRight /= 2;
		cfL_tempLeft = (cimagf(cfP_buff2[FFT_MAX_LENGTH-i]) + cimagf(cfP_buff2[i])) + (crealf(cfP_buff2[FFT_MAX_LENGTH-i]) - crealf(cfP_buff2[i])) * I; // left
		cfL_tempLeft /= 2;

		cfL_tempRight = cmulf(cfL_tempRight, cfP_irR[i]);
		cfL_tempLeft = cmulf(cfL_tempLeft, cfP_irL[i]);

		cfP_buff3[i] = (crealf(cfL_tempRight) - cimagf(cfL_tempLeft)) + (cimagf(cfL_tempRight) + crealf(cfL_tempLeft)) * I;

		u64L_arg--;
	}

	uint64_t offset = TABLE_SIZE;
	uint64_t offsetOutput = IMPULSE_RESPONSE_LENGTH_MAX - 1;
	uint64_t range = FFT_MAX_LENGTH - TABLE_SIZE;

	fft_execute(objP_pfw);

	for (uint64_t i = 0; i < TABLE_SIZE; i++)
	{
		cfL_outputSamples[i] = cfP_buff2[offsetOutput++] * cf_reverbAmount + // processed samples
			cfP_buff1[IMPULSE_RESPONSE_LENGTH_MAX - 1 + i]; // input samples
	}

	for (uint64_t i = 0; i < range; i++)
	{
		cfP_buff1[i] = cfP_buff1[offset++];
	}
}

void reverb_setReverbAmount(uint8_t u8L_reverbAmount)
{
	cf_reverbAmount = ((float)u8L_reverbAmount) / 100.0;
}

uint8_t reverb_getReverbAmount(void)
{
	return (uint8_t)(cf_reverbAmount * 100);
}

