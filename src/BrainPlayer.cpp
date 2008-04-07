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
