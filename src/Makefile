GCC       = ./g++
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
BrainAI.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainAI.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainAI.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
BrainAI.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
BrainAI.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainAI.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainAI.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
BrainAI.o: /usr/include/time.h /usr/include/sys/select.h
BrainAI.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainAI.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainAI.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h Field.h
BrainAI.o: Brainblast.h BrainSound.h BrainState.h /usr/include/assert.h
BrainAI.o: TextListener.h
BrainPlayer.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainPlayer.o: /usr/include/stdlib.h /usr/include/features.h
BrainPlayer.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainPlayer.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainPlayer.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
BrainPlayer.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
BrainPlayer.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainPlayer.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayer.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
BrainPlayer.o: /usr/include/time.h /usr/include/sys/select.h
BrainPlayer.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainPlayer.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainPlayer.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h Field.h
BrainPlayerManager.o: BrainPlayerManager.h BrainPlayer.h BrainSprite.h bbc.h
BrainPlayerManager.o: BrainText.h Puzzle.h Brick.h /usr/include/stdlib.h
BrainPlayerManager.o: /usr/include/features.h /usr/include/stdc-predef.h
BrainPlayerManager.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainPlayerManager.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainPlayerManager.o: /usr/include/bits/waitflags.h
BrainPlayerManager.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
BrainPlayerManager.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainPlayerManager.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayerManager.o: /usr/include/bits/byteswap-16.h
BrainPlayerManager.o: /usr/include/sys/types.h /usr/include/time.h
BrainPlayerManager.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainPlayerManager.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainPlayerManager.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
BrainPlayerManager.o: /usr/include/bits/stdlib-float.h Field.h Brainblast.h
BrainPlayerManager.o: BrainSound.h BrainState.h /usr/include/assert.h
BrainPlayerManager.o: TextListener.h HighScore.h BrainAI.h ../images/bb.h
BrainSoundBASS.o: BrainSoundBASS.h BrainSound.h /usr/include/assert.h
BrainSoundBASS.o: /usr/include/features.h /usr/include/stdc-predef.h
BrainSoundBASS.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainSoundBASS.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSoundBASS.o: ../bassmod/bassmod.h /usr/include/stdio.h
BrainSoundBASS.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainSoundBASS.o: /usr/include/libio.h /usr/include/_G_config.h
BrainSoundBASS.o: /usr/include/wchar.h /usr/include/bits/wchar.h
BrainSoundBASS.o: /usr/include/xlocale.h /usr/include/bits/stdio_lim.h
BrainSoundBASS.o: /usr/include/bits/sys_errlist.h
BrainSoundFMOD.o: BrainSoundFMOD.h BrainSound.h ../fmod/api/inc/fmod.h
BrainSoundFMOD.o: /usr/include/assert.h /usr/include/features.h
BrainSoundFMOD.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainSoundFMOD.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSoundFMOD.o: /usr/include/gnu/stubs.h ../fmod/api/inc/fmod_errors.h
BrainSoundFMOD.o: /usr/include/stdio.h /usr/include/bits/types.h
BrainSoundFMOD.o: /usr/include/bits/typesizes.h /usr/include/libio.h
BrainSoundFMOD.o: /usr/include/_G_config.h /usr/include/wchar.h
BrainSoundFMOD.o: /usr/include/bits/wchar.h /usr/include/xlocale.h
BrainSoundFMOD.o: /usr/include/bits/stdio_lim.h
BrainSoundFMOD.o: /usr/include/bits/sys_errlist.h /usr/include/malloc.h
BrainSoundFMOD.o: /usr/include/string.h
BrainSoundFMODEX.o: BrainSoundFMODEX.h BrainSound.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod.hpp
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_codec.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_dsp.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_memoryinfo.h
BrainSoundFMODEX.o: /usr/include/assert.h /usr/include/features.h
BrainSoundFMODEX.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainSoundFMODEX.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSoundFMODEX.o: /usr/include/gnu/stubs.h /usr/include/stdio.h
BrainSoundFMODEX.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainSoundFMODEX.o: /usr/include/libio.h /usr/include/_G_config.h
BrainSoundFMODEX.o: /usr/include/wchar.h /usr/include/bits/wchar.h
BrainSoundFMODEX.o: /usr/include/xlocale.h /usr/include/bits/stdio_lim.h
BrainSoundFMODEX.o: /usr/include/bits/sys_errlist.h /usr/include/malloc.h
BrainSoundFMODEX.o: /usr/include/string.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_errors.h
BrainSoundSDL.o: BrainSoundSDL.h BrainSound.h bbc.h
BrainSprite.o: BrainSprite.h bbc.h Brainblast.h /usr/include/stdlib.h
BrainSprite.o: /usr/include/features.h /usr/include/stdc-predef.h
BrainSprite.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainSprite.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSprite.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
BrainSprite.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainSprite.o: /usr/include/bits/byteswap.h /usr/include/bits/types.h
BrainSprite.o: /usr/include/bits/typesizes.h /usr/include/bits/byteswap-16.h
BrainSprite.o: /usr/include/sys/types.h /usr/include/time.h
BrainSprite.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainSprite.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainSprite.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
BrainSprite.o: /usr/include/bits/stdlib-float.h Puzzle.h Brick.h Field.h
BrainSprite.o: BrainSound.h BrainState.h /usr/include/assert.h BrainText.h
BrainSprite.o: TextListener.h
BrainState.o: BrainState.h /usr/include/assert.h /usr/include/features.h
BrainState.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainState.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainState.o: /usr/include/gnu/stubs.h BrainText.h TextListener.h
BrainState.o: Brainblast.h /usr/include/stdlib.h
BrainState.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
BrainState.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainState.o: /usr/include/bits/byteswap.h /usr/include/bits/types.h
BrainState.o: /usr/include/bits/typesizes.h /usr/include/bits/byteswap-16.h
BrainState.o: /usr/include/sys/types.h /usr/include/time.h
BrainState.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainState.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainState.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
BrainState.o: /usr/include/bits/stdlib-float.h bbc.h Puzzle.h Brick.h Field.h
BrainState.o: BrainSprite.h BrainSound.h BrainPlayerManager.h BrainPlayer.h
BrainState.o: HighScore.h BrainAI.h
BrainText.o: BrainText.h consolefont.h bbc.h Brainblast.h
BrainText.o: /usr/include/stdlib.h /usr/include/features.h
BrainText.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainText.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainText.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
BrainText.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
BrainText.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainText.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainText.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
BrainText.o: /usr/include/time.h /usr/include/sys/select.h
BrainText.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainText.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainText.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h Puzzle.h
BrainText.o: Brick.h Field.h BrainSprite.h BrainSound.h BrainState.h
BrainText.o: /usr/include/assert.h TextListener.h
Brainblast.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
Brainblast.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brainblast.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
Brainblast.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
Brainblast.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
Brainblast.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Brainblast.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
Brainblast.o: /usr/include/time.h /usr/include/sys/select.h
Brainblast.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Brainblast.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Brainblast.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h bbc.h
Brainblast.o: Puzzle.h Brick.h Field.h BrainSprite.h BrainSound.h
Brainblast.o: BrainState.h /usr/include/assert.h BrainText.h TextListener.h
Brainblast.o: ../images/bb.h ../images/bb_bg.h BrainPlayerManager.h
Brainblast.o: BrainPlayer.h HighScore.h BrainAI.h BrainSoundFMODEX.h
Brainblast.o: ../fmod/fmodex/api/inc/fmod.hpp ../fmod/fmodex/api/inc/fmod.h
Brainblast.o: ../fmod/fmodex/api/inc/fmod_codec.h
Brainblast.o: ../fmod/fmodex/api/inc/fmod_dsp.h
Brainblast.o: ../fmod/fmodex/api/inc/fmod_memoryinfo.h
Brick.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
Brick.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brick.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
Brick.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
Brick.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
Brick.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Brick.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
Brick.o: /usr/include/time.h /usr/include/sys/select.h
Brick.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Brick.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Brick.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h
HighScore.o: HighScore.h bbc.h /usr/include/errno.h /usr/include/features.h
HighScore.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
HighScore.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
HighScore.o: /usr/include/gnu/stubs.h /usr/include/bits/errno.h
HighScore.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
HighScore.o: /usr/include/asm-generic/errno.h
HighScore.o: /usr/include/asm-generic/errno-base.h
Puzzle.o: Puzzle.h Brick.h bbc.h /usr/include/stdlib.h
Puzzle.o: /usr/include/features.h /usr/include/stdc-predef.h
Puzzle.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
Puzzle.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Puzzle.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
Puzzle.o: /usr/include/endian.h /usr/include/bits/endian.h
Puzzle.o: /usr/include/bits/byteswap.h /usr/include/bits/types.h
Puzzle.o: /usr/include/bits/typesizes.h /usr/include/bits/byteswap-16.h
Puzzle.o: /usr/include/sys/types.h /usr/include/time.h
Puzzle.o: /usr/include/sys/select.h /usr/include/bits/select.h
Puzzle.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Puzzle.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
Puzzle.o: /usr/include/bits/stdlib-float.h Field.h Brainblast.h BrainSprite.h
Puzzle.o: BrainSound.h BrainState.h /usr/include/assert.h BrainText.h
Puzzle.o: TextListener.h ../images/bb.h
TextListener.o: TextListener.h
bbc.o: bbc.h
consolefont.o: consolefont.h
main.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
main.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
main.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
main.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
main.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
main.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
main.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
main.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
main.o: /usr/include/time.h /usr/include/sys/select.h
main.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
main.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
main.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h bbc.h Puzzle.h
main.o: Brick.h Field.h BrainSprite.h BrainSound.h BrainState.h
main.o: /usr/include/assert.h BrainText.h TextListener.h
BrainAI.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainAI.o: /usr/include/stdlib.h /usr/include/features.h
BrainAI.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainAI.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainAI.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
BrainAI.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
BrainAI.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainAI.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainAI.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
BrainAI.o: /usr/include/time.h /usr/include/sys/select.h
BrainAI.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainAI.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainAI.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h Field.h
BrainPlayer.o: BrainSprite.h bbc.h BrainText.h Puzzle.h Brick.h
BrainPlayer.o: /usr/include/stdlib.h /usr/include/features.h
BrainPlayer.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainPlayer.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainPlayer.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
BrainPlayer.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
BrainPlayer.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainPlayer.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayer.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
BrainPlayer.o: /usr/include/time.h /usr/include/sys/select.h
BrainPlayer.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainPlayer.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainPlayer.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h Field.h
BrainPlayerManager.o: BrainPlayer.h BrainSprite.h bbc.h BrainText.h Puzzle.h
BrainPlayerManager.o: Brick.h /usr/include/stdlib.h /usr/include/features.h
BrainPlayerManager.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainPlayerManager.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainPlayerManager.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
BrainPlayerManager.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
BrainPlayerManager.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
BrainPlayerManager.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainPlayerManager.o: /usr/include/bits/byteswap-16.h
BrainPlayerManager.o: /usr/include/sys/types.h /usr/include/time.h
BrainPlayerManager.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainPlayerManager.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainPlayerManager.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
BrainPlayerManager.o: /usr/include/bits/stdlib-float.h Field.h Brainblast.h
BrainPlayerManager.o: BrainSound.h BrainState.h /usr/include/assert.h
BrainPlayerManager.o: TextListener.h HighScore.h BrainAI.h
BrainSoundBASS.o: BrainSound.h /usr/include/assert.h /usr/include/features.h
BrainSoundBASS.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainSoundBASS.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSoundBASS.o: /usr/include/gnu/stubs.h
BrainSoundFMOD.o: BrainSound.h ../fmod/api/inc/fmod.h /usr/include/assert.h
BrainSoundFMOD.o: /usr/include/features.h /usr/include/stdc-predef.h
BrainSoundFMOD.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
BrainSoundFMOD.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSoundFMODEX.o: BrainSound.h ../fmod/fmodex/api/inc/fmod.hpp
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_codec.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_dsp.h
BrainSoundFMODEX.o: ../fmod/fmodex/api/inc/fmod_memoryinfo.h
BrainSoundFMODEX.o: /usr/include/assert.h /usr/include/features.h
BrainSoundFMODEX.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainSoundFMODEX.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainSoundFMODEX.o: /usr/include/gnu/stubs.h
BrainSoundSDL.o: BrainSound.h
BrainSprite.o: bbc.h
BrainState.o: /usr/include/assert.h /usr/include/features.h
BrainState.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
BrainState.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
BrainState.o: /usr/include/gnu/stubs.h BrainText.h TextListener.h
Brainblast.o: /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
Brainblast.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brainblast.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
Brainblast.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
Brainblast.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
Brainblast.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Brainblast.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
Brainblast.o: /usr/include/time.h /usr/include/sys/select.h
Brainblast.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Brainblast.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Brainblast.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h bbc.h
Brainblast.o: Puzzle.h Brick.h Field.h BrainSprite.h BrainSound.h
Brainblast.o: BrainState.h /usr/include/assert.h BrainText.h TextListener.h
Brick.o: bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
Brick.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brick.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
Brick.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
Brick.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
Brick.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Brick.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
Brick.o: /usr/include/time.h /usr/include/sys/select.h
Brick.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Brick.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Brick.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h
HighScore.o: bbc.h
Puzzle.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Puzzle.o: /usr/include/stdc-predef.h /usr/include/bits/predefs.h
Puzzle.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Puzzle.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
Puzzle.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
Puzzle.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
Puzzle.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Puzzle.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
Puzzle.o: /usr/include/time.h /usr/include/sys/select.h
Puzzle.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Puzzle.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Puzzle.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h Field.h
