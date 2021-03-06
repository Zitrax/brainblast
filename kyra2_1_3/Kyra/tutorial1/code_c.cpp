/*--License:
	Kyra Sprite Engine
	Copyright Lee Thomason (Grinning Lizard Software) 2001-2005
	www.grinninglizard.com/kyra
	www.sourceforge.net/projects/kyra

	Kyra is provided under the LGPL. 
	
	I kindly request you display a splash screen (provided in the HTML documentation)
	to promote Kyra and acknowledge the software and everyone who has contributed to it, 
	but it is not required by the license.

--- LGPL License --

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	The full text of the license can be found in lgpl.txt
*/


#include "SDL.h"
#include <stdlib.h>
#include "../engine/kyra.h"

// code_b Include the generated header
#include "tutorial1.h"

#define SDL_TIMER_EVENT ( SDL_USEREVENT + 0 )

using namespace grinliz;
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

	// code_b Load the dat file.
	// The dat file was carefully created in the sprite
	// editor. Loading allows us access to the 
	// MAGE, PARTICLE, and CARPET.
	if ( !engine->Vault()->LoadDatFile( "tutorial1.dat" ) )
	{
		printf( "Error loading the tutorial dat file\n" );
		exit( 255 );
	}

	// code_b Get the CARPET resource
	KrSpriteResource* carpetRes = engine->Vault()->GetSpriteResource( TUT1_CARPET );
	GLASSERT( carpetRes );

	// code_b Create the carpet sprite and add it to the tree
	KrSprite* carpet = new KrSprite( carpetRes );
	carpet->SetPos( SCREENX, SCREENY / 2 );
	engine->Tree()->AddNode( 0, carpet );

	// code_c Get the MAGE resource, create the mage.
	KrSpriteResource* mageRes = engine->Vault()->GetSpriteResource( TUT1_MAGE );
	GLASSERT( mageRes );
	KrSprite* mage = new KrSprite( mageRes );

	// code_c Add the Mage as a child of the carpet.
	engine->Tree()->AddNode( carpet, mage );

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
				// code_b Move the carpet.
				carpet->DoStep();
				if ( carpet->X() < 0 )
				{
					carpet->SetPos( SCREENX, carpet->Y() );
				}

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

