#include "io-audio.h"

#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_WAV
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_ENGINE
#define MA_NO_GENERATION

#define MINIAUDIO_IMPLEMENTATION
#include "../miniaudio/miniaudio.h"
#include "../libwave/include/wave.h"

#define SAMPLE_RATE 48000

typedef struct MyDataSource
{
    ma_data_source_base obj_base;    
} myDataSource_t;

static ma_device obj_device;
static myDataSource_t obj_dataSource;
static WaveFile* objP_file = NULL;
static WaveFile* objP_fileCapture = NULL;

static void dataCallback(ma_device* objPL_device, void* pL_output, const void* pL_input, ma_uint32 u32L_frameCount)
{
    ma_data_source_read_pcm_frames(&obj_dataSource, pL_output, u32L_frameCount, NULL);

    wave_write(objP_fileCapture, pL_input, u32L_frameCount);
}

static ma_result myDataSourceRead(ma_data_source* objPL_dataSource, void* pL_framesOut, ma_uint64 u64L_frameCount, ma_uint64* u64PL_framesRead)
{
    wave_read(objP_file, pL_framesOut, u64L_frameCount);    
}

static ma_result myDataSourceSeek(ma_data_source* pDataSource, ma_uint64 frameIndex)
{
    return MA_NOT_IMPLEMENTED;
}

static ma_result myDataSourceGetDataFormat(
    ma_data_source* objPL_dataSource, 
    ma_format* objPL_format, 
    ma_uint32* u32PL_channels, 
    ma_uint32* u32PL_sampleRate, 
    ma_channel* objPL_channelMap, 
    size_t u32L_channelMapCap)
{
    *objPL_format = ma_format_s16;
    *u32PL_channels = 2;
    *u32PL_sampleRate = SAMPLE_RATE;

    return MA_SUCCESS;
}

static ma_result myDataSourceGetCursor(ma_data_source* objPL_dataSource, ma_uint64* u64PL_cursor)
{
    *u64PL_cursor = 0;
    return MA_NOT_IMPLEMENTED;
}

static ma_result myDataSourceGetLength(ma_data_source* objPL_dataSource, ma_uint64* u64PL_length)
{
    *u64PL_length = 0;
    return MA_NOT_IMPLEMENTED;
}

static ma_data_source_vtable obj_myDataSourceVtable =
{
    myDataSourceRead,
    myDataSourceSeek,
    myDataSourceGetDataFormat,
    myDataSourceGetCursor,
    myDataSourceGetLength
};

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
  

    ma_result eL_err = MA_SUCCESS;

    ma_data_source_config objL_baseConfig;
    objL_baseConfig = ma_data_source_config_init();
    objL_baseConfig.vtable = &obj_myDataSourceVtable;
    ma_data_source_init(&objL_baseConfig, &obj_dataSource.obj_base);
    
    ma_device_config objL_deviceConfig;
    objL_deviceConfig = ma_device_config_init(ma_device_type_duplex);
    objL_deviceConfig.playback.format = ma_format_s16;
    objL_deviceConfig.playback.channels = 2;
    objL_deviceConfig.capture.format = ma_format_s16;
    objL_deviceConfig.capture.channels = 2;
    objL_deviceConfig.sampleRate = SAMPLE_RATE;
    objL_deviceConfig.dataCallback = dataCallback;
    objL_deviceConfig.pUserData = NULL;
    objL_deviceConfig.periodSizeInFrames = 128; // frame buffer size

    if (ma_device_init(NULL, &objL_deviceConfig, &obj_device) != MA_SUCCESS) 
    {
        return;
    }

    if (ma_device_start(&obj_device) != MA_SUCCESS) 
    {
        ma_device_uninit(&obj_device);
        return;
    }
}

void ioAudio_deinit(void)
{
    ma_device_uninit(&obj_device);

    ma_data_source_uninit(&obj_dataSource.obj_base);

    wave_close(objP_fileCapture);

    wave_close(objP_file);
}