/*--License:
	Kyra Sprite Engine
	Copyright Lee Thomason (Grinning Lizard Software) 2001-2002
	www.grinninglizard.com/kyra
	www.sourceforge.net/projects/kyra

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	The full text of the license can be found in license.txt
*/

#include <stdlib.h>
#include <stdio.h>

#include "SDL.h"
#include "../engine/dirtyrectangle.h"
#include "../engine/painter.h"


int main(int argc, char *argv[])
{
	SDL_Surface* screen;

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(255);
	}
	atexit(SDL_Quit);

	/* Create a display for the image */
	int depth = SDL_VideoModeOK(600, 400, 32, SDL_SWSURFACE);
	if ( depth < 16 )
		depth = 24;

	screen = SDL_SetVideoMode( 600, 400, depth, SDL_SWSURFACE);
	if ( screen == NULL ) {
		fprintf(stderr,"Couldn't set video mode: %s\n", SDL_GetError());
		exit(3);
	}

	KrDirtyRectangle dirtyRect;
	KrDRIterator     dirtyRectIt;

	bool done = false;
	SDL_EnableUNICODE( 1 );
	int y0, y1;
	const int SIZE = 400;
	const int GRID = 20;

	while( !done )
	{
		SDL_FillRect( screen, 0, 0 );
		dirtyRect.DrawAllRects( screen );
		dirtyRect.DrawRects( screen );
		dirtyRect.DrawBlitRects( screen );
// 		dirtyRect.DrawWindow( screen );
		SDL_UpdateRect( screen, 0, 0, 0, 0 );

		SDL_Event event;

		if ( !SDL_WaitEvent( &event ) )
			break;
	
		switch ( event.type )
		{
			case SDL_QUIT:
				done = true;
				break;

			case SDL_KEYDOWN:
			{
				switch( event.key.keysym.unicode )
				{
					case ' ':
					{
						KrRect rect;
						rect.FromPair( (( rand() / GRID ) * GRID ) %SIZE,
									   (( rand() / GRID ) * GRID ) %SIZE,
									   (( rand() / GRID ) * GRID ) %SIZE,
									   (( rand() / GRID ) * GRID ) %SIZE );
						
						dirtyRect.AddRectangle( rect );
					}
					break;

					case 'c':
					case 'C':
					{
						dirtyRect.Clear();
					}
					break;

					case '0':
					{
						dirtyRect.StartWalk();
						y0 = 20 * rand()%3;
						y1 = y0 + rand()%(SIZE / 4);
						
						GLOUTPUT( "y0=%d y1=%d\n", y0, y1 );
						dirtyRectIt = dirtyRect.GetIterator( y0, y1 );						
						dirtyRectIt.DebugDump();
					}
					break;

					case '1':
					{
						y0 = y1 + 20 * rand()%3;
						y1 = y0 + rand()%(SIZE / 4);

						GLOUTPUT( "y0=%d y1=%d\n", y0, y1 );
						dirtyRectIt = dirtyRect.GetIterator( y0, y1 );						
						dirtyRectIt.DebugDump();
					}
					break;
				}
			}
			break;
		}
	}
		
	/* We're done! */
	exit(0);

	return 0;
}


