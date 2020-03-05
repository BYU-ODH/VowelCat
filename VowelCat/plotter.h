// Copyright 2014 Formant Industries. See the Copying file at the top-level
// directory of this project.

#ifndef PLOTTER_H
#define PLOTTER_H

#include <pthread.h>

#include <QObject>

extern "C" {
#include "audio.h"
#include "formant.h"
}

#include "formants.h"
#include "params.h"
#include "timespec.h"

// Worker that processes formants in a separate thread.
class Plotter : public QObject {
    Q_OBJECT

public:
    Plotter(audio_t *a, sound_t *s, Formants *f);

    // Start the plotter in a new thread.
    void listen();
    void record();
    void play();

    void listen_run();
    void record_run();
    void play_run();

    // Stop the plotter and wait for it to finish processing.
    void stop();
    void newFormant(formant_sample_t f1, formant_sample_t f2);

signals:
    void pauseSig();
    void newSamples(size_t offset);

private:
    // Thread ID.
    pthread_t tid;
    bool run;

    audio_t *audio;
    sound_t *sound;
    Formants *formants;
};

#endif
