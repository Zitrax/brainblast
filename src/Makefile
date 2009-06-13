GCC       = g++
CFLAGS    = -Weffc++ -Wall -Wextra -pedantic

DEBUG_BB  = YES

ifeq ($(DEBUG_BB), YES)
CFLAGS   += -D_DEBUG -g
else
CFLAGS   += -O3 -DNDEBUG
endif

CFLAGS   += -I../include

LIBS      = /usr/local/lib/libengine.a /usr/local/lib/libkyra.a  -L../fmod/api -lfmod-3.75 -lrt
SDL_LIBS  = $(shell sdl-config --static-libs) -lSDL_image #-lSDL_mixer
SDL_FLAGS = $(shell sdl-config --cflags)
IMAGES    = ../images

OBJECTS = bbc.o consolefont.o Brainblast.o Brick.o Puzzle.o Field.o BrainSprite.o BrainSound.o \
          BrainPlayer.o BrainAI.o BrainPlayerManager.o main.o BrainSoundFMOD.o HighScore.o BrainText.o \
          BrainState.o #BrainSoundSDL.o BrainSoundBASS.o

VERSION = $(shell perl -wne 'if(/brainblast \((.*)\)/){print "$$1";exit;}' < ../deb/debian/changelog)

all: brainblast

$(IMAGES)/bb.h: ../images/bricks.xml 
	cd $(IMAGES) && \
	krencoder -pBB_ bb.h bricks.xml

$(IMAGES)/bb_bg.h: ../images/bg.xml
	cd $(IMAGES) && \
	krencoder -pBB_BG_ bb_bg.h bg.xml

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

BrainAI.o: BrainAI.h BrainPlayer.h BrainSprite.h bbc.h Puzzle.h Brick.h
BrainAI.o: /usr/include/stdlib.h /usr/include/features.h
BrainAI.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainAI.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
BrainAI.o: /usr/include/sys/types.h /usr/include/bits/types.h
BrainAI.o: /usr/include/bits/typesizes.h /usr/include/time.h
BrainAI.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainAI.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainAI.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainAI.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainAI.o: /usr/include/alloca.h Field.h Brainblast.h BrainSound.h
BrainAI.o: BrainState.h
BrainPlayer.o: BrainPlayer.h BrainSprite.h bbc.h Puzzle.h Brick.h
BrainPlayer.o: /usr/include/stdlib.h /usr/include/features.h
BrainPlayer.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainPlayer.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
BrainPlayer.o: /usr/include/sys/types.h /usr/include/bits/types.h
BrainPlayer.o: /usr/include/bits/typesizes.h /usr/include/time.h
BrainPlayer.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainPlayer.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainPlayer.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainPlayer.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainPlayer.o: /usr/include/alloca.h Field.h
BrainPlayerManager.o: BrainPlayerManager.h BrainPlayer.h BrainSprite.h bbc.h
BrainPlayerManager.o: Puzzle.h Brick.h /usr/include/stdlib.h
BrainPlayerManager.o: /usr/include/features.h /usr/include/sys/cdefs.h
BrainPlayerManager.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainPlayerManager.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
BrainPlayerManager.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayerManager.o: /usr/include/time.h /usr/include/endian.h
BrainPlayerManager.o: /usr/include/bits/endian.h /usr/include/sys/select.h
BrainPlayerManager.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainPlayerManager.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainPlayerManager.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
BrainPlayerManager.o: Field.h Brainblast.h BrainSound.h BrainState.h
BrainPlayerManager.o: HighScore.h BrainAI.h ../images/bb.h
BrainSoundBASS.o: BrainSoundBASS.h BrainSound.h /usr/include/assert.h
BrainSoundBASS.o: /usr/include/features.h /usr/include/sys/cdefs.h
BrainSoundBASS.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSoundBASS.o: /usr/include/gnu/stubs-32.h ../bassmod/bassmod.h
BrainSoundBASS.o: /usr/include/stdio.h /usr/include/bits/types.h
BrainSoundBASS.o: /usr/include/bits/typesizes.h /usr/include/libio.h
BrainSoundBASS.o: /usr/include/_G_config.h /usr/include/wchar.h
BrainSoundBASS.o: /usr/include/bits/stdio_lim.h
BrainSoundBASS.o: /usr/include/bits/sys_errlist.h
BrainSoundFMOD.o: BrainSoundFMOD.h BrainSound.h ../fmod/api/inc/fmod.h
BrainSoundFMOD.o: /usr/include/assert.h /usr/include/features.h
BrainSoundFMOD.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSoundFMOD.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
BrainSoundFMOD.o: ../fmod/api/inc/fmod_errors.h /usr/include/stdio.h
BrainSoundFMOD.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainSoundFMOD.o: /usr/include/libio.h /usr/include/_G_config.h
BrainSoundFMOD.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
BrainSoundFMOD.o: /usr/include/bits/sys_errlist.h /usr/include/malloc.h
BrainSoundFMOD.o: /usr/include/string.h
BrainSoundSDL.o: BrainSoundSDL.h BrainSound.h bbc.h
BrainSprite.o: BrainSprite.h bbc.h Brainblast.h /usr/include/stdlib.h
BrainSprite.o: /usr/include/features.h /usr/include/sys/cdefs.h
BrainSprite.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSprite.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
BrainSprite.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainSprite.o: /usr/include/time.h /usr/include/endian.h
BrainSprite.o: /usr/include/bits/endian.h /usr/include/sys/select.h
BrainSprite.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainSprite.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainSprite.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
BrainSprite.o: Puzzle.h Brick.h Field.h BrainSound.h BrainState.h
Brainblast.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brainblast.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Brainblast.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brainblast.o: /usr/include/bits/typesizes.h /usr/include/time.h
Brainblast.o: /usr/include/endian.h /usr/include/bits/endian.h
Brainblast.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brainblast.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brainblast.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brainblast.o: /usr/include/alloca.h bbc.h Puzzle.h Brick.h Field.h
Brainblast.o: BrainSprite.h BrainSound.h BrainState.h ../images/bb.h
Brainblast.o: ../images/bb_bg.h consolefont.h BrainSoundFMOD.h
Brainblast.o: ../fmod/api/inc/fmod.h /usr/include/assert.h
Brainblast.o: BrainPlayerManager.h BrainPlayer.h HighScore.h BrainAI.h
Brick.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brick.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Brick.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brick.o: /usr/include/bits/typesizes.h /usr/include/time.h
Brick.o: /usr/include/endian.h /usr/include/bits/endian.h
Brick.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brick.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brick.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brick.o: /usr/include/alloca.h
HighScore.o: HighScore.h bbc.h /usr/include/errno.h /usr/include/features.h
HighScore.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
HighScore.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
HighScore.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
HighScore.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
HighScore.o: /usr/include/asm-generic/errno-base.h
Puzzle.o: Puzzle.h Brick.h bbc.h /usr/include/stdlib.h
Puzzle.o: /usr/include/features.h /usr/include/sys/cdefs.h
Puzzle.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Puzzle.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
Puzzle.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Puzzle.o: /usr/include/time.h /usr/include/endian.h
Puzzle.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Puzzle.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Puzzle.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Puzzle.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h Field.h
Puzzle.o: Brainblast.h BrainSprite.h BrainSound.h BrainState.h ../images/bb.h
bbc.o: bbc.h
consolefont.o: consolefont.h
main.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
main.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
main.o: /usr/include/sys/types.h /usr/include/bits/types.h
main.o: /usr/include/bits/typesizes.h /usr/include/time.h
main.o: /usr/include/endian.h /usr/include/bits/endian.h
main.o: /usr/include/sys/select.h /usr/include/bits/select.h
main.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
main.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
main.o: /usr/include/alloca.h bbc.h Puzzle.h Brick.h Field.h BrainSprite.h
main.o: BrainSound.h BrainState.h
BrainAI.o: BrainPlayer.h BrainSprite.h bbc.h Puzzle.h Brick.h
BrainAI.o: /usr/include/stdlib.h /usr/include/features.h
BrainAI.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainAI.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
BrainAI.o: /usr/include/sys/types.h /usr/include/bits/types.h
BrainAI.o: /usr/include/bits/typesizes.h /usr/include/time.h
BrainAI.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainAI.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainAI.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainAI.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainAI.o: /usr/include/alloca.h Field.h
BrainPlayer.o: BrainSprite.h bbc.h Puzzle.h Brick.h /usr/include/stdlib.h
BrainPlayer.o: /usr/include/features.h /usr/include/sys/cdefs.h
BrainPlayer.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainPlayer.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
BrainPlayer.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayer.o: /usr/include/time.h /usr/include/endian.h
BrainPlayer.o: /usr/include/bits/endian.h /usr/include/sys/select.h
BrainPlayer.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainPlayer.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainPlayer.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h Field.h
BrainPlayerManager.o: BrainPlayer.h BrainSprite.h bbc.h Puzzle.h Brick.h
BrainPlayerManager.o: /usr/include/stdlib.h /usr/include/features.h
BrainPlayerManager.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainPlayerManager.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
BrainPlayerManager.o: /usr/include/sys/types.h /usr/include/bits/types.h
BrainPlayerManager.o: /usr/include/bits/typesizes.h /usr/include/time.h
BrainPlayerManager.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainPlayerManager.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainPlayerManager.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainPlayerManager.o: /usr/include/sys/sysmacros.h
BrainPlayerManager.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
BrainPlayerManager.o: Field.h Brainblast.h BrainSound.h BrainState.h
BrainPlayerManager.o: HighScore.h BrainAI.h
BrainSoundBASS.o: BrainSound.h /usr/include/assert.h /usr/include/features.h
BrainSoundBASS.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSoundBASS.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
BrainSoundFMOD.o: BrainSound.h ../fmod/api/inc/fmod.h /usr/include/assert.h
BrainSoundFMOD.o: /usr/include/features.h /usr/include/sys/cdefs.h
BrainSoundFMOD.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSoundFMOD.o: /usr/include/gnu/stubs-32.h
BrainSoundSDL.o: BrainSound.h
BrainSprite.o: bbc.h
Brainblast.o: /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brainblast.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Brainblast.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brainblast.o: /usr/include/bits/typesizes.h /usr/include/time.h
Brainblast.o: /usr/include/endian.h /usr/include/bits/endian.h
Brainblast.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brainblast.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brainblast.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brainblast.o: /usr/include/alloca.h bbc.h Puzzle.h Brick.h Field.h
Brainblast.o: BrainSprite.h BrainSound.h BrainState.h
Brick.o: bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brick.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Brick.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brick.o: /usr/include/bits/typesizes.h /usr/include/time.h
Brick.o: /usr/include/endian.h /usr/include/bits/endian.h
Brick.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brick.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brick.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brick.o: /usr/include/alloca.h
HighScore.o: bbc.h
Puzzle.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Puzzle.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Puzzle.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Puzzle.o: /usr/include/sys/types.h /usr/include/bits/types.h
Puzzle.o: /usr/include/bits/typesizes.h /usr/include/time.h
Puzzle.o: /usr/include/endian.h /usr/include/bits/endian.h
Puzzle.o: /usr/include/sys/select.h /usr/include/bits/select.h
Puzzle.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Puzzle.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Puzzle.o: /usr/include/alloca.h Field.h
