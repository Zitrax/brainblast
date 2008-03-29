GCC       = g++
CFLAGS    = -g -Weffc++ -Wall -Wextra #-pedantic 

CFLAGS   += -I../include

SDL_LIBS  = $(shell sdl-config --libs) -lSDL_mixer
SDL_FLAGS = $(shell sdl-config --cflags)
IMAGES    = ../images

OBJECTS = bbc.o Brainblast.o Brick.o Puzzle.o Field.o BrainSprite.o BrainSound.o main.o

all: Brainblast

$(IMAGES)/bb.h: ../images/bricks.xml
	cd $(IMAGES) && \
	krencoder -pBB_ bb.h bricks.xml

Brainblast: $(OBJECTS)
	$(GCC) -o $@ $(OBJECTS) -lkyra -lengine $(SDL_LIBS) 

$(OBJECTS): %.o: %.cpp
	$(GCC) $(CFLAGS) $(SDL_FLAGS) -c $<

remake: 
	make clean; make

clean:
	rm -rfv *.o Brainblast *~ \
	$(IMAGES)/bb.h $(IMAGES)/bb.dat

# DO NOT DELETE

bbc.o: bbc.h
Brainblast.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brainblast.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Brainblast.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brainblast.o: /usr/include/bits/typesizes.h /usr/include/time.h
Brainblast.o: /usr/include/endian.h /usr/include/bits/endian.h
Brainblast.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brainblast.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brainblast.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brainblast.o: /usr/include/alloca.h /usr/include/assert.h bbc.h Puzzle.h
Brainblast.o: Brick.h Field.h BrainSprite.h BrainSound.h ../images/bb.h
Brainblast.o: /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Brainblast.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Brainblast.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brainblast.o: /usr/include/bits/typesizes.h /usr/include/time.h
Brainblast.o: /usr/include/endian.h /usr/include/bits/endian.h
Brainblast.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brainblast.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brainblast.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brainblast.o: /usr/include/alloca.h /usr/include/assert.h bbc.h Puzzle.h
Brainblast.o: Brick.h Field.h BrainSprite.h BrainSound.h
BrainSound.o: BrainSound.h bbc.h
BrainSprite.o: BrainSprite.h Brainblast.h /usr/include/stdlib.h
BrainSprite.o: /usr/include/features.h /usr/include/sys/cdefs.h
BrainSprite.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
BrainSprite.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
BrainSprite.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
BrainSprite.o: /usr/include/time.h /usr/include/endian.h
BrainSprite.o: /usr/include/bits/endian.h /usr/include/sys/select.h
BrainSprite.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
BrainSprite.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
BrainSprite.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
BrainSprite.o: /usr/include/assert.h bbc.h Puzzle.h Brick.h Field.h
BrainSprite.o: BrainSound.h
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
main.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
main.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
main.o: /usr/include/sys/types.h /usr/include/bits/types.h
main.o: /usr/include/bits/typesizes.h /usr/include/time.h
main.o: /usr/include/endian.h /usr/include/bits/endian.h
main.o: /usr/include/sys/select.h /usr/include/bits/select.h
main.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
main.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
main.o: /usr/include/alloca.h /usr/include/assert.h bbc.h Puzzle.h Brick.h
main.o: Field.h BrainSprite.h BrainSound.h
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
