/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

void BrainState::changeState(BrainState* state)
{
    m_mgr.changeState(state);
}

void BrainMenu::init()
{
	m_game.loadMusic("/usr/share/games/brainblast/music/Acidstorm.it");
	m_game.playMusic();

	m_game.stopPlay();

	m_game.text.write(TOP_LEFT, "MUSIC BY SAGA MUSIX, HTTP://SAGAMUSIX.DE/",0);
	m_game.text.write(TOP_RIGHT,"              CODE: DANIEL BENGTSSON",0);

	titleScreenUpdateText();
}

void BrainMenu::cleanup()
{
}

void BrainMenu::handleEvents()
{
}

void BrainMenu::update()
{
}

void BrainMenu::draw()
{
}

void BrainMenu::titleScreenUpdateText()
{
	m_game.text.clear(CENTER);
	m_game.text.clear(HIGH_SCORE); // Should not be needed, that mode should clear it

	m_game.text.write(CENTER,"BRAINBLAST 0.2",0);
	m_game.text.write(CENTER,"",1);

	ostringstream str;

	str << "F1: Human Players - " << m_human_players;
	m_game.text.write(CENTER,str.str(),2);

	str.str(""); 
	str << "F2: Computer Players - " << m_computer_players;
	m_game.text.write(CENTER,str.str(),3);

	str.str("");
	string set = levelSetToString(m_level_set);
	str << "F3: Level set - " << set;
	m_game.text.write(CENTER,str.str(),4);

	str.str("");
	string difficulty = m_player_manager->difficultyString();
	str << "F4: Difficulty - " << difficulty;
	m_game.text.write(CENTER,str.str(),5);

	m_game.text.write(CENTER,"F5: Highscores",6);
	m_game.text.write(CENTER,"",7);
	m_game.text.write(CENTER,"SPACE: Start game",8);
	m_game.text.write(CENTER,"",9);
	
	m_game.text.write(TOP_CENTER,"",0);
}
