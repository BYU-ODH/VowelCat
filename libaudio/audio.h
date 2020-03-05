// Copyright 2014 Formant Industries. See the Copying file at the top-level
// directory of this project.

//*************
#ifndef AUDIO_H
#define AUDIO_H

//***************************
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <inttypes.h>
#include <sys/stat.h>
#include "portaudio.h"
#include "pa_ringbuffer.h"
#ifdef __MINGW32__
   #include "mman.h"
#else
   #include <sys/mman.h>
#endif
//***************************

typedef short audio_sample_t;

typedef struct audio_t
{
   size_t sample_rate;
   size_t n_channels;
   size_t samples_per_chunk;

   PaStream *pstream;
   PaStream *rstream;

   bool wakeup;
   pthread_cond_t wakeup_sig;
   pthread_mutex_t wakeup_mutex;

   enum {
      DEFAULT = 0,
      SOURCE_DISK = 1 << 0,
   } flags;

   audio_sample_t *prbuf;
   // Number of samples in prbuf.
   size_t prbuf_size;
   // Current sample offset in prbuf.
   size_t prbuf_offset;

   PaUtilRingBuffer rb;
   void *rb_data;
} audio_t;

bool audio_init(audio_t *a, size_t sample_rate, size_t n_channels, size_t samples_per_chunk);
// Reset the audio buffer to a safe state. Call this before calling audio_play,
// audio_record, or audio_stop.
void audio_reset(audio_t *a);
// Reset the audio buffer and clear any buffers it holds. Call this when you
// want to reuse the audio buffer, but start fresh.
void audio_clear(audio_t *a);
// Free all memory held by the audio buffer. Call this when you're done with the
// audio buffer.
void audio_destroy(audio_t *a);

void audio_open(audio_t *a, FILE *fp);
void audio_save(audio_t *a, FILE *fp);

bool audio_play(audio_t *a);
bool audio_record(audio_t *a);
void audio_stop(audio_t *a);

bool audio_play_read(audio_t *a, audio_sample_t *samples);
bool audio_record_read(audio_t *a, audio_sample_t *samples);
bool audio_listen_read(audio_t *a, audio_sample_t *samples);
void audio_seek(audio_t *a, size_t index);

#endif
