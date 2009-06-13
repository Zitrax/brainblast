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
	BrainState(BrainStateManager* mgr);
	virtual ~BrainState(){}

	virtual void init()			= 0;
	virtual void cleanup()		= 0;
	virtual void handleEvent(SDL_Event& event) = 0;
	virtual void update()		= 0;
	virtual void draw()			= 0;

	void changeState(BrainState& state);

protected:

    BrainState(const BrainState& bb);
    BrainState& operator=(const BrainState& bb);

	BrainStateManager* m_mgr;
	Brainblast* m_game;
};

/**
 * Manages the states and the switches
 */
class BrainStateManager
{
public:
	BrainStateManager() : m_states(),m_running(false) {}
	virtual ~BrainStateManager() {}

	// Is this needed, can we go with just pushState instead ?
	virtual void changeState(BrainState& state)
		{
			pushState(state);
		}
	
	virtual void handleEvents() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;

	void pushState(BrainState& state) { m_states.push_back(&state); }
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
	BrainMenu(BrainStateManager* mgr) : BrainState(mgr) {}
	virtual ~BrainMenu(){}

	void init();
	void cleanup();
	void handleEvent(SDL_Event& event);
	void update();
	void draw();

	static BrainMenu& instance() { assert(s_instance); return *s_instance; }

private:

	void titleScreenUpdateText();

	static BrainMenu* s_instance;

};

class BrainPlayWait : public BrainState
{
public:
	BrainPlayWait(BrainStateManager* mgr) : BrainState(mgr) {}
	virtual ~BrainPlayWait(){}

	void init();
	void cleanup();
	void handleEvent(SDL_Event& event);
	void update();
	void draw();

	static BrainPlayWait& instance() { assert(s_instance); return *s_instance; }

private:

	static BrainPlayWait* s_instance;	
};

#endif // BRAINSTATE_H
