GCC       = g++
CFLAGS    = -Weffc++ -Wall -Wextra -pedantic

DEBUG_BB  = YES
SOUND    ?= YES

ifeq ($(DEBUG_BB), YES)
CFLAGS   += -D_DEBUG -g
else
CFLAGS   += -O3 -DNDEBUG
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
LIBS    += -L../fmod/api -lfmod-3.75
OBJECTS += BrainSoundFMOD.o
else
CFLAGS  += -DNOSOUND
endif

VERSION = $(shell perl -wne 'if(/brainblast \((.*)\)/){print "$$1";exit;}' < ../deb/debian/changelog)

KRENCODER = ../kyra2_1_3/Kyra/encoder/krencoder

all: brainblast

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
brainblast: $(OBJECTS)
	$(GCC) -o $@ $(OBJECTS) $(SDL_LIBS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(GCC) $(CFLAGS) $(SDL_FLAGS) -c $<

package: all
	make -C ../deb

install: package
	sudo dpkg -i ../deb/brainblast_${VERSION}_i386.deb

remake: 
	make clean; make

clean:
	rm -rfv *.o brainblast *~ \
	$(IMAGES)/bb.h $(IMAGES)/bb.dat $(IMAGES)/bb_bg.h $(IMAGES)/bb_bg.dat 

depend:
	makedepend *.cpp *.h

# DO NOT DELETE

BrainAI.o: BrainAI.h BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h
BrainAI.o: Brick.h /usr/include/stdlib.h /usr/include/features.h
BrainAI.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainAI.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainAI.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
BrainAI.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainAI.o: /usr/include/time.h /usr/include/endian.h
BrainAI.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainAI.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainAI.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainAI.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainAI.o: /usr/include/alloca.h Field.h Brainblast.h BrainSound.h
BrainAI.o: BrainState.h /usr/include/assert.h TextListener.h
BrainPlayer.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainPlayer.o: /usr/include/stdlib.h /usr/include/features.h
BrainPlayer.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainPlayer.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainPlayer.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
BrainPlayer.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayer.o: /usr/include/time.h /usr/include/endian.h
BrainPlayer.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainPlayer.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainPlayer.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainPlayer.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainPlayer.o: /usr/include/alloca.h Field.h
BrainPlayerManager.o: BrainPlayerManager.h BrainPlayer.h BrainSprite.h bbc.h
BrainPlayerManager.o: BrainText.h Puzzle.h Brick.h /usr/include/stdlib.h
BrainPlayerManager.o: /usr/include/features.h /usr/include/bits/predefs.h
BrainPlayerManager.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainPlayerManager.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
BrainPlayerManager.o: /usr/include/sys/types.h /usr/include/bits/types.h
BrainPlayerManager.o: /usr/include/bits/typesizes.h /usr/include/time.h
BrainPlayerManager.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainPlayerManager.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
BrainPlayerManager.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainPlayerManager.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainPlayerManager.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
BrainPlayerManager.o: Field.h Brainblast.h BrainSound.h BrainState.h
BrainPlayerManager.o: /usr/include/assert.h TextListener.h HighScore.h
BrainPlayerManager.o: BrainAI.h ../images/bb.h
BrainSoundBASS.o: BrainSoundBASS.h BrainSound.h /usr/include/assert.h
BrainSoundBASS.o: /usr/include/features.h /usr/include/bits/predefs.h
BrainSoundBASS.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSoundBASS.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
BrainSoundBASS.o: ../bassmod/bassmod.h /usr/include/stdio.h
BrainSoundBASS.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainSoundBASS.o: /usr/include/libio.h /usr/include/_G_config.h
BrainSoundBASS.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
BrainSoundBASS.o: /usr/include/bits/sys_errlist.h
BrainSoundFMOD.o: BrainSoundFMOD.h BrainSound.h ../fmod/api/inc/fmod.h
BrainSoundFMOD.o: /usr/include/assert.h /usr/include/features.h
BrainSoundFMOD.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainSoundFMOD.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSoundFMOD.o: /usr/include/gnu/stubs-64.h ../fmod/api/inc/fmod_errors.h
BrainSoundFMOD.o: /usr/include/stdio.h /usr/include/bits/types.h
BrainSoundFMOD.o: /usr/include/bits/typesizes.h /usr/include/libio.h
BrainSoundFMOD.o: /usr/include/_G_config.h /usr/include/wchar.h
BrainSoundFMOD.o: /usr/include/bits/stdio_lim.h
BrainSoundFMOD.o: /usr/include/bits/sys_errlist.h /usr/include/malloc.h
BrainSoundFMOD.o: /usr/include/string.h /usr/include/xlocale.h
BrainSoundSDL.o: BrainSoundSDL.h BrainSound.h bbc.h
BrainSprite.o: BrainSprite.h bbc.h Brainblast.h /usr/include/stdlib.h
BrainSprite.o: /usr/include/features.h /usr/include/bits/predefs.h
BrainSprite.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSprite.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
BrainSprite.o: /usr/include/sys/types.h /usr/include/bits/types.h
BrainSprite.o: /usr/include/bits/typesizes.h /usr/include/time.h
BrainSprite.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainSprite.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
BrainSprite.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainSprite.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainSprite.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
BrainSprite.o: Puzzle.h Brick.h Field.h BrainSound.h BrainState.h
BrainSprite.o: /usr/include/assert.h BrainText.h TextListener.h
BrainState.o: BrainState.h /usr/include/assert.h /usr/include/features.h
BrainState.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainState.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainState.o: /usr/include/gnu/stubs-64.h BrainText.h TextListener.h
BrainState.o: Brainblast.h /usr/include/stdlib.h /usr/include/sys/types.h
BrainState.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainState.o: /usr/include/time.h /usr/include/endian.h
BrainState.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainState.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainState.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainState.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainState.o: /usr/include/alloca.h bbc.h Puzzle.h Brick.h Field.h
BrainState.o: BrainSprite.h BrainSound.h BrainPlayerManager.h BrainPlayer.h
BrainState.o: HighScore.h BrainAI.h
BrainText.o: BrainText.h consolefont.h bbc.h Brainblast.h
BrainText.o: /usr/include/stdlib.h /usr/include/features.h
BrainText.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainText.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainText.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
BrainText.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainText.o: /usr/include/time.h /usr/include/endian.h
BrainText.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainText.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainText.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainText.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainText.o: /usr/include/alloca.h Puzzle.h Brick.h Field.h BrainSprite.h
BrainText.o: BrainSound.h BrainState.h /usr/include/assert.h TextListener.h
Brainblast.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
Brainblast.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Brainblast.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
Brainblast.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Brainblast.o: /usr/include/time.h /usr/include/endian.h
Brainblast.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
Brainblast.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brainblast.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brainblast.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brainblast.o: /usr/include/alloca.h bbc.h Puzzle.h Brick.h Field.h
Brainblast.o: BrainSprite.h BrainSound.h BrainState.h /usr/include/assert.h
Brainblast.o: BrainText.h TextListener.h ../images/bb.h ../images/bb_bg.h
Brainblast.o: BrainPlayerManager.h BrainPlayer.h HighScore.h BrainAI.h
Brainblast.o: BrainSoundFMOD.h ../fmod/api/inc/fmod.h
Brick.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
Brick.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Brick.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
Brick.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Brick.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
Brick.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
Brick.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Brick.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Brick.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
HighScore.o: HighScore.h bbc.h /usr/include/errno.h /usr/include/features.h
HighScore.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
HighScore.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
HighScore.o: /usr/include/gnu/stubs-64.h /usr/include/bits/errno.h
HighScore.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
HighScore.o: /usr/include/asm-generic/errno.h
HighScore.o: /usr/include/asm-generic/errno-base.h
Puzzle.o: Puzzle.h Brick.h bbc.h /usr/include/stdlib.h
Puzzle.o: /usr/include/features.h /usr/include/bits/predefs.h
Puzzle.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Puzzle.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
Puzzle.o: /usr/include/sys/types.h /usr/include/bits/types.h
Puzzle.o: /usr/include/bits/typesizes.h /usr/include/time.h
Puzzle.o: /usr/include/endian.h /usr/include/bits/endian.h
Puzzle.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
Puzzle.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Puzzle.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Puzzle.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h Field.h
Puzzle.o: Brainblast.h BrainSprite.h BrainSound.h BrainState.h
Puzzle.o: /usr/include/assert.h BrainText.h TextListener.h ../images/bb.h
TextListener.o: TextListener.h
bbc.o: bbc.h
consolefont.o: consolefont.h
main.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
main.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
main.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
main.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
main.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
main.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
main.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
main.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
main.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h bbc.h Puzzle.h
main.o: Brick.h Field.h BrainSprite.h BrainSound.h BrainState.h
main.o: /usr/include/assert.h BrainText.h TextListener.h
BrainAI.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainAI.o: /usr/include/stdlib.h /usr/include/features.h
BrainAI.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainAI.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainAI.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
BrainAI.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainAI.o: /usr/include/time.h /usr/include/endian.h
BrainAI.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainAI.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainAI.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainAI.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainAI.o: /usr/include/alloca.h Field.h
BrainPlayer.o: BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainPlayer.o: /usr/include/stdlib.h /usr/include/features.h
BrainPlayer.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainPlayer.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainPlayer.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
BrainPlayer.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayer.o: /usr/include/time.h /usr/include/endian.h
BrainPlayer.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainPlayer.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainPlayer.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainPlayer.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainPlayer.o: /usr/include/alloca.h Field.h
BrainPlayerManager.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h
BrainPlayerManager.o: Brick.h /usr/include/stdlib.h /usr/include/features.h
BrainPlayerManager.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainPlayerManager.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainPlayerManager.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
BrainPlayerManager.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayerManager.o: /usr/include/time.h /usr/include/endian.h
BrainPlayerManager.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainPlayerManager.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainPlayerManager.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainPlayerManager.o: /usr/include/sys/sysmacros.h
BrainPlayerManager.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
BrainPlayerManager.o: Field.h Brainblast.h BrainSound.h BrainState.h
BrainPlayerManager.o: /usr/include/assert.h TextListener.h HighScore.h
BrainPlayerManager.o: BrainAI.h
BrainSoundBASS.o: BrainSound.h /usr/include/assert.h /usr/include/features.h
BrainSoundBASS.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainSoundBASS.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSoundBASS.o: /usr/include/gnu/stubs-64.h
BrainSoundFMOD.o: BrainSound.h ../fmod/api/inc/fmod.h /usr/include/assert.h
BrainSoundFMOD.o: /usr/include/features.h /usr/include/bits/predefs.h
BrainSoundFMOD.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSoundFMOD.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
BrainSoundSDL.o: BrainSound.h
BrainSprite.o: bbc.h
BrainState.o: /usr/include/assert.h /usr/include/features.h
BrainState.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainState.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainState.o: /usr/include/gnu/stubs-64.h BrainText.h TextListener.h
Brainblast.o: /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
Brainblast.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Brainblast.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
Brainblast.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Brainblast.o: /usr/include/time.h /usr/include/endian.h
Brainblast.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
Brainblast.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brainblast.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brainblast.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brainblast.o: /usr/include/alloca.h bbc.h Puzzle.h Brick.h Field.h
Brainblast.o: BrainSprite.h BrainSound.h BrainState.h /usr/include/assert.h
Brainblast.o: BrainText.h TextListener.h
Brick.o: bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
Brick.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Brick.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
Brick.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Brick.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
Brick.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
Brick.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Brick.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Brick.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
HighScore.o: bbc.h
Puzzle.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Puzzle.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
Puzzle.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Puzzle.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
Puzzle.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Puzzle.o: /usr/include/time.h /usr/include/endian.h
Puzzle.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
Puzzle.o: /usr/include/sys/select.h /usr/include/bits/select.h
Puzzle.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Puzzle.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Puzzle.o: /usr/include/alloca.h Field.h
