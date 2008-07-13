/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainPlayerManager.h"
#include "Brainblast.h"
#include "../images/bb.h"

using namespace brain;

BrainPlayerManager::BrainPlayerManager()
	: m_players(), m_player_count(0)
{
}

void BrainPlayerManager::addPlayers(int human_players, int computer_players)
{
	// FIXME: Should handle updating number of players
	//        and thus remove the old ones here.
	assert( m_players.size() == 0 );

	m_player_count = human_players + computer_players;

    KrSpriteResource* wizardRes = 
        Brainblast::instance()->engine()->Vault()->GetSpriteResource( BB_WIZARD );

    for(int i=0;i<human_players;++i)
    {
		if(bbc::debug) cerr << "BrainPlayerManager() Adding human player " << i << "\n";
        BrainPlayer* player = new BrainPlayer( wizardRes, "wizard" );
        player->SetNodeId(BB_WIZARD);
        player->SetPos( spacing(), 0);
        Brainblast::instance()->engine()->Tree()->AddNode( 0, player );
        m_players.push_back(player);
    }

    for(int i=0;i<computer_players;++i)
    {
		if(bbc::debug) cerr << "BrainPlayerManager() Adding computer player " << i << "\n";
        BrainPlayer* player = new BrainAI( wizardRes, "computer" );
        player->SetNodeId(BB_WIZARD);
        player->SetPos( spacing(), 0);
        Brainblast::instance()->engine()->Tree()->AddNode( 0, player );
        m_players.push_back(player);
    }
	
	if( (m_players.size() > 0) && m_players[0] )
	{
		m_players[0]->mapAction(BrainPlayer::WALK_LEFT, SDLK_LEFT);
		m_players[0]->mapAction(BrainPlayer::WALK_RIGHT,SDLK_RIGHT);
		m_players[0]->mapAction(BrainPlayer::JUMP,      SDLK_RCTRL);
		m_players[0]->mapAction(BrainPlayer::PICKUP,    SDLK_UP);
		m_players[0]->mapAction(BrainPlayer::DROP,      SDLK_DOWN);
		m_players[0]->mapAction(BrainPlayer::SELECT,    SDLK_RETURN);
	}

	if( (m_players.size() > 1) && m_players[1] )
	{
		m_players[1]->mapAction(BrainPlayer::WALK_LEFT, SDLK_a);
		m_players[1]->mapAction(BrainPlayer::WALK_RIGHT,SDLK_d);
		m_players[1]->mapAction(BrainPlayer::JUMP,      SDLK_LCTRL);
		m_players[1]->mapAction(BrainPlayer::PICKUP,    SDLK_w);
		m_players[1]->mapAction(BrainPlayer::DROP,      SDLK_s);
		m_players[1]->mapAction(BrainPlayer::SELECT,    SDLK_LSHIFT);
	}
    
}

void BrainPlayerManager::removePlayers()
{
	vector<BrainPlayer*>::iterator it;
    vector<BrainPlayer*>::iterator end = m_players.end();
    for( it=m_players.begin(); it!=end; ++it )
		Brainblast::instance()->engine()->Tree()->DeleteNode(*it);
	m_players.clear();

	m_player_count = 0;
}

int BrainPlayerManager::spacing() const
{
	return m_player_count>1 ? 30 + m_players.size()*(VIDEOX-60)/(m_player_count-1) : VIDEOX/2;
}

void BrainPlayerManager::move()
{
    vector<BrainPlayer*>::iterator it;
    vector<BrainPlayer*>::iterator end = m_players.end();
    for( it=m_players.begin(); it!=end; ++it )
        (*it)->move();
}

BrainPlayer* BrainPlayerManager::getPlayer(int idx) const
{
	return m_players[idx];
}

bool BrainPlayerManager::handleKeyDown(SDLKey key)
{

    vector<BrainPlayer*>::iterator it;
    vector<BrainPlayer*>::iterator end = m_players.end();
    for( it=m_players.begin(); it!=end; ++it )
	{
		BrainPlayer* player = *it;
		Puzzle* level = player->getLevel();

		switch( player->action(key) )
		{

		case BrainPlayer::NONE:
			break;

		case BrainPlayer::WALK_LEFT:
			if( level->isSelecting() ) 
				level->navigate(Puzzle::LEFT);
			break;

		case BrainPlayer::WALK_RIGHT:
			if( level->isSelecting() )
				level->navigate(Puzzle::RIGHT);
			break;

		case BrainPlayer::JUMP:
		case BrainPlayer::PICKUP:
 			if( level->isSelecting() )
			{
				level->navigate(Puzzle::UP);
				return true;
			}
			else if( player->isCarrying() )
			{
				BrainSprite* o = player->drop(0);
				o->setStatic(true);
				level->startSelection(o);
				return true;
			}
			break;
				
		case BrainPlayer::DROP:
			if( level->isSelecting() )
				level->navigate(Puzzle::DOWN);
			break;

		case BrainPlayer::SELECT:
			if( level->isSelecting() )
				Brainblast::instance()->select(*level,*player);
			break;

		}
	}

	return false;
}

bool BrainPlayerManager::handleKeyHeld(const bool* const keys_held)
{
	
	vector<BrainPlayer*>::iterator it;
    vector<BrainPlayer*>::iterator end = m_players.end();
    for( it=m_players.begin(); it!=end; ++it )
	{
		BrainPlayer* player = *it;
		Puzzle* level = player->getLevel();

		std::vector<SDLKey> keys = player->keys();

		std::vector<SDLKey>::const_iterator it = keys.begin();

		while( it != keys.end() )
		{

			if( keys_held[*it] )
			{
				
				switch( player->action(*it) )
				{
					
				case BrainPlayer::NONE:
				case BrainPlayer::SELECT:
					break;

				case BrainPlayer::PICKUP:
					if( !player->isCarrying() )
					{
						BrainSprite* c = Brainblast::instance()->collisionCheck(player);
						if( c ) 
							player->pickUp( Brainblast::instance()->reparentSprite(c,player));
					}
					break;

				case BrainPlayer::WALK_LEFT:
					if( !level->isSelecting() )
						player->left();
					break;

				case BrainPlayer::WALK_RIGHT:
					if( !level->isSelecting() )
						player->right();
					break;

				case BrainPlayer::JUMP:
					player->jump();
					break;
			
				case BrainPlayer::DROP:
					Brainblast::instance()->dropPlayerSprite(player);
					break;
					
				}
			}

			++it;
		}
	}

	return false;
}
