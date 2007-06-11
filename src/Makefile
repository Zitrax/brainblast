GCC       = g++
CFLAGS    = -g -Weffc++ -Wall -Wextra #-pedantic 

CFLAGS   += -I../include

SDL_LIBS  = $(shell sdl-config --libs)
SDL_FLAGS = $(shell sdl-config --cflags)
KYRALIB   = /usr/local/lib/libkyra.a
IMAGES    = ../images

OBJECTS = bbc.o Brainblast.o Brick.o Puzzle.o Field.o BrainSprite.o main.o

all: Brainblast

$(IMAGES)/bb.h:
	cd $(IMAGES) && \
	krencoder -pBB_ bb.h bricks.xml

Brainblast: $(OBJECTS)
	$(GCC) -o $@ $(OBJECTS) $(KYRALIB) -lengine $(SDL_LIBS) 

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
Brainblast.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Brainblast.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brainblast.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Brainblast.o: /usr/include/time.h /usr/include/endian.h
Brainblast.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Brainblast.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Brainblast.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Brainblast.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Brainblast.o: /usr/include/alloca.h /usr/include/assert.h bbc.h Puzzle.h
Brainblast.o: Brick.h Field.h BrainSprite.h ../images/bb.h
BrainSprite.o: BrainSprite.h Brainblast.h /usr/include/stdlib.h
BrainSprite.o: /usr/include/features.h /usr/include/sys/cdefs.h
BrainSprite.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
BrainSprite.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
BrainSprite.o: /usr/include/bits/typesizes.h /usr/include/time.h
BrainSprite.o: /usr/include/endian.h /usr/include/bits/endian.h
BrainSprite.o: /usr/include/sys/select.h /usr/include/bits/select.h
BrainSprite.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
BrainSprite.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
BrainSprite.o: /usr/include/bits/sched.h /usr/include/alloca.h
BrainSprite.o: /usr/include/assert.h bbc.h Puzzle.h Brick.h Field.h
Brick.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Brick.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brick.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Brick.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
Brick.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brick.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brick.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brick.o: /usr/include/bits/sched.h /usr/include/alloca.h
main.o: Brainblast.h /usr/include/stdlib.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
main.o: /usr/include/sys/types.h /usr/include/bits/types.h
main.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
main.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
main.o: /usr/include/sys/select.h /usr/include/bits/select.h
main.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
main.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
main.o: /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/assert.h
main.o: bbc.h Puzzle.h Brick.h Field.h BrainSprite.h
Puzzle.o: Puzzle.h Brick.h bbc.h /usr/include/stdlib.h
Puzzle.o: /usr/include/features.h /usr/include/sys/cdefs.h
Puzzle.o: /usr/include/gnu/stubs.h /usr/include/sys/types.h
Puzzle.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
Puzzle.o: /usr/include/bits/typesizes.h /usr/include/time.h
Puzzle.o: /usr/include/endian.h /usr/include/bits/endian.h
Puzzle.o: /usr/include/sys/select.h /usr/include/bits/select.h
Puzzle.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Puzzle.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Puzzle.o: /usr/include/bits/sched.h /usr/include/alloca.h Field.h
Brainblast.o: /usr/include/stdlib.h /usr/include/features.h
Brainblast.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Brainblast.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brainblast.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Brainblast.o: /usr/include/time.h /usr/include/endian.h
Brainblast.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Brainblast.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Brainblast.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Brainblast.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Brainblast.o: /usr/include/alloca.h /usr/include/assert.h bbc.h Puzzle.h
Brainblast.o: Brick.h Field.h BrainSprite.h
Brick.o: bbc.h /usr/include/stdlib.h /usr/include/features.h
Brick.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Brick.o: /usr/include/sys/types.h /usr/include/bits/types.h
Brick.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Brick.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
Brick.o: /usr/include/sys/select.h /usr/include/bits/select.h
Brick.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Brick.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Brick.o: /usr/include/bits/sched.h /usr/include/alloca.h
Puzzle.o: Brick.h bbc.h /usr/include/stdlib.h /usr/include/features.h
Puzzle.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Puzzle.o: /usr/include/sys/types.h /usr/include/bits/types.h
Puzzle.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
Puzzle.o: /usr/include/time.h /usr/include/endian.h
Puzzle.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Puzzle.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Puzzle.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
Puzzle.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Puzzle.o: /usr/include/alloca.h Field.h
