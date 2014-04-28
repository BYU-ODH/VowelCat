// Copyright 2014 Formant Industries. See the Copying file at the top-level
// directory of this project.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <inttypes.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include <QBrush>
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>

extern "C" {
    #include "audio.h"
    #include "formant.h"
}

#include "formants.h"
#include "plotter.h"
#include "qcustomplot.h"
#include "spectrogram.h"
#include "timespec.h"
#include "ui_mainwindow.h"

class Plotter;

class Tracer: public QCPItemTracer {
public:
    enum { COUNT = 8 };
    // The main tracer -- the biggest and most opaque.
    enum { LAST = COUNT - 1 };

private:
    enum { DIAM_MAX = 40 };
    enum { DIAM_MIN = 14 };
    enum { DIAM_RANGE = DIAM_MAX - DIAM_MIN };

    // Red.
    enum { COLOR_BASE = 0xff0000 };

    enum { ALPHA_MAX = 255 };
    enum { ALPHA_MIN = 0 };
    enum { ALPHA_RANGE = ALPHA_MAX - ALPHA_MIN };

    const QBrush brush;

public:
    Tracer(QCustomPlot *plot, QCPGraph *graph, size_t i);

    void hide();
    void show();

private:
    // Calculate a tracer diameter f(x) for the tracer x s.t.
    //
    //   f(0) = DIAM_MIN
    //   f(Tracer::LAST) = DIAM_MAX
    //
    // and all values in between follow a simple linear curve.
    static double size(double x);

    // Calculate an alpha value f(x) for the tracer x s.t.
    //
    //   f(0) = ALPHA_MIN
    //   f(Tracer::LAST) = ALPHA_MAX
    //
    // and all values inbetween follow an exponential curve.
    static uint32_t alpha(double x);

    // Splice together an RGBA integer for QColor::fromRgba of the form AARRGGBB.
    static uint32_t rgba(double i);
};

class MainWindow: public QMainWindow {
    Q_OBJECT

private:
    enum { TIMER_INTERVAL = 10 };
    enum { TIMER_SLOWDOWN = 50 };

    enum { DUR_BASE = SAMPLES_PER_CHUNK * 1000000000 / SAMPLE_RATE };
    enum { DUR_FUDGE = DUR_BASE };
    enum { DUR = DUR_BASE + DUR_FUDGE };

    typedef struct {
        formant_sample_t x, y;
    } pair_t;

    pair_t from, to;
    pair_t cur;

    timespec_t start;

    double slope;
    double x_range;

    Ui::MainWindow *ui;
    QCustomPlot *plot;
    QCPGraph *graph;

    Tracer *tracers[Tracer::COUNT];
    size_t tracer;

    pthread_mutex_t plot_lock;
    QTimer timer;

    QVector<QCPItemText*> vowelSymbols;
    QVector<QPushButton*> vowelButtons;
    QVector<QCPItemLine*> vowelLines;
    QSignalMapper signalMapper;
    int accentToggle;
    bool vowelToggle;

    enum {
        DEFAULT     = 0,
        INVERT_AXIS = 1 << 0,
        CHINESE_SYMBOLS = 1 << 1,
        BRITISH_RECEIVED = 1 << 2,
        ENGLISH_IPA = 1 << 3
    };

    uint32_t flags;

    audio_t *audio;
    Formants *formants;
    Plotter *plotter;

    int aud_fd;

public:
    explicit MainWindow(audio_t *a, Formants *f, Plotter *p, Spectrogram *s);
    ~MainWindow();

signals:
    void clearAudio();
    void audioSeek(size_t offset);

public slots:
    void plotFormant(formant_sample_t f1, formant_sample_t f2);
    void spectroClicked(size_t offset);
    void seekAudio(size_t offset);

private slots:
    void plotNext();

    void vowelButtonPushed(int vowelButtonPushed);
    void resetPlot();
    void defaultSymbolsButtonPushed();
    void addSymbolButtonPushed();
    void accentButtonPushed();

    void invertAxis();

    void loadSymbols();
    void saveSymbols();

    void newAudio();
    void startRecord();
    void stopAudio();

    void startPlay();
    void pauseAudio();
    void beginAudio();
    void endAudio();

    void openFile();
    void saveAsFile();

    void mouseMove(QMouseEvent*);
    void mouseRelease();

private:
    void initButtons();
    void loadSymbols(FILE *stream);
    void saveSymbols(FILE *stream) const;

    void setupPlot();
    void showTracers();
    void hideTracers();
    void setupVowelButtons();
    void setupEnglishButtons();
    void setupEnglishSymbols();
    void setupChineseButtons();
    void setupChineseSymbols();
    void setupEnglishReceivedSymbols();
    void addSymbol(QString symbol);
    void clearSymbols();

    void updateTracers(formant_sample_t x, formant_sample_t y);
    void clearTracer();
    void pauseTracers(size_t offset);

    void updateFPS() const;
};

#endif // MAINWINDOW_H
