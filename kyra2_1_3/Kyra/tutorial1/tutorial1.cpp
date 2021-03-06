#include "SDL.h"
#include <stdlib.h>
#include "../engine/kyra.h"


using namespace grinliz;

#define SDL_TIMER_EVENT ( SDL_USEREVENT + 0 )
const int TIMER_INTERVAL = 80;

const int SCREENX = 640;
const int SCREENY = 480;


Uint32 TimerCallback(Uint32 interval)
{
	SDL_Event event;
	event.type = SDL_TIMER_EVENT;

	SDL_PeepEvents( &event, 1, SDL_ADDEVENT, 0 );
	return TIMER_INTERVAL;
}


int main( int argc, char *argv[] )
{
	// code_d A random number generator.
	Random random;

	SDL_Surface* screen;

	const SDL_version* sdlVersion = SDL_Linked_Version();
	if ( sdlVersion->minor < 2 )
	{
		printf( "SDL version must be at least 1.2.0" );
		exit( 254 );
	}

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) < 0 ) {
		printf( "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(255);
	}

	/* Create a display for the image */
	screen = SDL_SetVideoMode( SCREENX, SCREENY, 0, SDL_SWSURFACE );
	if ( screen == NULL ) {
		exit(3);
	}

	KrEngine* engine = new KrEngine( screen );
	engine->Draw();

	SDL_Event event;
	bool done = false;

	// Start timing!
	SDL_SetTimer( TIMER_INTERVAL, TimerCallback );

	while( !done && SDL_WaitEvent(&event) )
	{
		if ( event.type == SDL_QUIT )
			break;

		switch(event.type)
		{
			case SDL_KEYDOWN:
			{
				done = true;
			}
			break;

			case SDL_TIMER_EVENT:
			{
				// code_d Walk the list and move the particles
				engine->Draw();
			}
			break;

			default:
				break;
		}

	}

	delete engine;

	SDL_Quit();	
	return 0;
}
