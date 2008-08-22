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

class BrainAI;
class BrainPlayerManager;
class BrainPlayer;

using namespace std;
using namespace bbc;

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

/**
 * This class is used to report back when a string
 * input is done which was requested using
 * Brainblast::startTextInput()
 *
 * When a string is ready textReady() will be
 * called with the string and it's id.
 */
class TextListener
{
public:
	virtual ~TextListener(){}

	virtual void textReady(string s,int id) = 0;

	static int id() { return m_text_id++; }

private:
	static int m_text_id;
};

/** Functor for use in for_each iterations */
struct text_ready
{
	text_ready(string s, int id) : m_s(s), m_id(id) {}
	void operator() (TextListener* tl) { tl->textReady(m_s,m_id); }
	string m_s;
	int m_id;
};

/**
 * Starting a Brainblast game:
 * 1. Call initGameKyra()
 * 2. Start the event loop: eventLoop()
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

    bool initGameKyra();

	void titleScreen();
	void titleScreenUpdateText();
	
	/**
	 * Load a font from a bitmap
	 */
	KrFontResource* loadFont(const char* file, int glyphs);

	BrainSprite* addSprite();

	BrainSprite* reparentSprite(BrainSprite* bs, KrImNode* parent);
	//! Drop eventual carried sprite and add it back to the bg tree.
	void dropPlayerSprite(BrainPlayer* player, bool remove=false);

	enum sounds {
		CLICK,
		BOUNCE
	};
	
	bool playSample(enum sounds sound) const { return m_sound->playSample(sound); }

	void drawText(const char* text, SDL_Rect pos, int size=16);

	bool changeLevel(int lvl);

	vector<BrainSprite*>& getAllSprites() { return m_sprites; }

	void writeScoreAndTime(time_t& now);

	/**
	 * Perform a select for the player at lvl.
	 */
	void select(Puzzle& lvl, BrainPlayer& player);

	/**
	 * This is the loop that runs continuously during
	 * gameplay to move all sprites and handle key events.
	 */ 
    int eventLoop();

	/**
	 * Return the first colliding sprite with the player
	 * If player is not colliding with a sprite, 0 is returned.
	 */
	BrainSprite* collisionCheck(BrainPlayer* player);

	/**
	 * Enter text input mode
	 * Returns an id of the string to be returned
	 */
	int startTextInput(string label);

private:

    Brainblast(const Brainblast& bb);
    Brainblast& operator=(const Brainblast& bb);

	// Called for every key when we are supposed to write text
	void textInput(SDLKey k);
	// Called when finished with one text
	void nextTextInput();

	// Should be called whenever the game ends
	void gameOver();

	void clearTextBox( KrTextBox* tb );

	void showHighScore();

	/** 
	 * Called when the initial wait where the solution is shown
	 * should end.
	 **/
	void finishInitialWait();

    //! Start a new game
    bool startGame();

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

    //! Used to clean up for all pointers
    void cleanup();
	
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
	
	enum gamestate
	{
		PLAY_WAIT, // Waiting while showing solution
		PLAYING,   // Playing the game
		TITLE,     // Showing the title screen
		GAME_OVER, // 
		HIGH_SCORE 
	};
	
	enum gamestate m_gamestate;

	time_t m_start_time;

	/**
	 * Handles all the sounds (effects and music)
	 */
	BrainSound* m_sound;

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
	KrFontResource* m_score_font;
	KrFontResource* m_title_font;

	KrTextBox*      m_left_score_text_box;
	KrTextBox*      m_right_score_text_box;
	KrTextBox*      m_center_text_box;
	KrTextBox*      m_top_center_text_box;

	BrainPlayerManager* m_player_manager;

	// Used for settings at the title screen
	// might go into a Prefs class later
	int m_human_players;
	int m_computer_players;
	LEVEL_SET m_level_set;

	vector<TextListener*> m_text_listeners;
	map<int,string> m_text_queue;
};

#endif
