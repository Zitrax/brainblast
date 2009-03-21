/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#ifndef BRAINSTATE_H
#define BRAINSTATE_H

using namespace std;

class BrainStateManager;

/**
 * Every state in the game will inherit this
 * class which defines the operations that 
 * must be supprted by a state in the game.
 */
class BrainState
{
public:
	BrainState(BrainStateManager& mgr) : m_mgr(mgr) {}
	virtual ~BrainState(){}

	virtual void init()			= 0;
	virtual void cleanup()		= 0;
	virtual void pause()		= 0;
	virtual void handleEvents() = 0;
	virtual void update()		= 0;
	virtual void draw()			= 0;

	void changeState(BrainState* state);

private:
	BrainStateManager& m_mgr;

};

/**
 * Manages the states and the switches
 */
class BrainStateManager
{
public:
	BrainStateManager() : m_states(),m_running(false) {}
	virtual ~BrainStateManager() {}

	virtual void changeState(BrainState* state) = 0;
	
	virtual void handleEvents() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;

	void pushState(BrainState* state) { m_states.push_back(state); }
	void popState() { m_states.pop_back(); }

	bool running() const { return m_running; }
	void quit() { m_running = false; }

private:
	vector<BrainState*> m_states;
	bool m_running;

};

#endif // BRAINSTATE_H
