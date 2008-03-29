/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAINSOUND_H
#define BRAINSOUND_H

#include "SDL.h"
#include "SDL_mixer.h"

class BrainSound
{
public:

	BrainSound() : m_music(0) {};
	~BrainSound();

	bool initializeSound();
	bool loadMusic(const char* file);
	bool playMusic();
	void stopMusic();
	void pauseMusic();
	void resumeMusic();
	void toggleMusic();

private:

    BrainSound(const BrainSound& bb);
    BrainSound& operator=(const BrainSound& bb);

	Mix_Music* m_music;

};

#endif
