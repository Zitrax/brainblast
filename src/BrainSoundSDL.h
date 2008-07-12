/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAINSOUNDSDL_H
#define BRAINSOUNDSDL_H

#include "BrainSound.h"
#include "SDL.h"
#include "SDL_mixer.h"

#include <map>

/**
 * Implements the BrainSound api using SDL_mixer
 */
class BrainSoundSDL : public BrainSound
{
public:

	BrainSoundSDL() : BrainSound(), m_music(0), m_samples() {}
	~BrainSoundSDL();

	bool initializeSound();
	bool isInitialized() const { return m_initialized == true; }

	// Music functions
	bool loadMusic(const char* file);
	bool playMusic();
	bool stopMusic();
	bool pauseMusic();
	bool resumeMusic();
	bool toggleMusic();

	// Sound effects
	bool addSample(const char* file,int id);
	bool playSample(int id) const;

private:

    BrainSoundSDL(const BrainSoundSDL& bb);
    BrainSoundSDL& operator=(const BrainSoundSDL& bb);

	Mix_Music* m_music;
	std::map<int,Mix_Chunk*> m_samples;

};

#endif
