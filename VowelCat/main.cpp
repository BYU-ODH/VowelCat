// Copyright 2014 Formant Industries. See the Copying file at the top-level
// directory of this project.

#include <signal.h>
#include <stdlib.h>

#include <QApplication>
#include <QSplashScreen>

extern "C" {
#include "audio.h"
#include "formant.h"
}

#include "formants.h"
#include "mainwindow.h"
#include "params.h"
#include "plotter.h"
#include "spectrogram.h"

// Handle OS signals.
static void sig(int s) {
    switch (s) {
    case SIGINT:
    case SIGTERM:
        QCoreApplication::quit();
    }
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QApplication::setGraphicsSystem("raster");
#endif

    signal(SIGINT, sig);
    signal(SIGTERM, sig);

    QApplication app(argc, argv);

    QPixmap pixmap(":/images/splash.png");
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

    QFont splashFont;
    splashFont.setFamily("Sans");
    splashFont.setWeight(QFont::DemiBold);

    QColor ou_green(0, 105, 78);

    splash.setFont(splashFont);
    splash.showMessage("v 1.0.0", Qt::AlignBottom, ou_green);
    splash.show();

    audio_t audio;

    if (!audio_init(&audio, SAMPLE_RATE, CHANNELS, SAMPLES_PER_CHUNK))
        abort();

    sound_t sound;
    sound_init(&sound);

    Formants formants(&audio, &sound);
    Plotter plotter(&audio, &sound, &formants);
    auto spectro = new Spectrogram(&audio);
    MainWindow window(&audio, &formants, &plotter, spectro);

    QObject::connect(&plotter, SIGNAL(pauseSig()),
                     &window, SLOT(pauseAudio()));
    // This multithreaded DirectConnection is safe since proper locking is done
    // inside the MainWindow functions.
    QObject::connect(&plotter, &Plotter::newFormant,
                     &window, &MainWindow::plotFormant,
                     Qt::DirectConnection);
    QObject::connect(&window, &MainWindow::audioSeek,
                     &window, &MainWindow::seekAudio);

    QObject::connect(&plotter, &Plotter::newSamples,
                     spectro, &Spectrogram::update,
                     Qt::QueuedConnection);
    QObject::connect(&window, &MainWindow::audioSeek,
                     spectro, &Spectrogram::update,
                     Qt::QueuedConnection);
    QObject::connect(spectro, &Spectrogram::clicked,
                     &window, &MainWindow::spectroClicked);
    QObject::connect(&window, &MainWindow::clearAudio,
                     spectro, &Spectrogram::reset);

    plotter.listen();
    window.show();

    // This function blocks until the main window is closed or
    // QCoreApplication::quit is called.
    QCoreApplication::exec();

    delete spectro;
    plotter.stop();
    sound_destroy(&sound);
    audio_destroy(&audio);
}
