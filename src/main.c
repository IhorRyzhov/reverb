/** 
 * @file main.c
 * @brief Play a sine wave for several seconds, pausing in the middle.
*/
#include <stdio.h>
#include <math.h>
#include "portaudio/include/portaudio.h"
#include "liquid-dsp/include/liquid.h"

static float complex cfP_buff1[2048];

static float complex cfP_buff2[2048];

int main(void)
{
    fftplan objPL_pbw = fft_create_plan(2048, cfP_buff1, cfP_buff2, LIQUID_FFT_BACKWARD, 0); //static

	return 0;
}