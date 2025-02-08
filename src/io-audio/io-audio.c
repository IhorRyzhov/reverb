#include "io-audio.h"
#include "../libwave/include/wave.h"
#include "../libsoundio/soundio/soundio.h"

#define SAMPLE_RATE 44100

static WaveFile* objP_file = NULL;
static WaveFile* objP_fileCapture = NULL;

//wave_write(objP_fileCapture, pL_input, u32L_frameCount);
//wave_read(objP_file, pL_framesOut, u64L_frameCount);   

static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max) 
{
    double float_sample_rate = outstream->sample_rate;
    struct SoundIoChannelArea *areas;
    int err;
    int s32L_framesToWrite = frame_count_max;

    err = soundio_outstream_begin_write(outstream, &areas, &s32L_framesToWrite);

    wave_read(objP_file, areas->ptr, frame_count_max);
    areas->step = 2;

    err = soundio_outstream_end_write(outstream);
}

static struct SoundIo *soundio;
static struct SoundIoDevice *device;
static struct SoundIoOutStream *outstream;

void ioAudio_init(void)
{
	const WaveErr* objPL_err = wave_err();
	objP_file = wave_open("tel.wav", WAVE_OPEN_READ);	
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

    soundio = soundio_create();
  
    int err = soundio_connect_backend(soundio, SoundIoBackendAlsa);

    soundio_flush_events(soundio);

    int selected_device_index = soundio_default_output_device_index(soundio);

    device = soundio_get_output_device(soundio, selected_device_index);

    outstream = soundio_outstream_create(device);


    outstream->write_callback = write_callback;
    outstream->underflow_callback = NULL;
    outstream->software_latency = 0.1;
    outstream->sample_rate = SAMPLE_RATE;
    
    if (soundio_device_supports_format(device, SoundIoFormatS16NE)) 
    {
        outstream->format = SoundIoFormatS16NE;
    }

    err = soundio_outstream_open(outstream);

    err = soundio_outstream_start(outstream);


}

void ioAudio_deinit(void)
{
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);

    wave_close(objP_fileCapture);

    wave_close(objP_file);
}