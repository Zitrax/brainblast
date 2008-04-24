/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainAI.h"

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
                // Stupid kyra has not made NodeId const so we have to const_case here
                m_needed_ids.erase(std::find(m_needed_ids.begin(),
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

    std::vector<BrainSprite*>& sprites = Brainblast::instance()->getAllSprites();

    // TODO: Sort by distance
    
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
            break;
    }

    if( it == end )
    {
        m_player->stop();
        return;
    }

    // If we have reached a brick, pick it up
    if( (abs(m_player->X() - (*it)->X()) < 5) && 
        (abs(m_player->Y() - (*it)->Y()) < 15))
    {
        m_player->pickUp(Brainblast::instance()->reparentSprite(*it,m_player));
        return;
    }

    // Now we have a candidate to run towards
    // printf("%i > %i\n",m_player->X(),(*it)->X());
    if( m_player->X() > (*it)->X() )
        m_player->left();
    else
        m_player->right();
}
