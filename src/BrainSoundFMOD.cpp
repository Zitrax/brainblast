/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#include "BrainSoundFMOD.h"

#include "../fmod/api/inc/fmod_errors.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

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
    return true;
}

bool BrainSoundFMOD::loadMusic(const char* file)
{
	if( m_music && (m_loaded_music == file) )
		return true; // We have already loaded this file

    if( m_music )
    {
        FMUSIC_FreeSong(m_music);
        m_music = 0;
    }

    m_music = FMUSIC_LoadSong(file);

    if( !m_music )
	{
		error("loadMusic - Can't load the file");
		m_loaded_music = "";
		return false;
	}

	m_loaded_music = file;

	return true;
}

bool BrainSoundFMOD::playMusic()
{
    if( !m_music )
    {
        printf("ERROR/FMOD - Play:No music loaded\n");
        return false;
    }

    if( m_off_toggle )
        return true; // Not an error

    if( FMUSIC_IsPlaying(m_music) )
		return true; // Do not restart if already playing

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

    if( !FMUSIC_IsPlaying(m_music) )
    {
        m_off_toggle = false;
        return playMusic();
    }
    else if( FMUSIC_GetPaused(m_music) )
    {
        m_off_toggle = false;
        return resumeMusic();
    }
    else
    {
        m_off_toggle = true;
        return pauseMusic();
    }

    return false;
}

bool BrainSoundFMOD::addSample(const char* file, int id)
{
    if( !FSOUND_Sample_Load( id, file, 0, 0, 0 ) )
    {
		char* msg = (char*) malloc(strlen(file)+50);
		sprintf(msg,"Could not load the file (%s)\n",file);
        error(msg);
		free(msg);
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
