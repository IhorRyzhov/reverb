#ifndef WAV_READER_WAV_READER_H_
#define WAV_READER_WAV_READER_H_

#include <complex.h>

enum StreamChannelMode
{
	eStreamChannelModeDefault = 0, // same as in file
	eStreamChannelModeLeftMono,    // sent left channel to right and left channels
	eStreamChannelModeLeftOnly,    // read only left channel, right is ignored
	eStreamChannelModeRightMono,   // sent right channel to right and left channels
	eStreamChannelModeRightOnly,   // read only right channel, left is ignored
	eStreamChannelModeMono,        // mix left and right channels
	eStreamChannelModeStereo       // left to left, right to right
};
typedef enum StreamChannelMode streamChannelMode_t;

void wawReader_openStream(
	const char* cPL_nameOfFile,
	streamChannelMode_t eL_channelMode);

void wawReader_readStream(
	float complex* fcPL_samples,
	uint64_t u64L_quantity);

void wawReader_closeStream(void);

#endif /* WAV_READER_WAV_READER_H_ */
