#include <stdio.h>
#include <stdint.h>
#include "wav-reader.h"

struct WavHeader
{
    // WAV-формат начинается с RIFF-заголовка:

    // Содержит символы "RIFF" в ASCII кодировке
    // (0x52494646 в big-endian представлении)
    char cP_chunkId[4];

    // 36 + subchunk2Size, или более точно:
    // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
    // Это оставшийся размер цепочки, начиная с этой позиции.
    // Иначе говоря, это размер файла - 8, то есть,
    // исключены поля chunkId и chunkSize.
    unsigned long u32_chunkSize;

    // Содержит символы "WAVE"
    // (0x57415645 в big-endian представлении)
    char cP_format[4];

    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:

    // Содержит символы "fmt "
    // (0x666d7420 в big-endian представлении)
    char cP_subchunk1Id[4];

    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    unsigned long u32_subchunk1Size;

    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    unsigned short u16_audioFormat;

    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    unsigned short u16_numChannels;

    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    unsigned long u32_sampleRate;

    // sampleRate * numChannels * bitsPerSample/8
    unsigned long u32_byteRate;

    // numChannels * bitsPerSample/8
    // Количество байт для одного сэмпла, включая все каналы.
    unsigned short u16_blockAlign;

    // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
    unsigned short u16_bitsPerSample;

    // Подцепочка "data" содержит аудио-данные и их размер.

    // Содержит символы "data"
    // (0x64617461 в big-endian представлении)
    char cP_subchunk2Id[4];

    // numSamples * numChannels * bitsPerSample/8
    // Количество байт в области данных.
    unsigned long u32_subchunk2Size;

    // Далее следуют непосредственно Wav данные.
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
