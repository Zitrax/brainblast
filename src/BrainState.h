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
	virtual void handleEvents() = 0;
	virtual void update()		= 0;
	virtual void draw()			= 0;

	void changeState(BrainState* state);

protected:
	BrainStateManager& m_mgr;
};

/**
 * Adds reference to the game
 */
class BrainBlastState : public BrainState
{
public:
	BrainBlastState(BrainStateManager& mgr, Brainblast& game) : BrainState(mgr), m_game(game) {}
	virtual ~BrainBlastState(){}
protected:
	Brainblast& m_game;
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

class BrainMenu : public BrainBlastState
{
public:
	BrainMenu(BrainStateManager& mgr, Brainblast& game) : BrainBlastState(mgr,game) {}
	virtual ~BrainMenu(){}

	void init();
	void cleanup();
	void handleEvents();
	void update();
	void draw();

	void changeState(BrainState*){}

	static BrainMenu& instance() { return s_instance; }

private:

	void titleScreenUpdateText();

	static BrainMenu s_instance;

};

#endif // BRAINSTATE_H
