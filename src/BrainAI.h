/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAIN_AI_H
#define BRAIN_AI_H

#include "BrainPlayer.h"
#include "Brainblast.h"
#include "Puzzle.h"

class BrainAI : public BrainPlayer
{
	enum Difficulty
	{
		IDIOT,
		STUPID,
		EASY,
		MEDIUM,
		HARD,
		IMPOSSIBLE
	};

public:
    BrainAI(KrSpriteResource* res, std::string name, enum Difficulty diff = EASY);
	
    virtual ~BrainAI(){}
	
	virtual void move();
	
	virtual void setLevel(Puzzle* lvl) 
		{ 
			m_level = lvl; 
			m_needed_ids = lvl->getSolutionTypes();
		}

private:

	BrainAI(const BrainAI& bb);
    BrainAI& operator=(const BrainAI& bb);

	void setDifficulty();

	std::vector<int> m_needed_ids;

  	clock_t m_selection_start;
  	float m_selection_delay;
	enum Difficulty m_difficulty;

};

#endif // BRAIN_AI_H
