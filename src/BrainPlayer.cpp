/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainPlayer.h"

BrainPlayer::BrainPlayer(KrSpriteResource* res, std::string name) :
    BrainSprite(res,"Player",false),
    m_score(0),
    m_name(name)
{
}

BrainPlayer::~BrainPlayer()
{
}

void BrainPlayer::left()
{
	if( isCarrying() )
		SetAction("HOLDING.LEFT");
	else
		SetAction("WALKING.LEFT");
	DoStep();
}

void BrainPlayer::right()
{
	if( isCarrying() )
		SetAction("HOLDING.RIGHT");
	else
		SetAction("WALKING.RIGHT");
	DoStep();
}
