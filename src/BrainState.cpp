/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#include "BrainState.h"
#include "Brainblast.h"
#include "BrainText.h"
#include "BrainPlayerManager.h"
#include <sstream>   // ostringstream

BrainStateManager* BrainState::s_mgr = 0;

Brainblast* BrainState::game() const
{ 
	assert(s_mgr); 
	return dynamic_cast<Brainblast*>(s_mgr); 
}

void BrainState::changeState(BrainState& state)
{
    s_mgr->changeState(state);
}

void BrainStateManager::pushState(BrainState& state)
{ 
	if(bbc::debug) cerr << "BrainStateManager::pushState(" << state.name() << ")" << endl;
	currentState().cleanup();
	m_states.push_back(&state); 
	state.init(); 
}

void BrainMenu::init()
{
	game()->loadMusic("music/Acidstorm.it");
	game()->playMusic();

	game()->stopPlay();

	game()->text().write(BrainText::TOP_LEFT, "MUSIC BY SAGA MUSIX, HTTP://SAGAMUSIX.DE/",0);
	game()->text().write(BrainText::TOP_RIGHT,"              CODE: DANIEL BENGTSSON",0);

	titleScreenUpdateText();
}

void BrainMenu::cleanup()
{
	game()->text().clear(BrainText::CENTER);
}

bool BrainMenu::handleEvent(SDL_Event& event)
{
	switch( event.type )
	{
	case SDL_KEYDOWN:

		switch(event.key.keysym.sym)
		{
			
		case SDLK_SPACE:
		case SDLK_RETURN:
			changeState(BrainPlayWait::instance());
			return true;

		case SDLK_F1:
			game()->playSample(Brainblast::CLICK);
			game()->addHumanPlayer();
			titleScreenUpdateText();
			return true;

		case SDLK_F2:
			game()->playSample(Brainblast::CLICK);
			game()->addComputerPlayer();
			titleScreenUpdateText();
			return true;
			
		case SDLK_F3:
			game()->playSample(Brainblast::CLICK);
			switch( game()->getLevelSet() )
			{
			case NORMAL:
				game()->setLevelSet(RANDOM); break;
			case RANDOM:
				game()->setLevelSet(NORMAL); break;
			default:
				game()->setLevelSet(NORMAL); break;
			}
			titleScreenUpdateText();
			return true;
			
		case SDLK_F4:
			game()->playerManager().toggleDifficulty();
			titleScreenUpdateText();
			return true;

		case SDLK_F5:
			assert(!"Not implemented");
			//showHighScore();
			return true;

		default:
			// Just skip other keys
			return true;
		}
	}
	
	return false;
}

void BrainMenu::update()
{
}

void BrainMenu::draw()
{
}

void BrainMenu::titleScreenUpdateText()
{
	game()->text().clear(BrainText::CENTER);
	game()->text().clear(BrainText::HIGH_SCORE); // Should not be needed, that mode should clear it

	game()->text().write(BrainText::CENTER,"BRAINBLAST 0.2",0);
	game()->text().write(BrainText::CENTER,"",1);

	ostringstream str;

	str << "F1: Human Players - " << game()->getHumanPlayers();
	game()->text().write(BrainText::CENTER,str.str(),2);

	str.str(""); 
	str << "F2: Computer Players - " << game()->getComputerPlayers();
	game()->text().write(BrainText::CENTER,str.str(),3);

	str.str("");
	string set = levelSetToString(game()->getLevelSet());
	str << "F3: Level set - " << set;
	game()->text().write(BrainText::CENTER,str.str(),4);

	str.str("");
	string difficulty = game()->playerManager().difficultyString();
	str << "F4: Difficulty - " << difficulty;
	game()->text().write(BrainText::CENTER,str.str(),5);

	game()->text().write(BrainText::CENTER,"F5: Highscores",6);
	game()->text().write(BrainText::CENTER,"",7);
	game()->text().write(BrainText::CENTER,"SPACE: Start game",8);
	game()->text().write(BrainText::CENTER,"",9);
	
	game()->text().write(BrainText::TOP_CENTER,"",0);
}

void BrainPlayWait::init()
{
	game()->playerManager().resetScores();

	game()->loadMusic("music/enigmatic_path.it");
	game()->playMusic();
	
	game()->addPlayers();

	time(&m_start_time);
}

void BrainPlayWait::cleanup()
{
}

bool BrainPlayWait::handleEvent(SDL_Event& event)
{
	game()->writeScoreAndTime(secondsLeft());

	switch( event.type )
	{
	case SDL_KEYDOWN:
		if( event.key.keysym.sym == SDLK_SPACE || 
			event.key.keysym.sym == SDLK_RETURN )
		{
			changeState(BrainPlaying::instance());
			return true;
		}
	}

	if( difftime(time(0),m_start_time) > m_wait_time )
		changeState(BrainPlaying::instance());

	return false;
}

void BrainPlayWait::update()
{
}

void BrainPlayWait::draw()
{
}

unsigned int BrainPlayWait::secondsLeft() const
{
	time_t now = time(0);
	int sec = static_cast<int>(m_wait_time - difftime(now,m_start_time));
	assert(sec <= m_wait_time);
	return sec;
}

void BrainPlaying::init()
{
	game()->hideSolutions();
	game()->text().write(BrainText::CENTER,"",0);
	time(&m_start_time);

	m_add_sprite_event.type = SDL_ADD_SPRITE_EVENT;
	m_add_sprite_timer = SDL_AddTimer( 2000, TimerCallback, &m_add_sprite_event );	
}

void BrainPlaying::cleanup()
{
	SDL_RemoveTimer(m_add_sprite_timer);
}

bool BrainPlaying::handleEvent(SDL_Event& event)
{
	game()->writeScoreAndTime(secondsLeft());

	switch( event.type )
	{
	case SDL_ADD_SPRITE_EVENT:
		game()->addSprite();
		break;
	}

	return false;
}

unsigned int BrainPlaying::secondsLeft() const
{
	time_t now = time(0);
	int sec = static_cast<int>(m_play_time - difftime(now,m_start_time));
	assert(sec <= m_play_time);
	bool game_over = sec <= 0;
	return game_over ? 0 : sec;
}
