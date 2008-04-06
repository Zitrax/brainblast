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

	// TODO: Make sure all sounds are freed correctly
	if( m_music )
		Mix_FreeMusic(m_music);
	
	std::map<int,Mix_Chunk*>::const_iterator it;
	std::map<int,Mix_Chunk*>::const_iterator end = m_samples.end();
	for(it=m_samples.begin();it!=end;++it)
		Mix_FreeChunk(it->second);
}

bool BrainSound::initializeSound()
{
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
	int audio_channels = 2;

	// Note that high values on this
	// seems to increase latency quite alot.
	int audio_buffers = 1024; 
	
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
		printf("Unable to open audio!\n");
		m_initialized = false;
		return false;
	}

	m_initialized = true;
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

bool BrainSound::addSample(const char* file,int id)
{
	if( m_samples.find(id) != m_samples.end() )
	{
		printf("BrainSound::addSample id=%i is already in use.\n",id);
		return false;
	}

	Mix_Chunk* sound = Mix_LoadWAV(file);
	if( !sound )
	{
		printf("BrainSound::addSample Could not load sample '%s' (%s).\n",file,Mix_GetError());
		return false;
	}

	m_samples.insert( std::pair<int,Mix_Chunk*>(id,sound) );
	return true;
}

bool BrainSound::playSample(int id) const
{
	std::map<int,Mix_Chunk*>::const_iterator it = m_samples.find(id);
	if( it == m_samples.end() )
	{
		printf("BrainSound::playSample id=%i do not exist.\n",id);
		return false;
	}
	
	if( Mix_PlayChannel(-1,it->second,0) == -1 )
	{
		printf("BrainSound::playSample Could not play sample (%s)\n",Mix_GetError());
		return false;
	}
	return true;
}
