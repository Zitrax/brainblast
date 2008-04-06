/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAINSOUND_H
#define BRAINSOUND_H

#include "SDL.h"
#include "SDL_mixer.h"

#include <map>

class BrainSound
{
public:

	BrainSound() : m_initialized(false), m_music(0), m_samples() {};
	~BrainSound();

	bool initializeSound();
	bool isInitialized() const { return m_initialized == true; }

	// Music functions
	bool loadMusic(const char* file);
	bool playMusic();
	void stopMusic();
	void pauseMusic();
	void resumeMusic();
	void toggleMusic();

	// Sound effects
	bool addSample(const char* file,int id);
	bool playSample(int id) const;

private:

    BrainSound(const BrainSound& bb);
    BrainSound& operator=(const BrainSound& bb);

	bool m_initialized;
	Mix_Music* m_music;
	std::map<int,Mix_Chunk*> m_samples;

};

#endif
