// AudioProcessor.h
#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <algorithm>
#include <vector>
#include <cmath>
#include <memory>
#include "3rdParty/kissfft/kiss_fft.h"
#include "3rdParty/kissfft/kiss_fftr.h"
#include "kiss_fft.h"
/*
FFT (Fast Fourier Transform) is an algorithm used to efficiently compute the Discrete Fourier Transform (DFT) of a sequence of values. The DFT is a mathematical operation that converts a time-domain signal into its frequency-domain representation. The FFT algorithm reduces the number of computations required to calculate the DFT, making it more efficient for use in computing devices.
*/
class KissFftWrapper
{
public:
    KissFftWrapper(int fftSize)
        : fftSize(fftSize), fftCfg(kiss_fft_alloc(fftSize, 0, nullptr, nullptr)) {
    }

    ~KissFftWrapper() {
        kiss_fft_free(fftCfg);
    }

    std::vector<double> performFFT(const std::vector<int16_t>& audioData)
    {
        // Prepare input and output buffers
        std::vector<kiss_fft_cpx> input(fftSize), output(fftSize);

        // Copy audio data to input buffer
        for (int i = 0; i < fftSize; i++) {
            input[i].r = audioData[i];
            input[i].i = 0;
        }

        // Perform FFT
        kiss_fft(fftCfg, input.data(), output.data());

        // Prepare FFT data
        std::vector<double> fftData(fftSize / 2);
        for (int i = 0; i < fftSize / 2; i++) {
            fftData[i] = sqrt(output[i].r * output[i].r + output[i].i * output[i].i) / fftSize;
        }

        return fftData;
    }

private:
    int fftSize;
    kiss_fft_cfg fftCfg;
};

class AudioProcessor
{

public:
    explicit AudioProcessor(const int& sampleRate = 44100)
        : _sampleRate(sampleRate),
        _fftWrapper(std::make_unique<KissFftWrapper>(4096))  // Initialize KissFftWrapper with a size of 4096
    {}
    virtual ~AudioProcessor() = default;

    void processAudioData(const char* audioData) {
        // Process the audio data and extract audio parameters
    }

    double processBuffer(const char *buf, size_t size)
    {
        int sampleCount = size / sizeof(int16_t);
        // Prepare audio data for FFT
        std::vector<int16_t> audioData(sampleCount);
        for (int i = 0; i < sampleCount; i++) {
            audioData[i] = buf[i];
        }
        // Perform FFT on the audio data
        std::vector<double> fftData = _fftWrapper->performFFT(audioData);
        // Find the dominant frequency
        int dominantFrequencyIndex = findDominantFrequencyIndex(fftData, _fftWrapper.get());
        // Convert the dominant frequency index to actual frequency
        double frequency = (dominantFrequencyIndex + 0.5) * _sampleRate / fftData.size();
        // Convert frequency to semitone
        double semitone;
        if (frequency <= 0) {
            semitone = -INFINITY;
        } else {
            semitone = 12 * log2(frequency / 440.0) + 69;
        }
        return semitone;
    }
    
    int findDominantFrequencyIndex(const std::vector<double> &fftData, const KissFftWrapper* _fftWrapper)
    {
        // using some arbitrary minimum and maximum frequency values here - adjust as needed
        return findDominantFrequencyIndex(fftData, 0, _sampleRate / 2, _fftWrapper);
    }

    int findDominantFrequencyIndex(const std::vector<double> &fftData, double minFrequency, double maxFrequency, const KissFftWrapper* _fftWrapper)
    {
        int index = 0;
        double maxValue = 0;
        int startIndex = std::round(minFrequency * fftData.size() / _sampleRate);
        int endIndex = std::round(maxFrequency * fftData.size() / _sampleRate);
        for (int i = startIndex; i < endIndex; i++) {
            if (fftData[i] > maxValue) {
                maxValue = fftData[i];
                index = i;
            }
        }
        return index;
    }

private:
    std::unique_ptr<KissFftWrapper> _fftWrapper;
    int _sampleRate;
};

#endif // AUDIOPROCESSOR_H
