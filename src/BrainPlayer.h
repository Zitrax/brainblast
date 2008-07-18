/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAINPLAYER_H
#define BRAINPLAYER_H

#include "Brainblast.h"
#include "BrainSprite.h"
#include "Puzzle.h"

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
	virtual void setLevel(Puzzle* lvl)    { m_level = lvl; m_level_count++; }
	int getLevelCount() const             { return m_level_count; }
	void resetLevelCount()                { m_level_count = 0; }
	LEVEL_SET getLevelSet() const         { return m_level_set; }
	void setLevelSet(LEVEL_SET level_set) { m_level_set = level_set; }

    virtual void left();
    virtual void right();

	KrTextBox* getScoreBox() const { return m_score_box; }
	void setScoreBox(KrTextBox* box)  { m_score_box = box; }

	/**
	 * Return the action that this player maps 
	 * to a certain key.
	 */
	enum PlayerAction action(SDLKey key) const 
		{
			keymap::const_iterator it = m_actions.find(key);
			return it==m_actions.end() ? NONE : it->second;
		}

	keymap::const_iterator keyMapIterator() const 
		{
			return m_actions.begin();
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

	KrTextBox* m_score_box;

	LEVEL_SET m_level_set;
};

#endif
