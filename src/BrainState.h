/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#ifndef BRAINSTATE_H
#define BRAINSTATE_H

#include <wSDL.h>
#include <vector>
#include <assert.h>
#include <string>

class Brainblast;
class BrainPlayer;

using namespace std;

class BrainStateManager;

/**
 * Every state in the game will inherit this
 * class which defines the operations that 
 * must be supported by a state in the game.
 */
class BrainState
{
public:
	BrainState(){};
	virtual ~BrainState(){}

	/**
	 * First function to run when entering the state
	 */
	virtual void init()	= 0;

	/**
	 * Last function to run when leaving the state
	 */
	virtual void cleanup() = 0;

	/**
	 * Handle events arriving during the lifetime of the state
	 */
	virtual bool handleEvent(SDL_Event& event) = 0;

	/**
	 * The name of this state
	 */
	virtual string name() const = 0;

	// Currently not used
	virtual void update() = 0;
	virtual void draw() = 0;

	void changeState(BrainState& state);
	
	/**
	 * This must be set before using any of the states.
	 */
	static void setStateManager(BrainStateManager& mgr) { s_mgr = &mgr; } 
	Brainblast* game() const;

protected:

    BrainState(const BrainState& bb);
    BrainState& operator=(const BrainState& bb);

	static BrainStateManager* s_mgr;
};

/**
 * The states vector will be initialized with this such that it's never empty
 */
class NullState : public BrainState
{
	void    init(){}
	void    cleanup(){}
	bool    handleEvent(SDL_Event&){assert(false); return false;}
	void    update(){assert(false);}
	void    draw(){assert(false);}
	string  name() const { return "NullState"; }
};

/**
 * Manages the states and the switches
 */
class BrainStateManager
{
public:
	BrainStateManager() : m_states(),m_running(true),m_null_state() {m_states.push_back(&m_null_state);}
	virtual ~BrainStateManager() {}

	// Is this needed, can we go with just pushState instead ?
	virtual void changeState(BrainState& state)
		{
			pushState(state);
		}
	
	virtual void handleEvents() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;

	void pushState(BrainState& state);
	void popState() { m_states.pop_back(); }
	BrainState& currentState() const { assert(m_states.size()); return *m_states.back(); }

	bool running() const { return m_running; }
	void quit() { m_running = false; }

private:
	vector<BrainState*> m_states;
	bool m_running;
	NullState m_null_state;

};

class BrainMenu : public BrainState
{
public:
	virtual ~BrainMenu(){}

	void init();
	void cleanup();
	bool handleEvent(SDL_Event& event);
	void update();
	void draw();
	string name() const { return "BrainMenu"; }

	static BrainMenu& instance() 
		{
			static BrainMenu instance;
			return instance;
		}

private:

	BrainMenu(){assert(s_mgr);};

	void titleScreenUpdateText();

};

/**
 * Abstract base class for all states that shows the play field, thus
 * not the main menu, highscores and such...
 */
class BrainPlayArea : public BrainState
{
public:
	virtual ~BrainPlayArea() {}

	// Just to make it abstract
	virtual void init() = 0;

	/**
	 * Update the score boxes with the current scores
	 */
	void writeScoreAndTime(int sec);
	
};

class BrainPlayWait : public BrainPlayArea
{
public:
	virtual ~BrainPlayWait() {}

	void init();
	void cleanup();
	bool handleEvent(SDL_Event& event);
	void update();
	void draw();
	string name() const { return "BrainPlayWait"; }

	static BrainPlayWait& instance(bool first_level)
		{
			static BrainPlayWait instance;
			instance.m_first_level = first_level;
			return instance;
		}

private:

	time_t m_start_time;
	const double m_wait_time;
	bool m_first_level;

	BrainPlayWait() : BrainPlayArea(),
					  m_start_time(0),
					  m_wait_time(10.0),
					  m_first_level(true)
		{assert(s_mgr);}

	unsigned int secondsLeft() const;
};

class BrainPlaying : public BrainPlayArea
{
public:
	virtual ~BrainPlaying() {}

	void init();
	void cleanup();
	bool handleEvent(SDL_Event& event);
	void update(){}
	void draw(){}
	string name() const { return "BrainPlaying"; }

	static BrainPlaying& instance() 
		{
			static BrainPlaying instance;
			return instance;
		}

private:

	time_t m_start_time;
	const double m_play_time;
	SDL_Event m_add_sprite_event; 
	SDL_TimerID m_add_sprite_timer;

	BrainPlaying() : m_start_time(0),
					 m_play_time(60.0),
					 m_add_sprite_event(),
					 m_add_sprite_timer()
		{assert(s_mgr);}

	unsigned int secondsLeft() const;

	BrainPlaying(const BrainPlaying&);
	BrainPlaying& operator=(const BrainPlaying&);
};

class BrainTimeBonus : public BrainPlayArea
{
public:
	virtual ~BrainTimeBonus() {}
	
	void init();
	void cleanup(){}
	bool handleEvent(SDL_Event& event);
	void update(){}
	void draw(){}
	string name() const { return "BrainTimeBonus"; }

	static BrainTimeBonus& instance(
		BrainPlayer* player,
		int secondsLeft) 
		{
			static BrainTimeBonus instance;
			instance.setPlayer(player);
			instance.setSecondsLeft(secondsLeft);
			return instance;
		}

private:

	BrainTimeBonus() : m_time_bonus_timer(), 
					   m_time_bonus_event(),
					   m_player(0),
					   m_seconds_left(0)
		{assert(s_mgr);}

	BrainTimeBonus(const BrainTimeBonus&);
	BrainTimeBonus& operator=(const BrainTimeBonus&);

	void setPlayer(BrainPlayer* player) { m_player = player; }
	void setSecondsLeft(int s) { m_seconds_left = s; }

	// Speeds up the timebonus timer so we count as
	// fast as we can.
	void speedyTimeBonus();

	SDL_TimerID m_time_bonus_timer;
	SDL_Event m_time_bonus_event;
	BrainPlayer* m_player;
	int m_seconds_left;
};

#include "BrainText.h"
#include "TextListener.h"

class BrainGameOver : public BrainState
{
public:
	virtual ~BrainGameOver() {}
	
	void init();
	void cleanup(){}
	bool handleEvent(SDL_Event& event);
	void update(){}
	void draw(){}
	string name() const { return "BrainGameOver"; }

	/**
	 * Enter text input mode
	 * Returns an id of the string to be returned
	 */
	int startTextInput(string label);

	static BrainGameOver& instance()
		{
			static BrainGameOver instance;
			return instance;
		}

private:

	BrainGameOver();
		
	// Called for every key when we are supposed to write text
	void textInput(SDLKey k);
	// Called when finished with one text
	void nextTextInput();
	
	vector<TextListener*> m_text_listeners;
	map<int,string> m_text_queue;

};

class BrainHighScore : public BrainState
{
public:
	virtual ~BrainHighScore() {}
	
	void init();
	void cleanup(){}
	bool handleEvent(SDL_Event& event);
	void update(){}
	void draw(){}
	string name() const { return "BrainHighScore"; }

	static BrainHighScore& instance()
		{
			static BrainHighScore instance;
			return instance;
		}

private:

	BrainHighScore() {assert(s_mgr);}
};


#endif // BRAINSTATE_H
