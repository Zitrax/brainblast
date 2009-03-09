/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAIN_AI_H
#define BRAIN_AI_H

#include "BrainPlayer.h"
#include "Puzzle.h"

class BrainAI : public BrainPlayer
{
public:

	enum Difficulty
	{
		IDIOT,
		STUPID,
		EASY,
		MEDIUM,
		HARD,
		IMPOSSIBLE,
		UNKNOWN
	};

    BrainAI(KrSpriteResource* res, std::string name, enum Difficulty diff);
	
    virtual ~BrainAI(){}
	
	virtual void move();
	
	virtual void setLevel(Puzzle* lvl) 
		{ 
			m_level = lvl; 
			if(lvl)
				m_needed_ids = lvl->getSolutionTypes();
		}

private:

	BrainAI(const BrainAI& bb);
    BrainAI& operator=(const BrainAI& bb);

	void setDifficulty();

	std::vector<int> m_needed_ids;

	float time_since_start();
	timespec time_diff(timespec start, timespec end);

  	timespec m_selection_start;
	/** Time in seconds to wait between moving each brick */
  	float m_selection_delay;
	enum Difficulty m_difficulty;

};

#endif // BRAIN_AI_H
