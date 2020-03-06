# Copyright 2014 Formant Industries. See the Copying file at the top-level
# directory of this project.

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
}

TARGET = VowelCat
TEMPLATE = app

SOURCES += \
    formants.cpp \
    main.cpp \
    mainwindow.cpp \
    phonetic-chart.cpp \
    plotter.cpp \
    qcustomplot.cpp \
    spectrogram.cpp \
    timespec.cpp \

HEADERS += \
    formants.h \
    mainwindow.h \
    phonetic-chart.h \
    plotter.h \
    qcustomplot.h \
    spectrogram.h \
    timespec.h \

FORMS += \
    mainwindow.ui \

RESOURCES += \
    icons.qrc \
    images.qrc \

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += $$(CFLAGS)

LIBS += -lm
LIBS += $(LDFLAGS)

# https://stackoverflow.com/questions/34040791/no-matching-function-for-call-to-connect-qt-5-5
CONFIG += c++11
