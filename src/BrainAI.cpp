/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainAI.h"

#include <queue>

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

    bool operator() (BrainSprite*& lhs, BrainSprite*& rhs) const
        {
            return 
                min(abs(lhs->X()-m_x),abs(m_x-lhs->X()+brain::VIDEOX)) >
                min(abs(rhs->X()-m_x),abs(m_x-rhs->X()+brain::VIDEOX));
        }
private:
    int m_x;
};


BrainAI::BrainAI(BrainPlayer* player, Puzzle* lvl)
    : m_player(player),
      m_lvl(lvl),
      m_needed_ids(lvl->getSolutionTypes()),
      m_selection_start(),
      m_selection_delay(0.005) // I need a better timer than clock
                               // perhaps clock_gettime would be ok.
{
}

void BrainAI::move()
{

    if( m_lvl->isSelecting() )
    {
        printf("%f\n",static_cast<float>(clock()-m_selection_start)/CLOCKS_PER_SEC);
//        if( (static_cast<float>(clock()-m_selection_start)/CLOCKS_PER_SEC) > m_selection_delay )
        {
            if( m_lvl->navigateTowards() )
            {
                // Stupid kyra has not made NodeId const so we have to const_cast here
                m_needed_ids.erase(find(m_needed_ids.begin(),
										m_needed_ids.end(),
										const_cast<BrainSprite*>(m_lvl->getSelectionSprite())->NodeId()));
                Brainblast::instance()->select(*m_lvl,*m_player);
                
            }
            m_selection_start = clock();
        }
        return;
    }

    if( m_player->isCarrying() )
    {
        BrainSprite* o = m_player->drop(0);
        o->setStatic(true);
        m_lvl->startSelection(o);
        m_selection_start = clock();
        return;
    }

    // Locate pieces on ground  
    //   For later : even take into consideration 
    //               bricks in the air with jumping.

    vector<BrainSprite*>& sprites = Brainblast::instance()->getAllSprites();

    // Constructing a priority queue with distance comparison
    // The comparison class is constructed with the players x-position
    priority_queue< BrainSprite*, vector<BrainSprite*>, distcmp > pqueue(m_player->X());

    std::vector<BrainSprite*>::const_iterator it;
    std::vector<BrainSprite*>::const_iterator end = sprites.end();
    for( it=sprites.begin(); it!=end; ++it)
    {
        if( *it == m_player )
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

    int px = m_player->X();
    int py = m_player->Y();
    int cx = candidate->X();
    int cy = candidate->Y();

    // If we have reached the brick, pick it up
    if( (abs(px - cx) < 5) && 
        (abs(py - cy) < 15))
    {
        m_player->pickUp(Brainblast::instance()->reparentSprite(candidate,m_player));
        return;
    }

    // Now we have a candidate to run towards

    int left = cx-px<0 ? px-cx : px-cx+brain::VIDEOX;

    if( left < brain::VIDEOX/2 )
    {
        m_player->left();
    }
    else
    {
        m_player->right();
    }
}
