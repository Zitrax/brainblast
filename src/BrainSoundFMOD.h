/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#ifndef BRAINSOUNDFMOD_H
#define BRAINSOUNDFMOD_H

#include "BrainSound.h"
#include "../fmod/api/inc/fmod.h"

#include <assert.h>
#include <string>

/**
 * Implements the BrainSound api using the FMOD library
 */
class BrainSoundFMOD : public BrainSound
{
public:

	BrainSoundFMOD() : 
		BrainSound(), 
		m_music(0), 
		m_off_toggle(false),
		m_loaded_music() {}
	~BrainSoundFMOD();

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
	bool addSample(const char*,int);
	bool playSample(int) const;

	void error(const char* text) const;

private:

    BrainSoundFMOD(const BrainSoundFMOD& bb);
    BrainSoundFMOD& operator=(const BrainSoundFMOD& bb);

    FMUSIC_MODULE* m_music;

	bool m_off_toggle;

	std::string m_loaded_music;
};

#endif // BRAINSOUNDFMOD_H

