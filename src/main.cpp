/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2007
 * @author Daniel Bengtsson
 */

#include "Brainblast.h"

int main(int /*argc*/, char* /*argv*/ [])
{
	const SDL_version* sdlVersion = SDL_Linked_Version();
	if ( sdlVersion->minor < 2 )
	{
		printf( "ERROR: SDL version must be at least 1.2.0" );
		exit( 254 );
	}
	
	Brainblast* bb = new Brainblast();
	bb->startGame(1);
    zap(bb);
}
