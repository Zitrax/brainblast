/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAIN_PLAYER_MANAGER_H
#define BRAIN_PLAYER_MANAGER_H

#include <vector>
#include "BrainPlayer.h"

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
	virtual ~BrainPlayerManager(){}

	void addPlayers(int human_players, int compuer_players);
	void removePlayers();

	/**
	 * Move all players
	 */
	void move();

	/** 
	 * Returns true if the key was handled/eaten and should
	 * not be processed further by caller 
	 */
	bool handleKeyDown(SDLKey key);
	bool handleKeyHeld(const bool* const keys_held);

	BrainPlayer* getPlayer(int idx) const;

	unsigned int playerCount() const { return m_player_count; }

private:

    BrainPlayerManager(const BrainPlayerManager&);
    BrainPlayerManager& operator=(const BrainPlayerManager&);

	/** Space between starting positions */
	int spacing() const;

	vector<BrainPlayer*> m_players;
	unsigned int m_player_count;
};

#endif //  BRAIN_PLAYER_MANAGER_H
