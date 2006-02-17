// -*- mode: C++; -*-

#ifndef BRAINBLAST_H
#define BRAINBLAST_H

#undef NDEBUG

#include <stdlib.h>
#include <fstream>
#include <SDL/SDL.h>
#include <assert.h>
#include "bbc.h"
#include "Puzzle.h"
#include "Brick.h"

using namespace std;

//! The total number of different brick types (different symbols)
const int NOF_BRICK_TYPES = 5;

//! The width in number of bricks of the puzzle (X)
int PWIDTH = 5;

//! The height in number of bricks of the puzzle (Y)
int PHEIGHT = 5;

//! The vide mode to use
const int VIDEOX = 640;
const int VIDEOY = 480;
const int VIDEOBITS = 32;

//! Margin to be used in board/field layout
int MARGIN = 10;

int main(int argc, char *argv[]);

/**
 * Starting a Brainblast game should be as simple as 
 * creating an object of this class and calling
 * startGame();
 */
class Brainblast
{
public:
    
    Brainblast();
    Brainblast(const Brainblast& bb);
    ~Brainblast();

    Brainblast& operator=(const Brainblast& bb);

    //! Check if a solution is correct
    bool checkSolution(Puzzle* puzzle);

    //! Start a new game
    void startGame();

private:
    Puzzle* m_currentLvl;

    SDL_Surface* m_screen;
    SDL_Rect*    m_field1;
    SDL_Rect*    m_field2;

    Brick** bricks;

    const Uint32 red, blue, black, green, white, yellow, cyan, magenta;

    //! Returns true if created succesfully, false otherwise
    bool makeLevel(int lvl);

    void createBricks();

    void drawBoard(SDL_Surface* s, SDL_Rect* dim, Puzzle* p, int xTiles, int yTiles);

    void drawBrickAtIdx(SDL_Surface* s, Puzzle* p, SDL_Rect* dim, int idx, bool solution=false);

    void drawAllBricks(SDL_Surface* s, Puzzle* p, SDL_Rect* dim, bool solution=false);

    void placeBricksRandom(int numberOfBricks = (PWIDTH*PHEIGHT)/2);

    //! Sets up a new game
    void initGame(int lvl=1);

    //! Used to clean up for all pointers
    void cleanup();

    int eventLoop();

    void handleKeyEvent(SDL_KeyboardEvent* key);

};

#endif
