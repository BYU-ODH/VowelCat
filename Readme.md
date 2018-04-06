# VowelCat

This project was completed as part of a senior design project at Ohio University in 2014.

## Functional

**Brian Evans**

— *Ohio University School of Dance, Film, and Theater - Assistant Professor of Performance*

## Developers

**Natalie Yarnell**

— *Ohio University Russ College of Engineering and Technology - Computer Science*

**Patrick Gray**

— *Ohio University Russ College of Engineering and Technology - Computer Science*

**Mick Koch**

— *Ohio University Russ College of Engineering and Technology - Computer Science*

## Original information from the client

> As an Assistant Professor of Voice and Speech in the School of Theater at Ohio
> University I am interested in developing software to further our educational
> mission and share with the larger academic community.  I have used phonetics
> and the IPA’s vowel chart to demonstrate vowel shifts for the instruction of
> speech for many years, and I have wanted to see more tools that demonstrate
> these principles for quite some time.  I have been familiar with programs such
> as Dr. Speech and Sona-Match by Kay Pentax, but these programs are geared
> towards speech therapy, and my needs are more specifically didactic and
> artistic.  My goal is to develop and expand this approach to the visualization
> of vowels that may prove to be invaluable to the training of professional
> artists.

> I teach speech and dialects to actors, and I believe this tool could offer much
> to our curricula.  Specifically, the use of vowels for particular dialects or
> accents could be clarified with visual demonstration of sample recordings.  The
> student actors could then have the opportunity in real time to experiment with
> vowel substitutions in the context of speech in order to produce similar vowel
> patterns to the desired accent.  Vocal tract posture is a key component in my
> teaching of accents, and visualizing placement patterns of vowels could be
> extremely helpful with student outcomes.  Fundamental vowel identification in
> our work with speech helps students recognize patterns in their own accent, and
> again, the visual representation of this could further my ability to help the
> students’ progress.

— *Brian Evans*

## Requirements

> - Ability to record and display F1-F2 plots in real-time.
> - Ability to place phonetic symbols anywhere on the chart for reference points and target vowels.
> - Ability to place a vowel quadrilateral over the chart to give a relative orientation of target vowels.
> - An inverted x and y axis to give speakers a "right-side-up" display of vowel production.
> - A linear wave recording in the same pane as the vowel display in order to
>   review specific vowels for comparison and analysis.
> - Mac and PC versions.

— *Brian Evans*

## Analysis

At the start of the semester, we were introduced to Brian Evans' idea for a real-time
formant software. We were provided with a brief introduction to the theory regarding
formants, along with a C library that contained the functions to calculate formants. With
this information, we assembled as a team and began research into the development of such a
project. It was our responsibility to make use of the given C library and redesign the
code to work with any other code we stumbled upon or developed ourselves. This document
provides insight into the four month long development process of the VowelCat software.

## Introduction

A person's voice is comprised of many frequencies that all work to formulate the vowels
and consonants heard in speech. Lower frequencies are interpreted as vowel sounds, while
higher frequencies create the consonants. The pattern of such frequencies varies from
person to person, which ultimately leads to the formulation of distinguishable accents.
Accents are typically measured by the lower frequencies of a voice. Thus, accent training
revolves around manipulating vowel sounds.

Vowel sounds are formulated from the positioning of one's tongue, lips, and jaw. An open
jaw and compressed tongue help to shape the 'æ' that may be heard in the word cat. One
could transition to the 'ɔɪ' sound, as heard in the word boy, simply by puckering his or
her lips. Such processes seem intuitive, yet it is often the case that people have trouble
emulating an accent when learning a new language. Such people seek the help of teachers
and textbooks to learn the mouth movements and understand the sounds. With a new age of
technological advancements, software has taken on a major role in linguistics. VowelCat is
one such software that aims to guide peoples' accents through real time voice graphing.

VowelCat records and digitizes a user's voice. The data is immediately transferred to a
formant processing function that will extract the frequencies associated with vowel
sounds. These frequencies are plotted on a formant phonetic chart, which professional
linguists have designed in an attempt to visualize accents. It is assumed that the
real-time actions of this software will allow people to gradually correct their accents in
one fluid movement, rather than having to wait for evaluation, from possibly a teacher,
after physically practicing. This software also provides file saving an opening, giving
users an opportunity to review past training sessions.

## Design and Implementation

* `libaudio` written from scratch and leveraging the portaudio library

  The Vowel Formant Analyzer utilizes the open source audio I/O library PortAudio. This
  C written API executes low level functions to capture recordings from a microphone. At
  a 44100 Hz sampling rate, a high quality digital sound wave may be extracted from a
  user’s voice. This extraction is stored in a fixed sized ring buffer. The ring
  buffer’s architecture allows for a continuous, asynchronous recording. Much like an
  assembly line, one processing thread stores the recorded samples in the buffer, while
  another unloads the data for formant conversion, all at a low cost to memory.

* `libformant` adapted from Snack C source code

  Formant conversion involves a series of mathematical computations that have been
  abstracted away by the Snack Sound Toolkit. This open source C library provides
  numerous audio processing and sound visualization functions, useful for sound analysis
  software such as the Vowel Formant Analyzer. While the underlying code is written in
  C, all functions have been wrapped within Tcl and Python scripting to provide a simple
  command driven interaction. Unfortunately, such functionality was unnecessary overhead
  for the Vowel Formant Analyzer, thus much time was spent extracting and reconfiguring
  the code into a few, simple C files of formant functions.

* GUI written in C++ using the Qt framework

  The user interface of the Vowel Formant Analyzer must perform several tasks. Most
  prominently, it must continuously take formant values calculated from the audio
  captured by the microphone and plot them on a graph. To give this data some context,
  symbols representing the formant value of the outer edge of the International Phonetic
  Alphabet (IPA) are included by default. The IPA is a system of phonetic notation that
  represents standardized sounds of oral language. Since the primary purpose of this
  application is to assist actors and actresses in perfecting accents, including these
  vowel symbols gives them a scientific way of verifying that they are making the
  correct sounds. In addition to plotting the most recent points based on audio data, we
  also continue to plot two or three previous points on the graph. By altering the
  opacity and size of these 'tracers', we try to give the user some primitive
  biofeedback to assist them in seeing how small variations in their voice can affect
  the resulting formant values.

  The user is also able to add additional symbols and move those symbols around. While
  this removes the correlation between the symbols, their plot positions and the IPA,
  this allows users to create their own configurations to practice specific accent
  profiles, for example. There is also support for recording and manipulating audio
  files. This allows the user to record audio that demonstrates a specific accent
  profile and then share that with other users.

  The user interface is written using a framework called Qt, which is itself written in
  C++. Qt is an application framework and widget toolkit that allows for easy creation
  of GUI's. Additionally, Vowel Formant Analyzer also uses a 3rd party plotting widget
  called QCustomPlot to manage plotting the data in real time.

## Installation / Build guide

VowelCat utilizes an elaborate Makefile build system to compile and link all source code.
The underlying libraries are organized into their respective files, each provided with a
minimal Makefile. The parent directory contains a first level Makefile that recursively
compiles the source code, setting up any necessary directives along the way, ultimately
leaving the developer with an executable located in a newly created directory build/VowelCat.
Various commands, such as a debug enable option, may be issued to the build system. Clean
up is as simple as typing 'make clean'.

### Linux

Depending on the distro, you may need to install the following packages:

- gcc-4.7
- g++-4.7
- qt4
- libasound2-dev

```
$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test
$ sudo apt-get update
$ sudo apt-get install gcc-4.7 g++-4.7 qt4 libasound2-dev
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.6
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 40 --slave /usr/bin/g++ g++ /usr/bin/g++-4.7
$ sudo update-alternatives --config gcc
$ sudo update-alternatives --config g++
$ make
$ ./build/vowelcat
```

### Release Command

The following build command statically links all external libraries to produce one
independent executable to run on any Mac or Windows computer.

```
$ make release
```

## Known limitations

* There is no pitch detection, so female voices don't tend to work very well.
* Users cannot freely change audio settings (e.g. sample rate, channels, ).
* Users cannot select their input nor output device within program.
* There is only one accent profile implemented (though adding more is a more or less straightforward process).
* The spectrogram has a fixed width.

## Future work

- [*Evaluation of a New Beam-Search Formant
Tracking Algorithm in Noisy Environments*](http://link.springer.com/chapter/10.1007/978-3-642-35292-8_5)
- [*VOCAL TRACT RESONANCES TRACKING BASED ON VOICED AND UNVOICED
SPEECH CLASSIFICATION USING DYNAMIC PROGRAMMING AND FIXED INTERVAL
KALMAN SMOOTHER*](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=4518585)
- [*Developments of the Research of the Formant Tracking Algorithm*](http://ccsenet.org/journal/index.php/cis/article/view/5122)
- [*A NEW STRATEGY OF FORMANT TRACKING BASED
ON DYNAMIC PROGRAMMING*](http://terpconnect.umd.edu/~juneja/00832.pdf)
- [*Control Methods Used in a Study of the Vowels*](http://homepages.wmich.edu/~hillenbr/204/PetersonBarney1952.pdf)
- [*Acoustic characteristics of American English Vowels*](http://homepages.wmich.edu/~hillenbr/Papers/HillenbrandGettyClarkWheeler.pdf)

## Design decisions/tradeoffs

* We have to disable particular buttons while others are enabled. For example the
  record button is disabled while a file is opened because we do not allow editing of
  audio files. This tradeoff is necessary because we do no yet have the GUI features
  to make the whole process of editing intuitive.
