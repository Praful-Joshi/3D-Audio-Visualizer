// audioAnalyzer.cpp - Handles WAV loading and FFT to get 64-band frequency data
// Depends on: miniaudio.h and kissfft

#include "audioAnalyzer.h"
#include "miniaudio.h"  // include miniaudio directly
#include "kissfft/kiss_fft.h"  // add kissfft headers to project
#include <cstring>
#include <cmath>
#include <iostream>

#define FFT_SIZE 1024
#define NUM_BANDS 64

AudioAnalyzer::AudioAnalyzer(const std::string& filepath)
    : filePath(filepath), sampleIndex(0) {}

    void AudioAnalyzer::init() {
        ma_engine_init(nullptr, &engine);
        ma_sound_init_from_file(&engine, filePath.c_str(), 0, nullptr, nullptr, &sound);
        ma_sound_start(&sound);

        decoderConfig = ma_decoder_config_init(ma_format_f32, 1, 44100);
    
        if (ma_decoder_init_file(filePath.c_str(), &decoderConfig, &decoder) != MA_SUCCESS) {
            std::cerr << "Failed to initialize decoder.\n";
            exit(1);
        }
    
        ma_uint64 pcmFrameCount;
        if (ma_decoder_get_length_in_pcm_frames(&decoder, &pcmFrameCount) != MA_SUCCESS) {
            std::cerr << "Failed to get PCM frame count.\n";
            exit(1);
        }
        frameCount = static_cast<size_t>(pcmFrameCount);
        samples = new float[frameCount];
        ma_decoder_read_pcm_frames(&decoder, samples, frameCount, nullptr);
    
        fftCfg = kiss_fft_alloc(FFT_SIZE, 0, nullptr, nullptr);
        fftIn = new kiss_fft_cpx[FFT_SIZE];
        fftOut = new kiss_fft_cpx[FFT_SIZE];
        bands.resize(NUM_BANDS, 0.0f);
    
        sampleIndex = 0;
    }
    

void AudioAnalyzer::update() {
    if (sampleIndex + FFT_SIZE >= frameCount) sampleIndex = 0;

    for (int i = 0; i < FFT_SIZE; ++i) {
        fftIn[i].r = samples[sampleIndex + i];
        fftIn[i].i = 0.0f;
    }

    kiss_fft(fftCfg, fftIn, fftOut);

    std::fill(bands.begin(), bands.end(), 0.0f);
    for (int i = 0; i < FFT_SIZE / 2; ++i) {
        float mag = sqrt(fftOut[i].r * fftOut[i].r + fftOut[i].i * fftOut[i].i);
        int band = (i * NUM_BANDS) / (FFT_SIZE / 2);
        bands[band] += mag;
    }

    // normalize
    for (float& b : bands) b = log1p(b) / 10.0f;
    sampleIndex += FFT_SIZE / 2; // hop forward half-window
}

const std::vector<float>& AudioAnalyzer::getFrequencyBands() const {
    return bands;
}

AudioAnalyzer::~AudioAnalyzer() {
    delete[] samples;
    delete[] fftIn;
    delete[] fftOut;
    kiss_fft_free(fftCfg);
    ma_decoder_uninit(&decoder);
}

