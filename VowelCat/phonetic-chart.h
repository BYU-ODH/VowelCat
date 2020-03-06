// Copyright 2014 Formant Industries. See the Copying file at the top-level
// directory of this project.

#ifndef PHONETIC_CHART_H
#define PHONETIC_CHART_H

#include <inttypes.h>
#include <stdio.h>
#include <wchar.h>

#include <QString>
#include <QVector>
#include <QLabel>

#include "qcustomplot.h"

class VowelSymbol: public QCPItemText {
private:
    enum { FONT_SIZE = 30 };

public:
    VowelSymbol(QCustomPlot *plot, const char *symbol, uint32_t f1, uint32_t f2);
    VowelSymbol(QCustomPlot *plot, const QString &symbol, uint32_t f1, uint32_t f2);
    VowelSymbol(QCustomPlot *plot, uint32_t f1, uint32_t f2);
};

class DipthongArrow: public QCPItemLine {
public:
    DipthongArrow(QCustomPlot *plot, VowelSymbol *start_, const QPointF &end_);
};

class PhoneticChart {
private:
    QCustomPlot *plot;
    QLabel *label;

    QString title;
    QVector<VowelSymbol *> symbols;
    QVector<DipthongArrow *> arrows;

    QMap<VowelSymbol *, DipthongArrow *> symbolArrowMap;

public:
    PhoneticChart(QCustomPlot *plot_, QLabel *label_);
    PhoneticChart();

    bool load(FILE *stream);

    void install();
    void uninstall();

    void save(FILE *stream);

    void mouseMove(QMouseEvent *event);
    void addSymbol(QString &symbol, uint32_t f1, uint32_t f2);
    void hide();
    void show();

private:
    bool loadTitle(FILE *stream);
    bool loadSymbols(FILE *stream);
};

#endif
