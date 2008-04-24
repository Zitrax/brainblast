/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAIN_AI_H
#define BRAIN_AI_H

#include "Brainblast.h"
#include "Puzzle.h"

class BrainAI
{
public:
    BrainAI(BrainPlayer* player, Puzzle* lvl);
	
    virtual ~BrainAI(){}
	
	void move();

	void setLevel(Puzzle* lvl) 
		{ 
			m_lvl = lvl; 
			m_needed_ids = lvl->getSolutionTypes();
		}

private:

	BrainAI(const BrainAI& bb);
    BrainAI& operator=(const BrainAI& bb);

    BrainPlayer* m_player;
	Puzzle* m_lvl;

	std::vector<int> m_needed_ids;

  	clock_t m_selection_start;
  	float m_selection_delay;
};


#endif // BRAIN_AI_H
