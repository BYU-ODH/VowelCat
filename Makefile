# Export all variables to subshells so submakes pick up the CFLAGS and LDFLAGS
# assignments below.
export CFLAGS LDFLAGS

# Relative and absolute path where things will be built.
BUILD = build
BUILD_ABS = $(PWD)/$(BUILD)

# Final paths of static libraries.
STATICLIB_PORTAUDIO = $(BUILD)/libportaudio.a
STATICLIB_FORMANT = $(BUILD)/libformant.a
STATICLIB_AUDIO = $(BUILD)/libaudio.a

# Source paths of static libraries.
STATICLIB_PORTAUDIO_BUILD = $(BUILD)/libportaudio/lib/.libs/libportaudio.a
STATICLIB_FORMANT_BUILD = $(BUILD)/libformant/libformant.a
STATICLIB_AUDIO_BUILD = $(BUILD)/libaudio/libaudio.a

# Set up include dirs.
CFLAGS += -I$(BUILD_ABS)/libportaudio/include
CFLAGS += -I$(BUILD_ABS)/libaudio
CFLAGS += -I$(BUILD_ABS)/libformant

# Turn on optimizations and LTO?
ifdef OPTIMIZE
    CFLAGS += -O2 -flto
    LDFLAGS += -flto
endif

# Compile for the build computer?
ifdef NATIVE
    CFLAGS += -march=native
    LDFLAGS += -march=native
endif

# Enable debug symbols?
ifdef DEBUG
    CFLAGS += -O0 -g
endif

# Common commands to set up a build dir. This is used instead of a $(BUILD) rule
# because there's no dependency on the timestamp of $(BUILD).
define set_up_build =
    -mkdir -p $(BUILD)
    cp -ru $< $(BUILD)
endef

all: staticlibs

# Build the portaudio static library.
$(BUILD)/libportaudio: libportaudio
	$(set_up_build)
	cd $@ && autoreconf -fi && ./configure --enable-static
	$(MAKE) -C $@

$(STATICLIB_PORTAUDIO_BUILD): $(BUILD)/libportaudio

$(STATICLIB_PORTAUDIO): $(STATICLIB_PORTAUDIO_BUILD)
	cp $< $@

# Build the formant static library.
$(BUILD)/libformant: libformant
	$(set_up_build)
	$(MAKE) -C $@

$(STATICLIB_FORMANT_BUILD): $(BUILD)/libformant

$(STATICLIB_FORMANT): $(STATICLIB_FORMANT_BUILD)
	cp $< $@

# Build the audio static library.
$(BUILD)/libaudio: libaudio
	$(set_up_build)
	$(MAKE) -C $@

$(STATICLIB_AUDIO_BUILD): $(BUILD)/libaudio

$(STATICLIB_AUDIO): $(STATICLIB_AUDIO_BUILD)
	cp $< $@

staticlibs: $(STATICLIB_FORMANT) $(STATICLIB_PORTAUDIO) $(STATICLIB_AUDIO)

clean:
	-rm -r $(BUILD)

.PHONY: all staticlibs clean
