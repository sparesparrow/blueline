#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include <QObject>
#include <QAudioBuffer>
#include <QAudioRecorder>
#include <QAudioProbe>
#include <QAudioFormat>
#include <fftw3.h>
//Remember to include all the necessary KissFFT files in your project and to link them correctly. As a starting point, you might want to include the following files based on a simple KissFFT example【25†source】:

#include "_kiss_fft_guts.h"
#include "kiss_fft.c"
#include "kiss_fft.h"
#include "kiss_fftr.c"
#include "kiss_fftr.h"

class SpectrumAnalyser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAudioRecorder* audioRecorder READ audioRecorder WRITE setAudioRecorder NOTIFY audioRecorderChanged)

public:
    explicit SpectrumAnalyser(QObject *parent = nullptr);
    ~SpectrumAnalyser();

    QAudioRecorder* audioRecorder() const;
    void setAudioRecorder(QAudioRecorder *recorder);

signals:
    void audioRecorderChanged();
    void semitoneChanged(const QString &semitone);

public slots:
    void start();
    void stop();
    void processBuffer(const QAudioBuffer &buffer);

private:
    void analyseAudioData();

    QAudioRecorder *m_audioRecorder;
    QAudioProbe *m_probe;
    QAudioFormat m_format;
};

class KissFftWrapper {
public:
    KissFftWrapper(int size)
        : m_size(size), m_cfg(kiss_fft_alloc(size, 0, nullptr, nullptr)) {
    }

    ~KissFftWrapper() {
        free(m_cfg);
    }

    void fft(const QVector<kiss_fft_cpx> &in, QVector<kiss_fft_cpx> &out) {
        kiss_fft(m_cfg, in.data(), out.data());
    }

private:
    int m_size;
    kiss_fft_cfg m_cfg;
};

#endif // SPECTRUMANALYSER_H
