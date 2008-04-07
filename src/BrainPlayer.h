/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAINPLAYER_H
#define BRAINPLAYER_H

#include "BrainSprite.h"

using namespace std;

class BrainPlayer : public BrainSprite
{
public:
	BrainPlayer(KrSpriteResource* res, std::string name);  
	virtual ~BrainPlayer();

	int getScore() const      { return m_score; }
	int addScore(int score)   { m_score+=score; if(m_score<0) m_score=0; return m_score; }
	void setScore(int score)  { m_score = score; }

	string getName() const    { return m_name; }
	void setName(string name) { m_name = name; }

private:
	int m_score;
	string m_name;

};

#endif
