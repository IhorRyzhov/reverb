#pragma once
#include <complex>
#include "../liquid-dsp/include/liquid.h"
#include <vector>

class UpolsConv 
{
public:
    UpolsConv(uint32_t ioChunkSize) : fftSize(ioChunkSize * 2)
    {
        xT = new std::complex<float>[fftSize];
        xF = new std::complex<float>[fftSize];

        fftPlan = fft_create_plan(
            fftSize,
            reinterpret_cast<liquid_float_complex *>(xT),
            reinterpret_cast<liquid_float_complex *>(xF),
            LIQUID_FFT_FORWARD,
            0);
    }
    ~UpolsConv() = default;

    void setIr(std::vector<std::complex<float>> &ir) {
        uint32_t blocksQnt = ir.size() / fftSize + ir.size() % fftSize;
        std::complex<float> *hInput;
        std::complex<float> *hOutput;

        fftPlan = fft_create_plan(
            fftSize,
            reinterpret_cast<liquid_float_complex *>(xT),
            reinterpret_cast<liquid_float_complex *>(xF),
            LIQUID_FFT_FORWARD,
            0);
    }

private:
    const uint32_t fftSize;
    std::complex<float> *xT;
    std::complex<float> *xF;

    fftplan fftPlan;
    fftplan ifftPlan;

    std::vector<std::complex<float>> ir;
};