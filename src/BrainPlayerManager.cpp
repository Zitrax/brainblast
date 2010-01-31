/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#include "BrainPlayerManager.h"
#include "../images/bb.h"
#include <sstream>
#include <algorithm>

using namespace brain;

BrainPlayerManager::BrainPlayerManager()
	: m_players(), 
	  m_player_count(0),
	  m_difficulty(BrainAI::MEDIUM),
	  m_highscore(0),
	  m_high_scores()
{
	char* home = getenv("HOME");

	ostringstream ss;
	if( home )
		ss << home << "/.brainblast_score";
	else
		ss << "brainblast_score";
	
	m_highscore = new HighScore(ss.str(),15);
}

BrainPlayerManager::~BrainPlayerManager()
{
	delete m_highscore;
}

bool BrainPlayerManager::addPlayers(int human_players, int computer_players)
{
	// FIXME: Should handle updating number of players
	//        and thus remove the old ones here.
	assert( m_players.size() == 0 );

	m_player_count = human_players + computer_players;

    KrSpriteResource* wizardRes = 
        Brainblast::instance()->engine()->Vault()->GetSpriteResource( BB_WIZARD );
    KrSpriteResource* wizardRes2 = 
        Brainblast::instance()->engine()->Vault()->GetSpriteResource( BB_WIZARD2 );

	if( !(wizardRes && wizardRes2) )
		return false;

    for(int i=0;i<human_players;++i)
    {
		if(bbc::debug) cerr << "BrainPlayerManager() Adding human player " << i << "\n";
        BrainPlayer* player = new BrainPlayer( m_players.size()%2 ? wizardRes2 : wizardRes, "wizard" );
        player->SetNodeId(m_players.size()%2 ? BB_WIZARD2 : BB_WIZARD);
        player->SetPos( spacing(), 0);
        Brainblast::instance()->engine()->Tree()->AddNode( 0, player );
        m_players.push_back(player);
    }

    for(int i=0;i<computer_players;++i)
    {
		if(bbc::debug) cerr << "BrainPlayerManager() Adding computer player " << i << "\n";
        BrainPlayer* player = new BrainAI( m_players.size()%2 ? wizardRes2 : wizardRes, "computer", m_difficulty );
        player->SetNodeId(m_players.size()%2 ? BB_WIZARD2 : BB_WIZARD);
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
		m_players[0]->mapAction(BrainPlayer::SELECT,    SDLK_RSHIFT);
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
    
	return true;
}

void BrainPlayerManager::removePlayers()
{
	for_each(m_players.begin(),m_players.end(),playerDeleteNode);
	m_players.clear();
	m_player_count = 0;
}

int BrainPlayerManager::spacing() const
{
	return m_player_count>1 ? 30 + m_players.size()*(VIDEOX-60)/(m_player_count-1) : VIDEOX/2;
}

void BrainPlayerManager::move()
{
	for_each(m_players.begin(),m_players.end(),playerMove);
}

void BrainPlayerManager::gameOver()
{
	for_each(m_players.begin(),m_players.end(),playerCheckScore(*this));
}

void BrainPlayerManager::resetScores()
{
	for_each(m_players.begin(),m_players.end(),playerResetScore);
}

bool BrainPlayerManager::allScoresNull() const
{
    vector<BrainPlayer*>::const_iterator it;
    vector<BrainPlayer*>::const_iterator end = m_players.end();
    for( it=m_players.begin(); it!=end; ++it )
		if( (*it)->getScore() )
			return false;
	return true;
}

void BrainPlayerManager::playerCheckScore::operator() (BrainPlayer* player)
{
	// Avoid scoring computer players
	if( dynamic_cast<BrainAI*>(player) )
		return;

	if( m_manager.m_highscore->highEnough(player->getScore()) )
	{
		ostringstream ss;
		ss << "Player " << m_manager.getPlayerNumber(*player) << " enter name: ";

		// We should be in gameover state now
		assert( &Brainblast::instance()->currentState() == 
				&BrainGameOver::instance() );
		
		if( &Brainblast::instance()->currentState() == 
			&BrainGameOver::instance() )
		{
			int id = BrainGameOver::instance().startTextInput(ss.str());
			HighScore::Entry hs;
			hs.score = player->getScore();
			hs.level = player->getLevelCount();
			hs.level_set = player->getLevelSet();
			m_manager.m_high_scores[id] = hs;
		}
	}
}

BrainPlayer* BrainPlayerManager::getPlayer(unsigned int idx) const
{
	assert(m_players.size() > idx);
	return m_players[idx];
}

int BrainPlayerManager::getPlayerNumber(BrainPlayer& player) const
{
    vector<BrainPlayer*>::const_iterator it = find(m_players.begin(),m_players.end(),&player);
	if( it != m_players.end() )
		return it - m_players.begin() + 1;
	return -1;
}

void BrainPlayerManager::textReady(string str, int id)
{
	// Submit highscore
	map<int,HighScore::Entry>::iterator it = m_high_scores.find(id);
	if( it == m_high_scores.end() )
		return;

	HighScore::Entry hs = m_high_scores[id];
	m_highscore->addEntry(str,hs.score,hs.level,hs.level_set);
	m_high_scores.erase(id);
}

string BrainPlayerManager::difficultyString() const
{
	switch(m_difficulty)
	{
	case BrainAI::IDIOT:
		return "Idiot";
	case BrainAI::STUPID:
		return "Stupid";
	case BrainAI::EASY:
		return "Easy";
	case BrainAI::MEDIUM:
		return "Medium";
	case BrainAI::HARD:
		return "Hard";
	case BrainAI::IMPOSSIBLE:
		return "Impossible";
	default:
		return "UNKNOWN";
	}

}

void BrainPlayerManager::toggleDifficulty()
{
	m_difficulty = static_cast<BrainAI::Difficulty>(m_difficulty+1);
	if( m_difficulty == BrainAI::UNKNOWN )
		m_difficulty = BrainAI::IDIOT;
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
			if( level && level->isSelecting() ) 
				level->navigate(Puzzle::LEFT);
			break;

		case BrainPlayer::WALK_RIGHT:
			if( level && level->isSelecting() )
				level->navigate(Puzzle::RIGHT);
			break;

		case BrainPlayer::JUMP:
		case BrainPlayer::PICKUP:
			if( level )
			{
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
			}
			break;
				
		case BrainPlayer::DROP:
			if( level && level->isSelecting() )
				level->navigate(Puzzle::DOWN);
			break;

		case BrainPlayer::SELECT:
			if( level )
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

		std::vector<SDLKey>::const_iterator kit = keys.begin();

		while( kit != keys.end() )
		{

			if( keys_held[*kit] )
			{
				
				switch( player->action(*kit) )
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
					if( !level || !level->isSelecting() )
						player->left();
					break;

				case BrainPlayer::WALK_RIGHT:
					if( !level || !level->isSelecting() )
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

			++kit;
		}
	}

	return false;
}
