/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAIN_PLAYER_MANAGER_H
#define BRAIN_PLAYER_MANAGER_H

#include <vector>
#include "BrainPlayer.h"
#include "Brainblast.h"
#include "HighScore.h"
#include "BrainAI.h"

using namespace std;

/**
 * This class handles the players and should make sure 
 * the players act according to their actions. It handles
 * both human players and computer players.
 */
class BrainPlayerManager : public TextListener
{
public:
    BrainPlayerManager();
	virtual ~BrainPlayerManager();

	bool addPlayers(int human_players, int compuer_players);
	void removePlayers();

	/**
	 * Move all players
	 */
	void move();

	/**
	 * Call this when the game is over
	 * to reset scores and submit highscores
	 */
	void gameOver();

	/**
	 * All players get 0 points
	 */
	void resetScores();

	/**
	 * Returns true if no player has received any points.
	 */
	bool allScoresNull() const;

	/** 
	 * Returns true if the key was handled/eaten and should
	 * not be processed further by caller 
	 */
	bool handleKeyDown(SDLKey key);
	bool handleKeyHeld(const bool* const keys_held);

	BrainPlayer* getPlayer(unsigned int idx) const;
	int getPlayerNumber(BrainPlayer& player) const;

	unsigned int playerCount() const { return m_player_count; }

	vector<HighScore::Entry> getHighScoreEntries() const { return m_highscore->getEntries(); }
	unsigned int hs_max_entries() const { return m_highscore->max_entries(); }

	// TextListener
	void textReady(string str, int id);

	enum BrainAI::Difficulty difficulty() const { return m_difficulty; }
	void setDifficulty(enum BrainAI::Difficulty diff) { m_difficulty = diff; }
	string difficultyString() const;
	void toggleDifficulty();

private:

    BrainPlayerManager(const BrainPlayerManager&);
    BrainPlayerManager& operator=(const BrainPlayerManager&);

	// <Used for for_each>
	// TODO: Should really use references instead
	static void playerResetScore(BrainPlayer* player) { player->setScore(0); player->resetLevelCount(); }
	static void playerMove(BrainPlayer* player) { player->move(); }
	static void playerDeleteNode(BrainPlayer* player) { Brainblast::instance()->engine()->Tree()->DeleteNode(player); }

	struct playerCheckScore
	{
		friend class BrainPlayerManager;
		playerCheckScore(BrainPlayerManager& manager) : m_manager(manager) {}
		void operator() (BrainPlayer* player);
		BrainPlayerManager& m_manager;
	};
	// </Used for for_each>

	/** Space between starting positions */
	int spacing() const;

	vector<BrainPlayer*> m_players;
	unsigned int m_player_count;
	enum BrainAI::Difficulty m_difficulty;
	HighScore* m_highscore;

	map<int,HighScore::Entry> m_high_scores;
};

#endif //  BRAIN_PLAYER_MANAGER_H
