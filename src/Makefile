GCC       = g++
CFLAGS    = -Weffc++ -Wall -Wextra -pedantic

DEBUG_BB ?= YES
SOUND    ?= YES
COVERAGE ?= YES

ifeq ($(DEBUG_BB), YES)
CFLAGS   += -D_DEBUG -g
else
CFLAGS   += -O3 -DNDEBUG
endif

ifeq ($(COVERAGE), YES)
CFLAGS   += -fprofile-arcs -ftest-coverage
LDFLAGS  += -fprofile-arcs
endif

CFLAGS   += -I../include -I../kyra2_1_3

LIBS      = ../kyra2_1_3/Kyra/engine/.libs/libengine.a ../kyra2_1_3/Kyra/.libs/libkyra.a -lrt
SDL_LIBS  = $(shell sdl-config --static-libs) -lSDL_image #-lSDL_mixer
SDL_FLAGS = $(shell sdl-config --cflags)
IMAGES    = ../images

OBJECTS = bbc.o consolefont.o Brainblast.o Brick.o Puzzle.o Field.o BrainSprite.o BrainSound.o \
          BrainPlayer.o BrainAI.o BrainPlayerManager.o main.o HighScore.o BrainText.o \
          BrainState.o TextListener.o #BrainSoundSDL.o BrainSoundBASS.o

ifeq ($(SOUND), YES)
# FIXME: Handle 32/64 bit
LIBS    += -L../fmod/fmodex/api/lib -lfmodex64
OBJECTS += BrainSoundFMODEX.o
else
CFLAGS  += -DNOSOUND
endif

VERSION = $(shell perl -wne 'if(/brainblast \((.*)\)/){print "$$1";exit;}' < ../deb/debian/changelog)
ARCH    = $(shell dpkg-architecture -qDEB_HOST_ARCH)

KRENCODER = ../kyra2_1_3/Kyra/encoder/krencoder

all: brainblast

lcov:
	lcov --directory . --capture --output-file app.info
	genhtml -o coverage app.info

$(KRENCODER):
	cd ../kyra2_1_3/Kyra && ./autogen && make

$(IMAGES)/bb.h: ../images/bricks.xml $(KRENCODER) 
	cd $(IMAGES) && \
	$(KRENCODER) -pBB_ bb.h bricks.xml

$(IMAGES)/bb_bg.h: ../images/bg.xml $(KRENCODER)
	cd $(IMAGES) && \
	$(KRENCODER) -pBB_BG_ bb_bg.h bg.xml



# How to avoid hardcoding the static libraries ? Using -l just makes it find the .so it seems
  #/usr/local/lib/libengine.a /usr/local/lib/libkyra.a
  #-static-libgcc -Wl,-static -lengine -lkyra
  # -L../bassmod -lbassmod
brainblast: $(IMAGES)/bb.h $(IMAGES)/bb_bg.h $(OBJECTS) 
	$(GCC) -o $@ $(OBJECTS) $(LDFLAGS)  $(SDL_LIBS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(GCC) $(CFLAGS) $(SDL_FLAGS) -c $<

rpm: package
	sudo alien --to-rpm ../deb/brainblast_${VERSION}_i386.deb

package: all
	make -C ../deb

install: package
	sudo dpkg -i ../deb/brainblast_${VERSION}_$(ARCH).deb

remake: 
	make clean; make

clean:
	rm -rfv *.o brainblast *~ \
	$(IMAGES)/bb.h $(IMAGES)/bb.dat $(IMAGES)/bb_bg.h $(IMAGES)/bb_bg.dat 
	rm -rvf src/app.info src/coverage src/*.gcda src/*.gcno

depend:
	makedepend *.cpp *.h

# DO NOT DELETE

BrainAI.o: BrainAI.h BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h
BrainAI.o: Brick.h /usr/include/stdlib.h /usr/include/features.h
BrainAI.o: /usr/include/stdc-predef.h /usr/include/alloca.h Field.h
BrainAI.o: Brainblast.h BrainSound.h BrainState.h /usr/include/assert.h
BrainAI.o: TextListener.h
BrainPlayer.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainPlayer.o: /usr/include/stdlib.h /usr/include/features.h
BrainPlayer.o: /usr/include/stdc-predef.h /usr/include/alloca.h Field.h
BrainPlayerManager.o: BrainPlayerManager.h BrainPlayer.h BrainSprite.h bbc.h
BrainPlayerManager.o: BrainText.h Puzzle.h Brick.h /usr/include/stdlib.h
BrainPlayerManager.o: /usr/include/features.h /usr/include/stdc-predef.h
BrainPlayerManager.o: /usr/include/alloca.h Field.h Brainblast.h BrainSound.h
BrainPlayerManager.o: BrainState.h /usr/include/assert.h TextListener.h
BrainPlayerManager.o: HighScore.h BrainAI.h ../images/bb.h
BrainSoundBASS.o: BrainSoundBASS.h BrainSound.h /usr/include/assert.h
BrainSoundBASS.o: /usr/include/features.h /usr/include/stdc-predef.h
BrainSoundBASS.o: ../bassmod/bassmod.h /usr/include/stdio.h
BrainSoundBASS.o: /usr/include/libio.h /usr/include/_G_config.h
BrainSoundBASS.o: /usr/include/wchar.h /usr/include/xlocale.h
BrainSoundFMOD.o: BrainSoundFMOD.h BrainSound.h ../fmod/api/inc/fmod.h
BrainSoundFMOD.o: /usr/include/assert.h /usr/include/features.h
BrainSoundFMOD.o: /usr/include/stdc-predef.h ../fmod/api/inc/fmod_errors.h
BrainSoundFMOD.o: /usr/include/stdio.h /usr/include/libio.h
BrainSoundFMOD.o: /usr/include/_G_config.h /usr/include/wchar.h
BrainSoundFMOD.o: /usr/include/xlocale.h /usr/include/malloc.h
BrainSoundFMOD.o: /usr/include/string.h
BrainSoundFMODEX.o: BrainSoundFMODEX.h BrainSound.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod.hpp
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_codec.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_dsp.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_memoryinfo.h
BrainSoundFMODEX.o: /usr/include/assert.h /usr/include/features.h
BrainSoundFMODEX.o: /usr/include/stdc-predef.h /usr/include/stdio.h
BrainSoundFMODEX.o: /usr/include/libio.h /usr/include/_G_config.h
BrainSoundFMODEX.o: /usr/include/wchar.h /usr/include/xlocale.h
BrainSoundFMODEX.o: /usr/include/malloc.h /usr/include/string.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_errors.h
BrainSoundSDL.o: BrainSoundSDL.h BrainSound.h bbc.h
BrainSprite.o: BrainSprite.h bbc.h Brainblast.h /usr/include/stdlib.h
BrainSprite.o: /usr/include/features.h /usr/include/stdc-predef.h
BrainSprite.o: /usr/include/alloca.h Puzzle.h Brick.h Field.h BrainSound.h
BrainSprite.o: BrainState.h /usr/include/assert.h BrainText.h TextListener.h
BrainState.o: BrainState.h /usr/include/assert.h /usr/include/features.h
BrainState.o: /usr/include/stdc-predef.h BrainText.h TextListener.h
BrainState.o: Brainblast.h /usr/include/stdlib.h /usr/include/alloca.h bbc.h
BrainState.o: Puzzle.h Brick.h Field.h BrainSprite.h BrainSound.h
BrainState.o: BrainPlayerManager.h BrainPlayer.h HighScore.h BrainAI.h
BrainText.o: BrainText.h consolefont.h bbc.h Brainblast.h
BrainText.o: /usr/include/stdlib.h /usr/include/features.h
BrainText.o: /usr/include/stdc-predef.h /usr/include/alloca.h Puzzle.h
BrainText.o: Brick.h Field.h BrainSprite.h BrainSound.h BrainState.h
BrainText.o: /usr/include/assert.h TextListener.h
Brainblast.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/stdc-predef.h /usr/include/alloca.h bbc.h Puzzle.h
Brainblast.o: Brick.h Field.h BrainSprite.h BrainSound.h BrainState.h
Brainblast.o: /usr/include/assert.h BrainText.h TextListener.h ../images/bb.h
Brainblast.o: ../images/bb_bg.h BrainPlayerManager.h BrainPlayer.h
Brainblast.o: HighScore.h BrainAI.h BrainSoundFMODEX.h
Brainblast.o: ../fmod/fmodex/api/inc/fmod.hpp ../fmod/fmodex/api/inc/fmod.h
Brainblast.o: ../fmod/fmodex/api/inc/fmod_codec.h
Brainblast.o: ../fmod/fmodex/api/inc/fmod_dsp.h
Brainblast.o: ../fmod/fmodex/api/inc/fmod_memoryinfo.h
Brick.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/stdc-predef.h /usr/include/alloca.h
HighScore.o: HighScore.h bbc.h /usr/include/errno.h /usr/include/features.h
HighScore.o: /usr/include/stdc-predef.h
Puzzle.o: Puzzle.h Brick.h bbc.h /usr/include/stdlib.h
Puzzle.o: /usr/include/features.h /usr/include/stdc-predef.h
Puzzle.o: /usr/include/alloca.h Field.h Brainblast.h BrainSprite.h
Puzzle.o: BrainSound.h BrainState.h /usr/include/assert.h BrainText.h
Puzzle.o: TextListener.h ../images/bb.h
TextListener.o: TextListener.h
bbc.o: bbc.h
consolefont.o: consolefont.h
main.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
main.o: /usr/include/stdc-predef.h /usr/include/alloca.h bbc.h Puzzle.h
main.o: Brick.h Field.h BrainSprite.h BrainSound.h BrainState.h
main.o: /usr/include/assert.h BrainText.h TextListener.h
BrainAI.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainAI.o: /usr/include/stdlib.h /usr/include/features.h
BrainAI.o: /usr/include/stdc-predef.h /usr/include/alloca.h Field.h
BrainPlayer.o: BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainPlayer.o: /usr/include/stdlib.h /usr/include/features.h
BrainPlayer.o: /usr/include/stdc-predef.h /usr/include/alloca.h Field.h
BrainPlayerManager.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h
BrainPlayerManager.o: Brick.h /usr/include/stdlib.h /usr/include/features.h
BrainPlayerManager.o: /usr/include/stdc-predef.h /usr/include/alloca.h
BrainPlayerManager.o: Field.h Brainblast.h BrainSound.h BrainState.h
BrainPlayerManager.o: /usr/include/assert.h TextListener.h HighScore.h
BrainPlayerManager.o: BrainAI.h
BrainSoundBASS.o: BrainSound.h /usr/include/assert.h /usr/include/features.h
BrainSoundBASS.o: /usr/include/stdc-predef.h
BrainSoundFMOD.o: BrainSound.h ../fmod/api/inc/fmod.h /usr/include/assert.h
BrainSoundFMOD.o: /usr/include/features.h /usr/include/stdc-predef.h
BrainSoundFMODEX.o: BrainSound.h ../fmod/fmodex/api/inc/fmod.hpp
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_codec.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_dsp.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_memoryinfo.h
BrainSoundFMODEX.o: /usr/include/assert.h /usr/include/features.h
BrainSoundFMODEX.o: /usr/include/stdc-predef.h
BrainSoundSDL.o: BrainSound.h
BrainSprite.o: bbc.h
BrainState.o: /usr/include/assert.h /usr/include/features.h
BrainState.o: /usr/include/stdc-predef.h BrainText.h TextListener.h
Brainblast.o: /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/stdc-predef.h /usr/include/alloca.h bbc.h Puzzle.h
Brainblast.o: Brick.h Field.h BrainSprite.h BrainSound.h BrainState.h
Brainblast.o: /usr/include/assert.h BrainText.h TextListener.h
Brick.o: bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/stdc-predef.h /usr/include/alloca.h
HighScore.o: bbc.h
Puzzle.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Puzzle.o: /usr/include/stdc-predef.h /usr/include/alloca.h Field.h
