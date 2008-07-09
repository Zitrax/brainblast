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

	BrainSound() : m_initialized(false) {}
	virtual ~BrainSound(){}

	/**
	 * Initialize the sound library
	 * @return true if everything went fine and the other functions can be used
	 */
	virtual bool initializeSound() = 0;

	/**
	 * @return true if \ref initializeSound was called and successfull.
	 *  If this returns false you should not use the sound functions.
	 */
	bool isInitialized() const { return m_initialized == true; }

	// Music functions
	
	/**
	 * Load music file but do not start playing it.
	 * @return false if the file could not be loaded
	 */
	virtual bool loadMusic(const char* file) = 0;

	/**
	 * Play the music loaded in \ref loadMusic()
	 * @return false if the music could not be played
	 */
	virtual bool playMusic()   = 0;

	/**
	 * Stop the currently played music
	 */
	virtual bool stopMusic()   = 0;

	/**
	 * Pause the currently played music
	 */
	virtual bool pauseMusic()  = 0;

	/**
	 * If music have been paused in \ref pauseMusic() it can be resumed here.
	 */
	virtual bool resumeMusic() = 0;

	/**
	 * If paused resumes, if playing it pauses.
	 */
	virtual bool toggleMusic() = 0;

	// Sound effects

	/**
	 * Add a sound sample from file
	 * @return false if the sample could not be loaded
	 */
	virtual bool addSample(const char* file,int id) = 0;

	/**
	 * Play a sample added in \ref addSample
	 */
	virtual bool playSample(int id) const = 0;

protected:

	bool m_initialized;
	
};

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
