/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#ifndef BRAINPLAYER_H
#define BRAINPLAYER_H

#include "BrainSprite.h"
#include "BrainText.h"
#include "Puzzle.h"

#include <algorithm>
#include <ext/functional>

using namespace std;
using namespace bbc;

class BrainPlayer : public BrainSprite
{
	
public:

	enum PlayerAction
	{
		NONE,
		WALK_LEFT,
		WALK_RIGHT,
		JUMP,
		PICKUP,
		DROP,
		SELECT
	};

	typedef std::map<SDLKey,enum PlayerAction> keymap;

	BrainPlayer(KrSpriteResource* res, std::string name);  
	virtual ~BrainPlayer();

	int getScore() const      { return m_score; }
	int addScore(int score)   { m_score+=score; if(m_score<0) m_score=0; return m_score; }
	void setScore(int score)  { m_score = score; }

	string getName() const    { return m_name; }
	void setName(string name) { m_name = name; }

	virtual Puzzle* getLevel() const      { return m_level;}
	virtual void setLevel(Puzzle* lvl)    { m_level = lvl; if(lvl) m_level_count++; }
	int getLevelCount() const             { return m_level_count; }
	void resetLevelCount()                { m_level_count = 0; }
	LEVEL_SET getLevelSet() const         { return m_level_set; }
	void setLevelSet(LEVEL_SET level_set) { m_level_set = level_set; }

    virtual void left();
    virtual void right();

	BrainText::Box getScoreBox() const { return m_score_box; }
	void setScoreBox(BrainText::Box box)  { m_score_box = box; }

	virtual bool isComputerPlayer() const { return false; }

	/**
	 * Return the action that this player maps 
	 * to a certain key.
	 */
	enum PlayerAction action(SDLKey key) const 
		{
			keymap::const_iterator it = m_actions.find(key);
			return it==m_actions.end() ? NONE : it->second;
		}

	SDLKey keyForAction(enum PlayerAction action) const
		{
			// I think this would be more readable with a standard
			// iteration loop but for the purpose of using/learning 
			// stl I'll use and explain this. This uses SGI extensions 
			// so if this causes trouble it can easily be replaced 
			// with a small loop.
			//
			// Even a written functor (Unary predicate) would probably 
			// be more readable, sorry about this :)
			// 
			// compose1 : Composes two Adaptable Unary Functions
			//            with the same return type. For example
			//            turns the functions f(x) and g(x) to f(g(x))
			//
			// bind2nd  : This function constructs an unary function object 
			//            from the binary function object op by binding its 
			//            second parameter to the fixed value x.
			//
			// select2nd: Just makes sure we use the .second (the value)
			//            in the map.
			//
			keymap::const_iterator it = 
				find_if( m_actions.begin(), m_actions.end(),
						 compose1( bind2nd(equal_to<keymap::mapped_type>(), action),
								   __gnu_cxx::select2nd<keymap::value_type>()));
			assert( it != m_actions.end() );
			return it->first;
		}

	void mapAction( enum PlayerAction action, SDLKey key); 

	const std::vector<SDLKey>& keys() const { return m_keys; }

protected:

	Puzzle* m_level;

private:
    BrainPlayer(const BrainPlayer&);
    BrainPlayer& operator=(const BrainPlayer&);

	int m_score;
	string m_name;
	int m_level_count;

	keymap m_actions;
	std::vector<SDLKey> m_keys;

	BrainText::Box m_score_box;

	LEVEL_SET m_level_set;
};

#endif
