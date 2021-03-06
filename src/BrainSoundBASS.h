/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#ifndef BRAINSOUNDBASS_H
#define BRAINSOUNDBASS_H

#include "BrainSound.h"

#include <assert.h>

/**
 * Implements the BrainSound api using the BASSMOD library
 */
class BrainSoundBASS : public BrainSound
{
public:

	BrainSoundBASS() : BrainSound() {}
	~BrainSoundBASS();

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
	bool addSample(const char*,int) { assert(false); return false; }
	bool playSample(int) const { assert(false); return false; }

	void error(const char* text);

};

#endif // BRAINSOUNDBASS_H

