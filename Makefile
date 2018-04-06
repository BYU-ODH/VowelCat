# Copyright 2014 Formant Industries. See the Copying file at the top-level
# directory of this project.

# Public make rules:
#
#  - make
#  	build libraries and link gui into build directory
#  - make -B
#  	force rebuild all libraries and gui
#  - make release
#  	package binary into standalone app
#  - make compile DIR=scratch/dir
#  	compile (but don't link) code in scratch/dir using the Makefile in that
#  	directory
#  - make link DIR=scratch/dir
#  	link the code in VowelCat using the Makefile in that directory
#  - make clean
#  	remove build directory

# Public make flags:
#
#  - DEBUG=1
#       disable optimizations and enable debugging symbols
#  - NATIVE=1
#       compile for the native instruction set of the host computer
#  - PROFILE=1
#       enable gprof profiling symbols
#  - OPTIMIZE=1
#       enable link-time and general optimizations

# The build process is split into multiple stages:
#
#  1. Populate build directory.
#  2. Build static libraries and generate their pkgconfigs.
#  3. Link binaries with static libraries.
#  4. Perform any steps for a final release.

# Save overriden variables.
ECFLAGS := $(CFLAGS)
CFLAGS =
ELDFLAGS := $(LDFLAGS)
LDFLAGS =
EQMAKEFLAGS := $(QMAKEFLAGS)
QMAKEFLAGS =

# Disable user overrides. This is safe because the relevant ones are saved
# above. It's necessary so variables aren't overridden when passed to submakes.
MAKEOVERRIDES =

# Variables to export to submakes.
export CFLAGS LDFLAGS QMAKEFLAGS

# Relative and absolute path where things will be built.
BUILD = build
BUILD_ABS = $(PWD)/$(BUILD)

# What platform we're running on.
ifeq ($(OS), )
OS = $(shell uname)
endif

# Set up variables for submodules to build -- order is important!
SRC_AUDIO = libaudio
BUILD_AUDIO = $(BUILD)/$(SRC_AUDIO)
STATICLIB_AUDIO = $(BUILD)/libaudio.a
STATICLIB_AUDIO_BUILD = $(BUILD_AUDIO)/libaudio.a
DIRS += $(SRC_AUDIO)
STATICLIBS += $(STATICLIB_AUDIO)
INCLUDE += -I$(BUILD_ABS)/$(SRC_AUDIO)
PKG_CONFIGS += $(BUILD_AUDIO)/libaudio.pc

SRC_PORTAUDIO = libportaudio
BUILD_PORTAUDIO = $(BUILD)/$(SRC_PORTAUDIO)
STATICLIB_PORTAUDIO = $(BUILD)/libportaudio.a
STATICLIB_PORTAUDIO_BUILD = $(BUILD_PORTAUDIO)/lib/.libs/libportaudio.a
DIRS += $(SRC_PORTAUDIO)
STATICLIBS += $(STATICLIB_PORTAUDIO)
INCLUDE += -I$(BUILD_ABS)/$(SRC_PORTAUDIO)/include
PKG_CONFIGS += $(BUILD_PORTAUDIO)/portaudio-2.0.pc

SRC_FORMANT = libformant
BUILD_FORMANT = $(BUILD)/$(SRC_FORMANT)
STATICLIB_FORMANT = $(BUILD)/libformant.a
STATICLIB_FORMANT_BUILD = $(BUILD_FORMANT)/libformant.a
DIRS += $(SRC_FORMANT)
STATICLIBS += $(STATICLIB_FORMANT)
INCLUDE += -I$(BUILD_ABS)/$(SRC_FORMANT)
PKG_CONFIGS += $(BUILD_FORMANT)/libformant.pc

SRC_VOWELCAT = VowelCat
BUILD_VOWELCAT = $(BUILD)/$(SRC_VOWELCAT)
VOWELCAT_BIN_BUILD = $(BUILD_VOWELCAT)/VowelCat
VOWELCAT_BIN = $(BUILD)/vowelcat
VOWELCAT_EXE_BUILD = $(BUILD_VOWELCAT)/VowelCat.exe
VOWELCAT_EXE = $(BUILD)/VowelCat.exe
VOWELCAT_APP_BUILD = $(BUILD_VOWELCAT)/VowelCat.app
VOWELCAT_APP = $(BUILD)/VowelCat.app
VOWELCAT_DMG = $(BUILD)/VowelCat.dmg
DIRS += $(SRC_VOWELCAT)

ifeq ($(OS), Darwin)
APP_VOWELCAT_BUILD = $(VOWELCAT_APP_BUILD)
APP_VOWELCAT = $(VOWELCAT_APP)
else ifeq ($(OS), Windows_NT)
APP_VOWELCAT_BUILD = $(VOWELCAT_EXE_BUILD)
APP_VOWELCAT = $(VOWELCAT_EXE)
else
APP_VOWELCAT_BUILD = $(VOWELCAT_BIN_BUILD)
APP_VOWELCAT = $(VOWELCAT_BIN)
endif

APPS += $(APP_VOWELCAT)

ifneq ($(STAGE), )
    CFLAGS += $(INCLUDE)
    CFLAGS += $(ECFLAGS)

    ifeq ($(OS), Darwin)
        CFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
        QMAKEFLAGS += QMAKE_CXX=$(shell command -v clang++)
    else ifeq ($(OS), Windows_NT)
        LDFLAGS += -static
        QMAKEFLAGS += CONFIG-=debug_and_release
    endif

    ifeq ($(RELEASE), 1)
        QMAKEFLAGS += CONFIG+=release
    endif

    ifeq ($(OPTIMIZE), 1)
        CFLAGS += -O2 -flto -DNDEBUG
        LDFLAGS += -O2 -flto
    endif

    ifeq ($(NATIVE), 1)
        CFLAGS += -march=native
        LDFLAGS += -march=native
    endif

    ifeq ($(DEBUG), 1)
        CFLAGS += -O0 -g
        QMAKEFLAGS += CONFIG+=debug
    endif

    ifeq ($(PROFILE), 1)
        CFLAGS += -pg
        LDFLAGS += -pg
    endif
endif

ifeq ($(STAGE), 3)
    # Tell the compiler to search the build dir for libs.
    LDFLAGS += -L$(BUILD_ABS)
    # Include the LDFLAGS required by libs.
    LDFLAGS += $(shell pkg-config --libs $(PKG_CONFIGS))
    LDFLAGS += $(ELDFLAGS)
endif

# Take some default action based on the stage.
ifeq ($(STAGE), )
all: stage-3
else ifeq ($(STAGE), 1)
all: $(DIRS)
else ifeq ($(STAGE), 2)
all: $(STATICLIBS)
else ifeq ($(STAGE), 3)
all: $(APP_VOWELCAT)
else ifeq ($(STAGE), 4)
ifeq ($(OS), Darwin)
all: $(VOWELCAT_DMG)
else
all:
endif
endif

# Create the build directory.
$(BUILD):
	-mkdir -p $@

# Copy source directories into the build directory, updating only outdated
# files.
$(DIRS): | $(BUILD)
	cp -ruf $@ $(BUILD)

# Build the portaudio static library.
$(STATICLIB_PORTAUDIO):
	cd $(BUILD_PORTAUDIO) && ./configure --enable-static --without-jack
	$(MAKE) -C $(BUILD_PORTAUDIO)
	cp $(STATICLIB_PORTAUDIO_BUILD) $@

# Build the formant static library.
$(STATICLIB_FORMANT):
	$(MAKE) -C $(BUILD_FORMANT)
	cp $(STATICLIB_FORMANT_BUILD) $@

# Build the audio static library.
$(STATICLIB_AUDIO):
	$(MAKE) -C $(BUILD_AUDIO)
	cp $(STATICLIB_AUDIO_BUILD) $@

# Build the main app.
$(APP_VOWELCAT):
	$(MAKE) -C $(BUILD_VOWELCAT)
	cp -ruf $(APP_VOWELCAT_BUILD) $@

$(VOWELCAT_DMG):
	macdeployqt $(VOWELCAT_APP)
	hdiutil create -ov -srcfolder $(VOWELCAT_APP) $@

# Force these to be remade every time.
.PHONY: $(DIRS) $(STATICLIB_FORMANT) $(STATICLIB_AUDIO) $(APP_VOWELCAT)

stage-1:
	$(MAKE) STAGE=1

stage-2: stage-1
	$(MAKE) STAGE=2

stage-3: stage-2
	$(MAKE) STAGE=3

stage-4: stage-3
	$(MAKE) STAGE=4

ifeq ($(DIR), )
compile:
	@echo error: specify a directory to compile with DIR=
else ifeq ($(STAGE), 2)
compile:
	$(MAKE) -C $(DIR)
else
compile: stage-1
	$(MAKE) STAGE=2 compile
endif

ifeq ($(DIR), )
link:
	@echo error: specify a directory to link with DIR=
else ifeq ($(STAGE), 3)
link:
	$(MAKE) -C $(DIR)
else
link: stage-2
	$(MAKE) STAGE=3 link
endif

ifeq ($(RELEASE), )
release:
	$(MAKE) RELEASE=1 release
else
release: stage-4
endif

clean:
	-rm -r $(BUILD)

.PHONY: all stage-1 stage-2 stage-3 stage-4 compile link release clean
