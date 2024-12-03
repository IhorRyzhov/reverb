#include <stdio.h>
#include <stdint.h>
#include "wav-reader.h"

struct WavHeader
{ 
    // 0x52494646, "RIFF" in ASCII format
    char cP_chunkId[4];

    /* 36 + subchunk2Size:
    4 + (8 + subchunk1Size) + (8 + subchunk2Size),
	without chunkId и chunkSize. */
    unsigned long u32_chunkSize;

    // 0x57415645, "WAVE"
    char cP_format[4];

    // "fmt ", 0x666d7420
    char cP_subchunk1Id[4];

    // 16 PCM
    unsigned long u32_subchunk1Size;

    // PCM = 1, http://audiocoding.ru/wav_formats.txt
    unsigned short u16_audioFormat;

    // mono = 1, stereo = 2 etc.
    unsigned short u16_numChannels;

    // 8000 Hz, 44100 Hz etc.
    unsigned long u32_sampleRate;

    // sampleRate * numChannels * bitsPerSample / 8
    unsigned long u32_byteRate;

    // numChannels * bitsPerSample / 8
    unsigned short u16_blockAlign;

    // 8 bit, 16 bit etc.
    unsigned short u16_bitsPerSample;

    // "data", 0x64617461
    char cP_subchunk2Id[4];

    // numSamples * numChannels * bitsPerSample / 8
    unsigned long u32_subchunk2Size;
};
typedef struct WavHeader wavHeader_t;


static wavHeader_t obj_wavFileProperties;

static FILE* objP_file;

streamChannelMode_t e_channelMode;


void wawReader_openStream(
	const char* cPL_nameOfFile,
	streamChannelMode_t eL_channelMode)
{
	e_channelMode = eL_channelMode;

	objP_file = fopen(cPL_nameOfFile, "rb");

	fread(&obj_wavFileProperties, sizeof(wavHeader_t), 1, objP_file);

	fseek(objP_file, 256, SEEK_SET); // read addition chunks
}

void wawReader_readStream(
	float complex* fcPL_samples,
	uint64_t u64L_quantity)
{
	int16_t s16L_left;
	int16_t s16L_right;

	switch (e_channelMode)
	{
	case eStreamChannelModeDefault:
		break;

	case eStreamChannelModeLeftMono:
		for (uint64_t i = 0; i < u64L_quantity; i++)
		{
			fread(&s16L_left, sizeof(int16_t), 1, objP_file);
			fseek(objP_file, sizeof(int16_t), SEEK_CUR);

			fcPL_samples[i] = s16L_left + s16L_left * I;
		}
		break;

	case eStreamChannelModeLeftOnly:
		for (uint64_t i = 0; i < u64L_quantity; i++)
		{
			fread(&s16L_left, sizeof(int16_t), 1, objP_file);
			fseek(objP_file, sizeof(int16_t), SEEK_CUR);

			fcPL_samples[i] = 0 + s16L_left * I;
		}
		break;

	case eStreamChannelModeRightMono:
		for (uint64_t i = 0; i < u64L_quantity; i++)
		{
			fseek(objP_file, sizeof(int16_t), SEEK_CUR);
			fread(&s16L_right, sizeof(int16_t), 1, objP_file);

			fcPL_samples[i] = s16L_right + s16L_right * I;
		}
		break;

	case eStreamChannelModeRightOnly:
		for (uint64_t i = 0; i < u64L_quantity; i++)
		{
			fseek(objP_file, sizeof(int16_t), SEEK_CUR);
			fread(&s16L_right, sizeof(int16_t), 1, objP_file);

			fcPL_samples[i] = s16L_right + 0 * I;
		}
		break;

	case eStreamChannelModeMono:
		for (uint64_t i = 0; i < u64L_quantity; i++)
		{
			fread(&s16L_left, sizeof(int16_t), 1, objP_file);
			fread(&s16L_right, sizeof(int16_t), 1, objP_file);

			fcPL_samples[i] = ((s16L_right + s16L_left) / 2) +
				((s16L_right + s16L_left) / 2) * I;
		}
		break;

	case eStreamChannelModeStereo:
		for (uint64_t i = 0; i < u64L_quantity; i++)
		{
			fread(&s16L_left, sizeof(int16_t), 1, objP_file);
			fread(&s16L_right, sizeof(int16_t), 1, objP_file);

			fcPL_samples[i] = s16L_right + s16L_left * I;
		}
		break;

	default:
		break;
	}
}

void wawReader_closeStream(void)
{
	fclose(objP_file);
}
