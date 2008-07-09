/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainSoundBASS.h"

#include "../bassmod/bassmod.h"

BrainSoundBASS::~BrainSoundBASS()
{
	if( m_initialized )
		BASSMOD_Free();
}

void BrainSoundBASS::error(const char* text)
{
	printf("ERROR/BASSMOD - %s (%u)\n",text,BASSMOD_ErrorGetCode());
}

bool BrainSoundBASS::initializeSound()
{
	/* check that BASSMOD 2.0 was loaded */
	if( BASSMOD_GetVersion() != MAKELONG(2,0) ) {
		error("version 2.0 was not loaded");
		return false;
	}

	if (!BASSMOD_Init(-1,44100,0))
	{
		error("Can't initialize device");
		BASSMOD_Free();
		return false;
	}

	m_initialized = true;
}

bool BrainSoundBASS::loadMusic(const char* file)
{
	/* try loading the MOD (with looping, sensitive ramping, surround sound and calculate the duration) */
	if (!BASSMOD_MusicLoad(FALSE,(void*)file,0,0,
						   BASS_MUSIC_LOOP|BASS_MUSIC_RAMPS|BASS_MUSIC_SURROUND|BASS_MUSIC_CALCLEN))
	{
		error("loadMusic - Can't play the file");
		return false;
	}

	return true;
}

bool BrainSoundBASS::playMusic()
{
	if( !BASSMOD_MusicPlay() )
	{
		error("Could not play");
		return false;
	}
	return true;
}

bool BrainSoundBASS::stopMusic()
{
	if( !BASSMOD_MusicStop() )
	{
		error("Could not stop");
		return false;
	}
	return true;
}

bool BrainSoundBASS::pauseMusic()
{
	if( !BASSMOD_MusicPause() )
	{
		error("Could not pause");
		return false;
	}
	return true;	
}

bool BrainSoundBASS::resumeMusic()
{
	return playMusic();
}

bool BrainSoundBASS::toggleMusic()
{
	switch( BASSMOD_MusicIsActive() )
	{
	case BASS_ACTIVE_STOPPED:
	case BASS_ACTIVE_PAUSED:
		return playMusic();
		break;
	case BASS_ACTIVE_PLAYING:
		return stopMusic();
	default:
		error("toggleMusic unhandled value");
	}
}

