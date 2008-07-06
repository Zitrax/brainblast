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
    BrainPlayerManager(int human_players, int compuer_players);
	virtual ~BrainPlayerManager(){}

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

	unsigned int playerCount() const { return m_players.size(); }

private:

    BrainPlayerManager(const BrainPlayerManager&);
    BrainPlayerManager& operator=(const BrainPlayerManager&);

	vector<BrainPlayer*> m_players;
};

#endif //  BRAIN_PLAYER_MANAGER_H
