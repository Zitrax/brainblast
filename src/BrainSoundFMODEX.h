/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2013
 * @author Daniel Bengtsson
 */

#ifndef BRAINSOUNDFMODEX_H
#define BRAINSOUNDFMODEX_H

#include "BrainSound.h"
#include "../fmod/fmodex/api/inc/fmod.hpp"

#include <assert.h>
#include <string>
#include <map>

typedef std::map<int, FMOD::Sound*> SoundMap;

/**
 * Implements the BrainSound api using the FmodEx library
 */
class BrainSoundFMODEX : public BrainSound
{
public:

	BrainSoundFMODEX() : 
		BrainSound(),
		m_music_id(1000),
		m_sounds(SoundMap()),
		m_system(NULL),
		m_off_toggle(false) {}
	~BrainSoundFMODEX();

	bool initializeSound() override;
	bool isInitialized() const { return m_initialized == true; }

	// Music functions
	bool loadMusic(const char* file) override;
	bool playMusic() override;
	bool stopMusic() override;
	bool pauseMusic() override;
	bool resumeMusic() override;
	bool toggleMusic() override;

	// Sound effects
	bool addSample(const char*,int) override;
	bool playSample(int) const override;

	void error(const char* text) const;

private:

	BrainSoundFMODEX(const BrainSoundFMODEX& bb);
	BrainSoundFMODEX& operator=(const BrainSoundFMODEX& bb);

	bool checkResult(const FMOD_RESULT& result) const;
	
	const int m_music_id;
	
	SoundMap m_sounds;
	FMOD::System* m_system;
	bool m_off_toggle;
};

#endif // BRAINSOUNDFMOD_H

