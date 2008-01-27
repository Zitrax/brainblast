/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainSound.h"

#include "bbc.h"

BrainSound::~BrainSound()
{
	Mix_CloseAudio();	
}

bool BrainSound::initializeSound()
{
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
	int audio_channels = 2;
	int audio_buffers = 4096;
	
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
		printf("Unable to open audio!\n");
		return false;
	}

	return true;
}

bool BrainSound::loadMusic(const char* file)
{
	// Free eventual old music
	if( m_music )
	{
		Mix_FreeMusic(m_music);
		m_music = 0;
	}

	m_music = Mix_LoadMUS(file);
	if(!m_music) {
		printf("Mix_LoadMUS(\"%s\"): %s\n", file, Mix_GetError());
		return false;
	}
	return true;
}

bool BrainSound::playMusic()
{
	if(bbc::debug) std::cerr << "Starting Music\n";
	if( Mix_PlayMusic(m_music, -1)==-1 ) {
		printf("Mix_PlayMusic: %s\n", Mix_GetError());
		return false;
	}
	return true;
}

void BrainSound::stopMusic()
{
	if(bbc::debug) std::cerr << "Stopping Music\n";
	Mix_HaltMusic();
}

void BrainSound::pauseMusic()
{
	if(bbc::debug) std::cerr << "Pausing Music\n";
	Mix_PauseMusic();
}

void BrainSound::resumeMusic()
{
	if(bbc::debug) std::cerr << "Resume Music\n";
	Mix_ResumeMusic();
}

void BrainSound::toggleMusic()
{
	if( !m_music ) 
	{
		printf("Can't toggle, no music loaded...\n");
		return;
	}

	if(!Mix_PausedMusic()) 
		pauseMusic();
	else
		resumeMusic();
}
