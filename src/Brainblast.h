/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

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
#include "BrainSound.h"

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
	const int MARGIN = 0;
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

	static Brainblast* instance() { return s_instance; }

	KrEngine* engine() const { return m_engine; }

    //! Check if a solution is correct
    bool checkSolution(Puzzle* puzzle);

    //! Start a new game
    bool startGame();

	BrainSprite* createStar();

	/**
	 * 
	 */
	BrainSprite* reparentSprite(BrainSprite* bs, BrainSprite* parent);
	
private:

	static Brainblast* s_instance;

	BrainSound* m_sound;

    Puzzle* m_currentLvl1;
	Puzzle* m_currentLvl2;

    SDL_Surface* m_screen;
    SDL_Rect*    m_field1;
    SDL_Rect*    m_field2;

    Brick**      m_bricks;

	KrEngine*    m_engine;

	KrImNode*    m_bgTree;
	KrImNode*    m_fgTree;

	std::vector<BrainSprite*> m_sprites;

    const Uint32 red, blue, black, green, white, yellow, cyan, magenta;

    //! Returns true if created succesfully, false otherwise
    bool makeLevel(int lvl);

    void createBricks();
	bool createBoards();

    void drawBoard(SDL_Surface* s, SDL_Rect* dim, Puzzle* p);
	void drawBoards();

    //! Sets up a new game
    bool initGame(int lvl=1);
    bool initGameKyra();

    //! Used to clean up for all pointers
    void cleanup();

    int eventLoop();

    void handleKeyEvent(SDL_KeyboardEvent* key);
};

#endif
