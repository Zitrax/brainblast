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

BrainMenu* BrainMenu::s_instance = 0;
BrainPlayWait* BrainPlayWait::s_instance = 0;

BrainState::BrainState(BrainStateManager* mgr)
	: m_mgr(mgr), m_game(dynamic_cast<Brainblast*>(mgr)) {}

void BrainState::changeState(BrainState& state)
{
    m_mgr->changeState(state);
}

void BrainMenu::init()
{
	m_game->loadMusic("/usr/share/games/brainblast/music/Acidstorm.it");
	m_game->playMusic();

	m_game->stopPlay();

	m_game->text().write(BrainText::TOP_LEFT, "MUSIC BY SAGA MUSIX, HTTP://SAGAMUSIX.DE/",0);
	m_game->text().write(BrainText::TOP_RIGHT,"              CODE: DANIEL BENGTSSON",0);

	titleScreenUpdateText();
}

void BrainMenu::cleanup()
{
	m_game->text().clear(BrainText::CENTER);
}

void BrainMenu::handleEvent(SDL_Event& event)
{
	switch( event.type )
	{
	case SDL_KEYDOWN:
		if( event.key.keysym.sym == SDLK_SPACE || 
			event.key.keysym.sym == SDLK_RETURN )
		{
			changeState(BrainPlayWait::instance());
		}
	}
}

void BrainMenu::update()
{
}

void BrainMenu::draw()
{
}

void BrainMenu::titleScreenUpdateText()
{
	m_game->text().clear(BrainText::CENTER);
	m_game->text().clear(BrainText::HIGH_SCORE); // Should not be needed, that mode should clear it

	m_game->text().write(BrainText::CENTER,"BRAINBLAST 0.2",0);
	m_game->text().write(BrainText::CENTER,"",1);

	ostringstream str;

	str << "F1: Human Players - " << m_game->getHumanPlayers();
	m_game->text().write(BrainText::CENTER,str.str(),2);

	str.str(""); 
	str << "F2: Computer Players - " << m_game->getComputerPlayers();
	m_game->text().write(BrainText::CENTER,str.str(),3);

	str.str("");
	string set = levelSetToString(m_game->getLevelSet());
	str << "F3: Level set - " << set;
	m_game->text().write(BrainText::CENTER,str.str(),4);

	str.str("");
	string difficulty = m_game->playerManager().difficultyString();
	str << "F4: Difficulty - " << difficulty;
	m_game->text().write(BrainText::CENTER,str.str(),5);

	m_game->text().write(BrainText::CENTER,"F5: Highscores",6);
	m_game->text().write(BrainText::CENTER,"",7);
	m_game->text().write(BrainText::CENTER,"SPACE: Start game",8);
	m_game->text().write(BrainText::CENTER,"",9);
	
	m_game->text().write(BrainText::TOP_CENTER,"",0);
}

void BrainPlayWait::init()
{
	m_game->playerManager().resetScores();

	m_game->loadMusic("/usr/share/games/brainblast/music/enigmatic_path.it");
	m_game->playMusic();
	
	m_game->addPlayers();
}

void BrainPlayWait::cleanup()
{
}

void BrainPlayWait::handleEvent(SDL_Event& event)
{
	switch( event.type )
	{
	case SDL_KEYDOWN:
		if( event.key.keysym.sym == SDLK_SPACE || 
			event.key.keysym.sym == SDLK_RETURN )
		{
			m_game->finishInitialWait();
		}
	}
}

void BrainPlayWait::update()
{
}

void BrainPlayWait::draw()
{
}
