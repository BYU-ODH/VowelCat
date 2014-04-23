// Copyright 2014 Formant Industries. See the Copying file at the top-level
// directory of this project.

#ifndef PLOTTER_H
#define PLOTTER_H

#include <pthread.h>

extern "C" {
    #include "audio.h"
    #include "formant.h"
}
#include "mainwindow.h"
#include "params.h"
#include "timespec.h"

class MainWindow;

// Number of samples to take into account when checking for noise.
static const size_t NOISE_SAMPLES = 1 << 6;
// If recorded samples have an average value less than this, then consider them
// noise.
static const size_t NOISE_THRESHOLD = 80;

static_assert(NOISE_SAMPLES > 0 && NOISE_SAMPLES < SAMPLES_PER_CHUNK,
              "NOISE_SAMPLES is within bounds");

// Worker that processes formants in a separate thread.
class Plotter : public QObject {

    Q_OBJECT

public:
    MainWindow *window;

    Plotter(audio_t *a);
    ~Plotter();

    bool noise();
    void formant(uintmax_t &f1, uintmax_t &f2);

    // Start the plotter in a new thread.
    void listen();
    void record();
    void play();
    void begin();
    void end();

    void pause(size_t offset, uintmax_t &f1, uintmax_t &f2);

    void listen_run();
    void record_run();
    void play_run();

    // Stop the plotter and wait for it to finish processing.
    void stop();

signals:
    void pauseSig();
  
private:
    // Thread ID.
    pthread_t tid;

    bool run;

    // Relevant structures.
    audio_t *audio;
    formant_opts_t opts;
    sound_t sound;
};

#endif
