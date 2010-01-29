/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#ifndef BRAINBLAST_H
#define BRAINBLAST_H

#include <stdlib.h>
#include <fstream>

#include <wSDL.h>
#include <wkyra.h>

#include "bbc.h"
#include "Puzzle.h"
#include "Brick.h"
#include "BrainSprite.h"
#include "BrainSound.h"
#include "BrainState.h"
#include "BrainText.h"

class BrainAI;
class BrainPlayerManager;
class BrainPlayer;

using namespace std;
using namespace bbc;

namespace brain
{

//! The total number of different brick types (different symbols)
	const unsigned int MAX_NOF_BRICK_TYPES = 25;
	
//! The video mode to use
	const int VIDEOX = 1024;
	const int VIDEOY = 768;
	const int VIDEOBITS = 32;
	
//! Margin to be used in board/field layout
	const int MARGIN = 0;
}

/**
 * Starting a Brainblast game:
 * 1. Call initGameKyra()
 * 2. Start the event loop: eventLoop()
 */
class Brainblast : public BrainStateManager
{

private:

	/**
	 * Handles data for level creation
	 */
	class LevelData
	{
	public:
		LevelData(unsigned int max_width,
				  unsigned int max_height);
		
		unsigned int size() const { return m_width*m_height; }
		unsigned int w() const { return m_width; }
		unsigned int h() const { return m_height; }
		unsigned int n() const { return m_bricks; }
		unsigned int types() const { return m_types; }

		void setMaxTypes(unsigned int max_types) { m_max_types = max_types; }

		bool increaseDifficulty();
		void reset();

	private:

		friend ostream& operator<<(ostream& out, const LevelData& lvl)
		{
			return 
				out << "(" << lvl.m_width << "," << lvl.m_height << 
				":" << lvl.m_bricks << ":" << lvl.m_types << ")";
		}

		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_bricks;
		unsigned int m_types;

		unsigned int m_max_types;
		unsigned int m_max_width;
		unsigned int m_max_height;
	};

#ifdef _DEBUG
	friend class BrainSprite;
#endif // _DEBUG

public:
    
    Brainblast(string base_dir);
    ~Brainblast();

	static Brainblast* instance() { return s_instance; }

	KrEngine* engine() const { return m_engine; }
	
    //! Check if a solution is correct
    bool checkSolution(Puzzle* puzzle);

    bool initGameKyra();

	void stopPlay();
	
	BrainSprite* addSprite();

	BrainSprite* reparentSprite(BrainSprite* bs, KrImNode* parent);
	//! Drop eventual carried sprite and add it back to the bg tree.
	void dropPlayerSprite(BrainPlayer* player, bool remove=false);

	enum sounds {
		CLICK,
		BOUNCE,
		WARNING
	};
	
	// Make sound interface reachable from the states
	bool playSample(enum sounds sound) const { return m_sound->playSample(sound); }

	/**
	 * @param file Relative directory to the music file from the base directory
	 */
	bool loadMusic(const char* file);

	bool playMusic() { return m_sound->playMusic(); }

	BrainText& text() { return m_text; }

	bool changeLevel(int lvl);

	vector<BrainSprite*>& getAllSprites() { return m_sprites; }

	void writeScoreAndTime(int sec);

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
	 * Will add a human player and roll around if there is one too many
	 */
	void addHumanPlayer();

	/**
	 * Will add a computer player and roll around if there is one too many
	 */
	void addComputerPlayer();

	/**
	 * Prepend a string with the base directory
	 * This allocates a new string that must be freed
	 */
	const char* addBaseDir(const char* const str);

	// < FIXME: Should move into state code >

	void allowNavigation();
	void forbidNavigation();
	void hideSolutions();
    bool addPlayers();
	BrainPlayerManager& playerManager() const { return *m_player_manager; }
	int getHumanPlayers() const { return m_human_players; }
	int getComputerPlayers() const { return m_computer_players; }
	LEVEL_SET getLevelSet() const { return m_level_set;	}
	void setLevelSet(LEVEL_SET s) { m_level_set = s; }
	int getCurrentLevel() const { return m_current_lvl; }

	/**
	 * Removes all sprites on the playfield
	 * except the players and the ones belonging
	 * to the puzzles.
	 */
	void clearFloor();

	// < / FIXME: Should move into state code >

	//// <BrainStateManager> ////

	void handleEvents();
	void update(){}
	void draw(){}

	//// </BrainStateManager> ////


private:

    Brainblast(const Brainblast& bb);
    Brainblast& operator=(const Brainblast& bb);

	bool setupFields(int players);

	void deleteLevels();

    //! Returns true if created succesfully, false otherwise
    bool makeLevel(int lvl);
	/**
	 * Creates random levels with specified
	 * width and height, and n number of bricks.
	 */
	void makeRandomLevel(LevelData& lvl);

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
	static const double WAITTIME;
	
	static void allowLevelNavigation(Puzzle* lvl) { lvl->allowNavigation(true); }
	static void forbidLevelNavigation(Puzzle* lvl) { lvl->allowNavigation(false); }

	/**
	 * Handles all the sounds (effects and music)
	 */
	BrainSound* m_sound;

	LevelData m_level_data;

	vector<Puzzle*> m_current_levels;
	vector<SDL_Rect> m_fields;
	int m_current_lvl;

    SDL_Surface* m_screen;

	map<int,Brick*> m_bricks;

	KrEngine*    m_engine;

	KrImNode*    m_bgTree;
	KrImNode*    m_fgTree;

	vector<BrainSprite*> m_sprites;

	KrResourceVault* m_bg_vault;
	KrSprite*        m_bg_sprite;

	BrainText m_text;

	BrainPlayerManager* m_player_manager;

	// Used for settings at the title screen
	// might go into a Prefs class later
	int m_human_players;
	int m_computer_players;
	LEVEL_SET m_level_set;

	string m_base_dir;
};

/**
 * Will free the string when it goes out of scope
 */
class AutoCStr
{
public:
	AutoCStr(const char* s) : m_s(s) {}
	~AutoCStr() { free(const_cast<char*>(m_s)); }
	operator const char* () { return m_s; }
private:
	AutoCStr(const AutoCStr&);
	AutoCStr& operator=(const AutoCStr&);

	const char* m_s;
};

// FIXME: Move to some better place
#define SDL_DRAW_EVENT ( SDL_USEREVENT + 0 )
#define SDL_ADD_SPRITE_EVENT ( SDL_USEREVENT + 1 )
#define SDL_ENTER_TIME_BONUS ( SDL_USEREVENT + 2 )
#define SDL_TIME_BONUS_EVENT ( SDL_USEREVENT + 3 )

#endif // BRAINBLAST_H
