/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#ifndef BBC_H
#define BBC_H

#include <iostream>
#include <string>

#include <wSDL.h>

class Brainblast;

// Should change to use C++ versions of these
#define bb_max(a,b) (((a) > (b)) ? (a) : (b))
#define bb_min(a,b) (((a) < (b)) ? (a) : (b))
#define bb_abs(a) (((a)<0) ? -(a) : (a))
#define bb_sign(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

// Avoid defines here ?
#define zap(x) if(x){delete(x); x=0;}
#define zapArr(x) if(x){delete [] x; x=0;}

namespace bbc{
  
	extern bool debug;

	enum{ERROR, NOTUSED};

	extern bool ERRFLAG;

	struct error{
		std::string msg;
		error(std::string s) : msg(s){}
	};

	/**
	 * Marks which types of level that is being played.
	 * This is stored in the highscore table and is 
	 * also used to determine which level to select
	 * when you are done with a previous level.
	 */
	typedef enum
	{
		RANDOM,
		EASY,
		NORMAL,
		HARD,
		UNKNOWN = 2048
	} LEVEL_SET;
	
	void line(SDL_Surface *s, 
			  int x1, int y1, 
			  int x2, int y2, 
			  Uint32 color);

	void line8(SDL_Surface *s, 
			   int x1, int y1, 
			   int x2, int y2, 
			   Uint32 color);

	void line16(SDL_Surface *s, 
				int x1, int y1, 
				int x2, int y2, 
				Uint32 color);
  
	void line32(SDL_Surface *s, 
				int x1, int y1, 
				int x2, int y2, 
				Uint32 color);
	
	template<class T> int round( T value );
	
	inline int randint(int min,int max) 
	{
		return min+int((max-min+1)*(rand()/(RAND_MAX+1.0)));
	}

	std::string levelSetToString( LEVEL_SET level_set );
	LEVEL_SET stringToLevelSet(std::string str);
	LEVEL_SET intToLevelSet(int i);

}

#endif
