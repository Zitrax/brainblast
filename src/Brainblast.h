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
#include "BrainPlayer.h"
#include "BrainSound.h"

using namespace std;

namespace brain
{

//! The total number of different brick types (different symbols)
	const int MAX_NOF_BRICK_TYPES = 25;
	
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
    ~Brainblast();

	static Brainblast* instance() { return s_instance; }

	KrEngine* engine() const { return m_engine; }

    //! Check if a solution is correct
    bool checkSolution(Puzzle* puzzle);

    //! Start a new game
    bool startGame(int players);

	BrainSprite* addSprite();

	/**
	 * 
	 */
	BrainSprite* reparentSprite(BrainSprite* bs, BrainSprite* parent);

	enum sounds {
		CLICK,
		BOUNCE
	};
	
	bool playSample(enum sounds sound) const { return m_sound->playSample(sound); }

	void drawText(const char* text, SDL_Rect pos, int size=16);

	bool changeLevel(int lvl);

private:

    Brainblast(const Brainblast& bb);
    Brainblast& operator=(const Brainblast& bb);

	bool setupFields(int players);
	/**
	 * Removes all sprites on the playfield
	 * except the players and the ones belonging
	 * to the puzzles.
	 */
	void clearFloor();

	static Brainblast* s_instance;

	static const double WAITTIME = 3.0;

	BrainSound* m_sound;

	BrainPlayer* m_player1; // The player
	int m_players;

    Puzzle* m_currentLvl1;
	Puzzle* m_currentLvl2;
	int m_current_lvl;

    SDL_Surface* m_screen;
    SDL_Rect*    m_field1;
    SDL_Rect*    m_field2;

	std::map<int,Brick*> m_bricks;
	int          m_total_bricks;

	KrEngine*    m_engine;

	KrImNode*    m_bgTree;
	KrImNode*    m_fgTree;

	time_t m_start_time;

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
