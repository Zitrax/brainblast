/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2013
 * @author Daniel Bengtsson
 */

#include "BrainSoundFMODEX.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <iostream>

#include "../fmod/fmodex/api/inc/fmod_errors.h"

BrainSoundFMODEX::~BrainSoundFMODEX()
{
	SoundMap::iterator it;
	for(it=m_sounds.begin(); it!=m_sounds.end(); ++it)
		checkResult(it->second->release());
	m_sounds.clear();
	checkResult(m_system->close());
	checkResult(m_system->release());
	m_system = NULL;
}

void BrainSoundFMODEX::error(const char* /*text*/) const
{
	assert(!"Not implemented");
}

bool BrainSoundFMODEX::checkResult(const FMOD_RESULT& result) const
{
	if (result != FMOD_OK)
	{
		printf("ERROR/FMOD - (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}
	return true;
}

bool BrainSoundFMODEX::initializeSound()
{
	if( checkResult(FMOD::System_Create(&m_system)) && 
		// For some reason the autodect output type
		// has quite a long delay before the sound plays.
		// Forcing pulseaudio worked - but might not be optimal.
		checkResult(m_system->setOutput(FMOD_OUTPUTTYPE_PULSEAUDIO)) &&
		checkResult(m_system->init(32, FMOD_INIT_NORMAL, 0))
	) {
		m_initialized = true;
	}
	return m_initialized;
}

bool BrainSoundFMODEX::loadMusic(const char* file)
{
	if(addSample(file, m_music_id))
	{
		return checkResult(m_sounds[m_music_id]->setMode(FMOD_LOOP_NORMAL));
	}
	return false;
}

bool BrainSoundFMODEX::playMusic()
{
	return playSample(m_music_id);
}

bool BrainSoundFMODEX::stopMusic()
{
	return false;
}

bool BrainSoundFMODEX::pauseMusic()
{
	return false;
}

bool BrainSoundFMODEX::resumeMusic()
{
	return false;
}

bool BrainSoundFMODEX::toggleMusic()
{
	return false;
}

bool BrainSoundFMODEX::addSample(const char* file, int id)
{
	if(m_sounds.find(id) == m_sounds.end())
	{
		FMOD::Sound* sound = NULL;
		if(checkResult(m_system->createSound(file, FMOD_SOFTWARE, 0, &sound)))
		{
			m_sounds[id] = sound;
			return true;
		}
	}
	return false;
}

bool BrainSoundFMODEX::playSample(int id) const
{
	SoundMap::const_iterator it = m_sounds.find(id);
	if(it != m_sounds.end())
	{
		FMOD::Channel* channel;
		return checkResult(m_system->playSound(FMOD_CHANNEL_FREE, it->second, 0, &channel));
	}
	return false;
}
