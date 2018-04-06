// Copyright 2014 Formant Industries. See the Copying file at the top-level
// directory of this project.

#include <math.h>

#include <QPainter>
#include <QPaintEvent>
#include <QPen>

extern "C"{
#include "formant.h"
}

#include "params.h"
#include "spectrogram.h"
#include "util.h"

Spectrogram::Spectrogram(audio_t *audio):
    QWidget(NULL),
    audio(audio),
    formants(audio, &sound),
    prev(0),
    pos(0)
{
    sound_init(&sound);
}

Spectrogram::~Spectrogram() {
    sound_destroy(&sound);
}

int Spectrogram::offset_to_pixel(size_t offset) {
    return offset / SAMPLES_PER_CHUNK * PIXELS_PER_CHUNK;
}

size_t Spectrogram::pixel_to_offset(int pixel) {
    return pixel / PIXELS_PER_CHUNK * SAMPLES_PER_CHUNK;
}

void Spectrogram::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Left, right, top, and bottom of area that needs painted.
    int l, r, t, b;
    event->rect().getCoords(&l, &t, &r, &b);

    // Convert to width and height.
    int w = r - l + 1;
    int h = b - t + 1;

    // The number of chunks to draw. Take the ceiling in case only a partial
    // amount of the last chunk needs drawn.
    int chunks = DIV_CEIL(w, PIXELS_PER_CHUNK);

    for (int c = 0; c < chunks; c += 1) {
        // The left and right offsets of the chunk.
        int cl = l + c * PIXELS_PER_CHUNK;
        int cr = MIN(cl + PIXELS_PER_CHUNK - 1, r);

        draw_chunk(&painter, cl, cr, h);
    }

    // Draw the position indicator if it's within this chunk.
    if (pos >= l && pos < r) {
        painter.setPen(QPen(QColor(INDICATOR)));
        painter.drawLine(pos, 0, pos, h);
    }
}

void Spectrogram::update(size_t offset) {
    prev = pos;
    pos = offset_to_pixel(offset);

    QWidget::update(prev, 0, PIXELS_PER_CHUNK, height());
    QWidget::update(pos, 0, PIXELS_PER_CHUNK, height());
}

void Spectrogram::draw_chunk(QPainter *paint, int l, int r, int h) {
    // Convert a pixel offset to a bin index.
    auto pixel_to_freq = [=] (int p) -> formant_sample_t {
        return FREQ_START + p * FREQ_RANGE / h;
    };

    size_t sample = pixel_to_offset(l);

    if (audio->samples_per_chunk + sample > audio->prbuf_size || !formants.calc(sample)) {
        paint->setPen(Qt::NoPen);
        paint->fillRect(l, 0, r - l + 1, h, QColor(BACKGROUND));

        return;
    }

    for (int p = 1; p <= h; p += 1) {
        paint->setPen(QPen(QColor(freq_argb(pixel_to_freq(p),
            formants.f1, formants.f2))));
        paint->drawLine(l, h - p, r, h - p);
    }
}

uint8_t Spectrogram::freq_byte(formant_sample_t dist) {
    if (dist >= RADIUS)
        return GREY_MAX;

    return GREY_MIN + GREY_RANGE * pow(dist, 1) / RADIUS;
}

QRgb Spectrogram::freq_argb(formant_sample_t f, formant_sample_t f1,
                            formant_sample_t f2)
{
    uint8_t byte = freq_byte(MIN(ABS(f - f1), ABS(f - f2)));

    return ALPHA << 24 | byte << 16 | byte << 8 | byte;
}

void Spectrogram::mouseReleaseEvent(QMouseEvent *event) {
    emit clicked(pixel_to_offset(event->x()));
}

void Spectrogram::reset() {
    prev = pos = 0;
    QWidget::update();
}
