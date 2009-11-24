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

#include "SDL.h"
#include "../engine/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include "../engine/painter.h"


int main( int argc, char* argv[] )
{
 	int ret = SDL_Init( SDL_INIT_VIDEO );
	GLASSERT( ret == 0 );

	if ( argc < 2 )
	{
		fprintf(stderr,"Usage: tilemake height\n" );
		fprintf(stderr,"Makes an isometric tile and writes the output "
				"to 'tile.bmp'.\n" );

		exit( 0 );
	}

	int height = atoi( argv[1] );
	if ( height <=0 )
	{
		fprintf(stderr,"Height must be greater than 0.\n" );
		exit( 0 );
	}

	SDL_Surface* screen;

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(255);
	}
	atexit(SDL_Quit);

	/* Create a display for the image */
	const int screenWidth  = 300;
	const int screenHeight = 200;
	screen = SDL_SetVideoMode( screenWidth, screenHeight, 24, SDL_SWSURFACE);
	if ( screen == NULL ) {
		fprintf(stderr,"Couldn't set video mode: %s\n", SDL_GetError());
		exit(3);
	}

	int width = height * 2;

	KrPainter painter( screen );
	int draw = ( width - 1 ) * 2;
	int x = 1;

	for( int j=0; j<height; j++ )
	{
		painter.DrawHLine( x, height+j, draw, 255, 255, 255 );
		painter.DrawHLine( x, height-1-j, draw, 255, 255, 255 );
		draw -= 4;
		x += 2;
	}
	if ( !(screen->flags & SDL_OPENGL ) )
		SDL_SaveBMP( screen, "tile.bmp" );

	SDL_Event event;
	while( SDL_WaitEvent( &event ) )
	{
		if ( event.type == SDL_QUIT || event.type == SDL_KEYDOWN )
			break;
	}

	exit( 0 );
	return 0;
}
