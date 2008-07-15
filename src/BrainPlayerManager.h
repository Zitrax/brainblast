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

using namespace std;

/**
 * This class handles the players and should make sure 
 * the players act according to their actions. It handles
 * both human players and computer players.
 */
class BrainPlayerManager
{
public:
    BrainPlayerManager();
	virtual ~BrainPlayerManager();

	void addPlayers(int human_players, int compuer_players);
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
	 * Returns true if the key was handled/eaten and should
	 * not be processed further by caller 
	 */
	bool handleKeyDown(SDLKey key);
	bool handleKeyHeld(const bool* const keys_held);

	BrainPlayer* getPlayer(int idx) const;
	int getPlayerNumber(BrainPlayer& player) const;

	unsigned int playerCount() const { return m_player_count; }

private:

    BrainPlayerManager(const BrainPlayerManager&);
    BrainPlayerManager& operator=(const BrainPlayerManager&);

	// <Used for for_each>
	// TODO: Should really use references instead
	static void playerResetScore(BrainPlayer* player) { player->setScore(0); }
	static void playerMove(BrainPlayer* player) { player->move(); }
	static void playerDeleteNode(BrainPlayer* player) { Brainblast::instance()->engine()->Tree()->DeleteNode(player); }

	struct playerCheckScore
	{
		playerCheckScore(HighScore& hs) : m_hs(hs) {}
		void operator() (BrainPlayer* player);
		HighScore& m_hs;
	};
	// </Used for for_each>

	/** Space between starting positions */
	int spacing() const;

	vector<BrainPlayer*> m_players;
	unsigned int m_player_count;
	HighScore* m_highscore;
};

#endif //  BRAIN_PLAYER_MANAGER_H
