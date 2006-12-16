GCC = g++
CFLAGS = -g #-Weffc++ -Wall -pedantic -Wextra

SDL_LIBS = $(shell sdl-config --libs)
SDL_FLAGS = $(shell sdl-config --cflags)

OBJECTS = bbc.o Brainblast.o Brick.o Puzzle.o Field.o BrainSprite.o

all: Brainblast

Brainblast: $(OBJECTS)
	$(GCC) -o Brainblast $(OBJECTS) ../../kyra/engine/libkyra.a $(SDL_LIBS) 

$(OBJECTS): %.o: %.cpp
	$(GCC) $(CFLAGS) $(SDL_FLAGS) -c $<

remake: 
	make clean; make

clean:
	rm -rfv *.o Brainblast *~
