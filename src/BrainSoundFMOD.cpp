/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainSoundFMOD.h"

#include "../fmod/api/inc/fmod_errors.h"
#include <stdio.h>

#ifndef TRUE
  #define TRUE 1
#endif
#ifndef FALSE
  #define FALSE 0
#endif

BrainSoundFMOD::~BrainSoundFMOD()
{
	if( m_initialized )
    {
        FMUSIC_FreeSong(m_music);
        FSOUND_Close();
    }
}

void BrainSoundFMOD::error(const char* text) const
{
	printf("ERROR/FMOD - %s (%s)\n",text,FMOD_ErrorString(FSOUND_GetError()));
}

bool BrainSoundFMOD::initializeSound()
{
	if (!FSOUND_Init(44100,200,0))
	{
		error("Can't initialize device");
		FSOUND_Close();
		return false;
	}

	m_initialized = true;
}

bool BrainSoundFMOD::loadMusic(const char* file)
{
    if( m_music )
    {
        FMUSIC_FreeSong(m_music);
        m_music = 0;
    }

    m_music = FMUSIC_LoadSong(file);

    if( !m_music )
	{
		error("loadMusic - Can't load the file");
		return false;
	}

	return true;
}

bool BrainSoundFMOD::playMusic()
{
    if( !m_music )
    {
        printf("ERROR/FMOD - Play:No music loaded\n");
        return false;
    }

	if( !FMUSIC_PlaySong(m_music) )
	{
		error("Could not play");
		return false;
	}
	return true;
}

bool BrainSoundFMOD::stopMusic()
{
    if( !m_music )
    {
        printf("ERROR/FMOD - Stop:No music loaded\n");
        return false;
    }

	if( !FMUSIC_StopSong(m_music) )
	{
		error("Could not stop");
		return false;
	}
	return true;
}

bool BrainSoundFMOD::pauseMusic()
{
    if( !m_music )
    {
        printf("ERROR/FMOD - Pause:No music loaded\n");
        return false;
    }

	if( !FMUSIC_SetPaused(m_music,TRUE) )
	{
		error("Could not pause");
		return false;
	}
	return true;	
}

bool BrainSoundFMOD::resumeMusic()
{
    if( !m_music )
    {
        printf("ERROR/FMOD - Resume:No music loaded\n");
        return false;
    }

	if( !FMUSIC_SetPaused(m_music,FALSE) )
	{
		error("Could not resume");
		return false;
	}
	return true;	
}

bool BrainSoundFMOD::toggleMusic()
{
    if( !m_music )
    {
        printf("ERROR/FMOD - Toggle:No music loaded\n");
        return false;
    }
    
    if( FMUSIC_GetPaused(m_music) )
        return resumeMusic();
    else
        return pauseMusic();

    return false;
}

bool BrainSoundFMOD::addSample(const char* file, int id)
{
    if( !FSOUND_Sample_Load( id, file, 0, 0, 0 ) )
    {
        error("Could not load the file\n");
        return false;
    }

    return true;
}

bool BrainSoundFMOD::playSample(int id) const
{
    if( !FSOUND_PlaySound(FSOUND_FREE, FSOUND_Sample_Get(id)) )
    {
        error("Could not play sound\n");
        return false;
    }

    return true;
}
