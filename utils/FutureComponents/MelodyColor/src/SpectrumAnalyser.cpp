#include "SpectrumAnalyser.h"

SpectrumAnalyser::SpectrumAnalyser(QObject *parent)
    : QObject(parent)
    , m_audioRecorder(new QAudioRecorder(this))
    , m_probe(new QAudioProbe(this))
{
    // Set up the format of the audio data
    m_format.setSampleRate(44100);
    m_format.setChannelCount(1);
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    connect(m_probe, &QAudioProbe::audioBufferProbed, this, &SpectrumAnalyser::processBuffer);
    m_probe->setSource(m_audioRecorder);
}

SpectrumAnalyser::~SpectrumAnalyser()
{
}

QAudioRecorder* SpectrumAnalyser::audioRecorder() const
{
    return m_audioRecorder;
}

void SpectrumAnalyser::setAudioRecorder(QAudioRecorder *audioRecorder) {
    if (m_audioRecorder != audioRecorder) {
        if (m_audioRecorder && m_probe) {
            m_probe->setSource(nullptr);
        }

        m_audioRecorder = audioRecorder;

        if (m_audioRecorder && m_probe) {
            m_probe->setSource(m_audioRecorder);
        }

        emit audioRecorderChanged(m_audioRecorder);
    }
}

void SpectrumAnalyser::start() {
    if (m_audioRecorder->state() != QAudioRecorder::RecordingState) {
        qWarning() << "Audio recorder is not in the recording state.";
        return;
    }

    if (!m_probe->setSource(m_audioRecorder)) {
        qWarning() << "Failed to set probe source.";
        return;
    }

    m_audioRecorder->record();
}


void SpectrumAnalyser::stop()
{
    m_audioRecorder->stop();
}

void SpectrumAnalyser::processBuffer(const QAudioBuffer &buffer)
{
    /*! extract the audio data from QAudioBuffer and store it in an array. You can do this using the QAudioBuffer::Data method. */
    //! const quint16 *data = buffer.constData<quint16>();



/* Implementation of the FFT algorithm in C++ using the FFTW library to determine the dominant frequency from the audio data obtained from a QAudioBuffer object in Qt6:

1. First, you need to extract the audio data from QAudioBuffer and store it in an array. You can do this using the QAudioBuffer::Data method.
2. Next, you need to prepare the audio data for FFT analysis by applying a window function to reduce spectral leakage. Popular window functions include the Hamming, Hanning, and Blackman windows.
3. Then, you need to apply the FFT algorithm to the windowed audio data. There are several libraries available that can help you with FFT, such as the FFTW library.
4. Once you have obtained the FFT output, you can calculate the frequency spectrum by finding the magnitude of the FFT coefficient at each frequency bin. This can be done using the Pythagorean theorem for complex numbers.
5. Finally, you can determine the dominant frequency by finding the bin with the maximum magnitude
*/


// Function to calculate the FFT using FFTW library
void calculateFFT(fftw_complex* fft_in, fftw_complex* fft_out, int N) {
    fftw_plan plan = fftw_plan_dft_1d(N, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

// Function to determine the dominant frequency
double getDominantFrequency(QAudioBuffer buffer) {
    const qint16* data = buffer.constData<qint16>();
    int N = buffer.frameCount();

    // Allocate memory for the FFT input and output
    fftw_complex* fft_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* fft_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    // Fill the FFT input with audio data
    for (int i = 0; i < N; i++) {
        fft_in[i][0] = data[i];
        fft_in[i][1] = 0;
    }

    // Calculate the FFT
    calculateFFT(fft_in, fft_out, N);

    // Find the peak frequency in the FFT output
    double dominant_frequency = 0;
    double max_amplitude = 0;
    for (int i = 0; i < N/2; i++) {
        double amplitude = sqrt(fft_out[i][0]*fft_out[i][0] + fft_out[i][1]*fft_out[i][1]);
        if (amplitude > max_amplitude) {
            max_amplitude = amplitude;
            dominant_frequency = i * buffer.sampleRate() / N;
        }
    }

    // Free memory and return the dominant frequency
    fftw_free(fft_in);
    fftw_free(fft_out);
    return dominant_frequency;
}

/*
To use this code, simply call the `getDominantFrequency()` function with a QAudioBuffer object as the argument, like so:


QAudioBuffer buffer = // ... get audio buffer from somewhere ...
double dominant_frequency = getDominantFrequency(buffer);
 

This will return the dominant frequency in the audio buffer as a double. Note that the above code assumes the input audio data is in 16-bit signed integer format. If your audio data is in a different format, you'll need to adjust the code accordingly
*/









    // analyseAudioData function needs to be implemented here
    // to analyse the audio data from buffer and identify the semitone
    // When a semitone is identified, you can emit semitoneChanged signal like this:
    // emit semitoneChanged("A");

    //

    // Convert the audio samples to the format expected by the FFT library
    // This will depend on the FFT library you're using and the format of your audio data

    // Perform FFT on the audio data to get the frequencies and their amplitudes
    // This will depend on the FFT library you're using

    // Find the dominant frequency (the one with the highest amplitude)

    // Map the dominant frequency to the nearest semitone
    /** semitone = round(12 * log2(frequency / A4_frequency)) + A4_semitone */
    int semitone = round(12 * log2(dominantFrequency / 440.0)) + 57;

    // Map the semitone to a note
    QString note;
    switch (semitone % 12) {
        case 0: note = "C"; break;
        case 1: note = "C#"; break;
        case 2: note = "D"; break;
        case 3: note = "D#"; break;
        case 4: note = "E"; break;
        case 5: note = "F"; break;
        case 6: note = "F#"; break;
        case 7: note = "G"; break;
        case 8: note = "G#"; break;
        case 9: note = "A"; break;
        case 10: note = "A#"; break;
        case 11: note = "B"; break;
    }

    // Add the octave
    note += QString::number(semitone / 12);

    // Notify the QML side
    emit semitoneChanged(note);
}
1. First, you need to extract the audio data from QAudioBuffer and store it in an array. You can do this using the QAudioBuffer::Data method.
2. Next, you need to prepare the audio data for FFT analysis by applying a window function to reduce spectral leakage. Popular window functions include the Hamming, Hanning, and Blackman windows.
3. Then, you need to apply the FFT algorithm to the windowed audio data. There are several libraries available that can help you with FFT, such as the FFTW library.
4. Once you have obtained the FFT output, you can calculate the frequency spectrum by finding the magnitude of the FFT coefficient at each frequency bin. This can be done using the Pythagorean theorem for complex numbers.
5. Finally, you can determine the dominant frequency by finding the bin with the maximum magnitude