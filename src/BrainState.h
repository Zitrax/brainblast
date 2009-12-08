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

	virtual void    init()								= 0;
	virtual void    cleanup()							= 0;
	virtual void    handleEvent(SDL_Event& event)		= 0;
	virtual void    update()							= 0;
	virtual void    draw()								= 0;
	virtual string  name() const						= 0;

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
 * Manages the states and the switches
 */
class BrainStateManager
{
public:
	BrainStateManager() : m_states(),m_running(true) {}
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
	BrainState& currentState() const { assert(m_states.back()); return *m_states.back(); }

	bool running() const { return m_running; }
	void quit() { m_running = false; }

private:
	vector<BrainState*> m_states;
	bool m_running;

};

class BrainMenu : public BrainState
{
public:
	virtual ~BrainMenu(){}

	void init();
	void cleanup();
	void handleEvent(SDL_Event& event);
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

class BrainPlayWait : public BrainState
{
public:
	virtual ~BrainPlayWait() {}

	void init();
	void cleanup();
	void handleEvent(SDL_Event& event);
	void update();
	void draw();
	string name() const { return "BrainPlayWait"; }

	static BrainPlayWait& instance() 
		{
			static BrainPlayWait instance;
			return instance;
		}

private:

	time_t m_start_time;
	const double m_wait_time;

	BrainPlayWait() : BrainState(),
					  m_start_time(0),
					  m_wait_time(10.0)
		{assert(s_mgr);}

	unsigned int secondsLeft() const;
};

class BrainPlaying : public BrainState
{
public:
	virtual ~BrainPlaying() {}

	void init();
	void cleanup(){}
	void handleEvent(SDL_Event& event);
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

	BrainPlaying() : m_start_time(0),
					 m_play_time(60.0) 
		{assert(s_mgr);}

	unsigned int secondsLeft() const;
};

#endif // BRAINSTATE_H
