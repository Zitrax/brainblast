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
#include "BrainPlayerManager.h"
#include "BrainSound.h"
#include "BrainAI.h"

class BrainAI;

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
    bool startGame();

	BrainSprite* addSprite();

	BrainSprite* reparentSprite(BrainSprite* bs, KrImNode* parent);
	//! Drop eventual carried sprite and add it back to the bg tree.
	void dropPlayerSprite(BrainPlayer* player);

	enum sounds {
		CLICK,
		BOUNCE
	};
	
	bool playSample(enum sounds sound) const { return m_sound_sdl->playSample(sound); }

	void drawText(const char* text, SDL_Rect pos, int size=16);

	bool changeLevel(int lvl);

	vector<BrainSprite*>& getAllSprites() { return m_sprites; }

	/**
	 * Perform a select for the player at lvl.
	 */
	void select(Puzzle& lvl, BrainPlayer& player);

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

    //! Returns true if created succesfully, false otherwise
    bool makeLevel(int lvl);
	/**
	 * Creates random levels with specified
	 * width and height, and n number of bricks.
	 */
	void makeRandomLevel(int w,int h,int n);

    void createBricks();
	bool createBoards();

    //! Sets up a new game
    bool initGame(int lvl=1);
    bool initGameKyra();

    //! Used to clean up for all pointers
    void cleanup();
	
	/**
	 * This is the loop that runs continuously during
	 * gameplay to move all sprites and handle key events.
	 */ 
    int eventLoop();

	/**
	 * The one and only instance of this class. It 
	 * can be accessed with Brainblast::instance().
	 */
	static Brainblast* s_instance;

	/**
	 * This is the time the game waits before hiding
	 * the solution and the game starts.
	 */
	static const double WAITTIME = 10.0;
	
	bool m_play; // Tells if the user has aborted the wait

	time_t m_start_time;

	/**
	 * Handles all the sounds (effects and music)
	 */
	BrainSound* m_sound_sdl;
	BrainSound* m_sound_bass;

	vector<Puzzle*> m_current_levels;
	vector<SDL_Rect> m_fields;
	int m_current_lvl;

    SDL_Surface* m_screen;

	map<int,Brick*> m_bricks;
	int          m_total_bricks;

	KrEngine*    m_engine;

	KrImNode*    m_bgTree;
	KrImNode*    m_fgTree;

	vector<BrainSprite*> m_sprites;

	KrResourceVault* m_bg_vault;
	KrSprite*        m_bg_sprite;

	// Text
	KrFontResource* m_font;
	KrTextBox*      m_score_text_box;
	KrTextBox*      m_center_text_box;

	BrainPlayerManager* m_player_manager;
};

#endif
