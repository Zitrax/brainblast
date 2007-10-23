/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006
 * @author Daniel Bengtsson
 */
// -*- mode: C++; -*-

#ifndef BRAINBLAST_H
#define BRAINBLAST_H

#undef NDEBUG

#include <stdlib.h>
#include <fstream>

#include <wSDL.h>
#include <wkyra.h>

#include <assert.h>
#include "bbc.h"
#include "Puzzle.h"
#include "Brick.h"
#include "BrainSprite.h"

using namespace std;

namespace brain
{

//! The total number of different brick types (different symbols)
	const int NOF_BRICK_TYPES = 5;
	
//! The vide mode to use
	const int VIDEOX = 1024;
	const int VIDEOY = 768;
	const int VIDEOBITS = 32;
	
//! Margin to be used in board/field layout
	const int MARGIN = 10;
}

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

	static BrainBlast* instance() { return s_instance; }

    //! Check if a solution is correct
    bool checkSolution(Puzzle* puzzle);

    //! Start a new game
    bool startGame();

	void createStar();

	/**
	 * 
	 */
	BrainSprite* reparentSprite(BrainSprite* bs, BrainSprite* parent);
	
private:

	static BrainBlast* s_instance;

    Puzzle* m_currentLvl;

    SDL_Surface* m_screen;
    SDL_Rect*    m_field1;
    SDL_Rect*    m_field2;

    Brick**      m_bricks;

	KrEngine*    m_engine;

	std::vector<BrainSprite*> m_sprites;

    const Uint32 red, blue, black, green, white, yellow, cyan, magenta;

    //! Returns true if created succesfully, false otherwise
    bool makeLevel(int lvl);

    void createBricks();

    void drawBoard(SDL_Surface* s, SDL_Rect* dim, Puzzle* p);

    void drawBrickAtIdx(SDL_Surface* s, Puzzle* p, SDL_Rect* dim, int idx, bool solution=false);

    void drawAllBricks(SDL_Surface* s, Puzzle* p, SDL_Rect* dim, bool solution=false);

    //! Sets up a new game
    bool initGame(int lvl=1);
    bool initGameKyra();

    //! Used to clean up for all pointers
    void cleanup();

    int eventLoop();

    void handleKeyEvent(SDL_KeyboardEvent* key);
};

#endif
