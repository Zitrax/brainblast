GCC = g++
CFLAGS = -g -Weffc++ -Wall -pedantic -Wextra

all: Brainblast

Brainblast: bbc.o Brainblast.o Puzzle.o Brick.o
	$(GCC) `sdl-config --libs` bbc.o Brainblast.o Brick.o Puzzle.o -o Brainblast

Brainblast.o: Brainblast.cpp Brainblast.h
	$(GCC) $(CFLAGS) -c `sdl-config --cflags` Brainblast.cpp 

bbc.o: bbc.cpp bbc.h
	$(GCC) $(CFLAGS) -c bbc.cpp

Brick.o: Brick.cpp Brick.h
	$(GCC) $(CFLAGS) -c Brick.cpp

Puzzle.o: Puzzle.cpp Puzzle.h Brick.o
	$(GCC) $(CFLAGS) -c Puzzle.cpp

clean:
	rm -rf *.o Brainblast *~
