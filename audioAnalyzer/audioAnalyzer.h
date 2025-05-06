// audioAnalyzer.h
#pragma once

#include <string>
#include <vector>
#include "miniaudio.h"    // you'll download this as a header-only lib
#include "kissfft/kiss_fft.h"  // header-only FFT library

class AudioAnalyzer {
public:
    AudioAnalyzer(const std::string& filepath);
    void init();
    void update();
    const std::vector<float>& getFrequencyBands() const;
    ~AudioAnalyzer();

private:
    std::string filePath;
    float* samples = nullptr;
    ma_uint64 frameCount = 0;
    ma_format format;
    ma_uint32 sampleRate;
    ma_decoder decoder;
    ma_decoder_config decoderConfig;
    ma_engine engine;
    ma_sound sound;

    kiss_fft_cfg fftCfg = nullptr;
    kiss_fft_cpx* fftIn = nullptr;
    kiss_fft_cpx* fftOut = nullptr;

    size_t sampleIndex;
    std::vector<float> bands;
};
