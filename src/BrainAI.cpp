/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainAI.h"

#include "Brainblast.h"

#include <queue>
#include <algorithm> // find

using namespace std;

/**
 * Comparison object used in priority queue
 * It measures the distance from the player
 * to the sprite.
 */
class distcmp
{
public:
    distcmp(int x) : m_x(x){}

	/**
	 * Should have been const pointers, but Kyras ->X() function
	 * is not const.
	 */
    bool operator() (BrainSprite*& lhs, BrainSprite*& rhs) const
        {
            return 
                min(abs(lhs->X()-m_x),abs(m_x-lhs->X()+brain::VIDEOX)) >
                min(abs(rhs->X()-m_x),abs(m_x-rhs->X()+brain::VIDEOX));
        }
private:
    int m_x;
};


// Future improvements:
//
//  * The AI should look at what the other player is doing
//    and not run towards a piece that the other player will
//    reach before him anyway. In this case run to the second
//    closest piece instead.
//
//  * Make the ai smart enough to jump after pieces it could
//    reach in the air.
//
//  * If ai has nothing to do, but the other player has it could
//    try to interfere with them, take their needed pieces and run 
//    away with them for example.
//
//  * Stop the ai from "vibrating" when he can't decide direction.
//
//  * Allow the ai to be stupid by randomly doing something incorrect.
// 

BrainAI::BrainAI(KrSpriteResource* res, std::string name, enum Difficulty diff)
    : BrainPlayer(res,name),
      m_needed_ids(),
      m_selection_start(),
      m_selection_delay(0.010), // I need a better timer than clock
                                // perhaps clock_gettime would be ok.
	  m_difficulty(diff)
{
	setDifficulty();
}

float BrainAI::time_since_start()
{
	timespec now;
	clock_gettime(CLOCK_REALTIME,&now);
	timespec diff = time_diff(m_selection_start,now);

	float time = diff.tv_sec;
	time += diff.tv_nsec/1000000000.0;
	return time;
}

timespec BrainAI::time_diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

void BrainAI::move()
{
	BrainSprite::move();

    if( m_level->isSelecting() )
    {
        printf("BrainAI::move %f\n",time_since_start());
		
		if( time_since_start() > m_selection_delay )
		{
            if( m_level->navigateTowards() )
            {
                // Stupid kyra has not made NodeId const so we have to const_cast here
                m_needed_ids.erase(find(m_needed_ids.begin(),
										m_needed_ids.end(),
										const_cast<BrainSprite*>(m_level->getSelectionSprite())->NodeId()));
                Brainblast::instance()->select(*m_level,*this);
                
            }
			clock_gettime(CLOCK_REALTIME,&m_selection_start);
        }
        return;
    }

    if( isCarrying() )
    {
        BrainSprite* o = drop(0);
        o->setStatic(true);
        m_level->startSelection(o);
        clock_gettime(CLOCK_REALTIME,&m_selection_start);
        return;
    }

    // Locate pieces on ground  
    //   For later : even take into consideration 
    //               bricks in the air with jumping.

    vector<BrainSprite*>& sprites = Brainblast::instance()->getAllSprites();

    // Constructing a priority queue with distance comparison
    // The comparison class is constructed with the players x-position
    priority_queue< BrainSprite*, vector<BrainSprite*>, distcmp > pqueue(X());

    std::vector<BrainSprite*>::const_iterator it;
    std::vector<BrainSprite*>::const_iterator end = sprites.end();
    for( it=sprites.begin(); it!=end; ++it)
    {
        if( *it == this )
            continue;

        // Can be faster if we just have an array with counts for needed id types
        if( std::find(m_needed_ids.begin(),
                      m_needed_ids.end(),
                      (*it)->NodeId()) != m_needed_ids.end() )
        {
            pqueue.push(*it);
        }
            
    }

    if( pqueue.empty() )
        return;

    // The candidate is: 
    BrainSprite* candidate = pqueue.top();

    int px = X();
    int py = Y();
    int cx = candidate->X();
    int cy = candidate->Y();

    // If we have reached the brick, pick it up
    if( (abs(px - cx) < 5) && 
        (abs(py - cy) < 15))
    {
        pickUp(Brainblast::instance()->reparentSprite(candidate,this));
        return;
    }

    // Now we have a candidate to run towards

    int left_dist = cx-px<0 ? px-cx : px-cx+brain::VIDEOX;

    (left_dist < brain::VIDEOX/2) ? left() : right();
}

void BrainAI::setDifficulty()
{
	switch(m_difficulty)
	{
	case IDIOT:
		m_selection_delay = 1.20;
		break;
	case STUPID:
		m_selection_delay = 1.00;
		break;
	case EASY:
		m_selection_delay = 0.60;
		break;
	case MEDIUM:
		m_selection_delay = 0.40;
		break;
	case HARD:
		m_selection_delay = 0.25;
		break;
	case IMPOSSIBLE:
		m_selection_delay = 0;
		break;
	case UNKNOWN:
		assert(!"Should not get here");
		m_selection_delay = 0.50;
		break;
	}
}
