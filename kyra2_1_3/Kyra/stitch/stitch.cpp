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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "stitch.h"
#include "SDL_image.h"

FrameManager::FrameManager()
{}


void FrameManager::Add( 	const char* filename,
							const char* action,
							int skip,
							int scale )
{
	// Look for the # in the filename:
	const char* p = 0;
	const char* start = 0, *end = 0;
	int nDigits = 0;
	int startIndex = surface.Count();

	for( p=filename; *p; ++p )
	{
		if ( isdigit( *p ) )
		{
			start = p;
			break;
		}
	}

	for( p=filename + strlen(filename) - 1; p >= filename; --p )
	{
		if ( isdigit( *p ) )
		{
			end = p;
			break;
		}
	}


	if ( start && *start && end && *end )
	{
		nDigits = end - start + 1;
	}
	if ( nDigits == 0 )
	{
		SDL_Surface* s = IMG_Load( filename );
		surface.PushBack( s );
		actionName.PushBack( action );
	}
	else
	{
		char buf[512];
		char printBuf[64];

		int maxFrame = 1;
		int i;

		for ( i=0; i<nDigits; i += skip )
			maxFrame *= 10;
		sprintf( printBuf, "%c0%dd", '%', nDigits );

		// Get the first digit.
		strcpy( buf, start );
		*( buf + nDigits ) = 0;
		i = atoi( buf );

		for( ; i<maxFrame; i += skip )
		{
			strcpy( buf, filename );
			sprintf( buf + ( start - filename ), printBuf, i );
			strcpy( buf + ( end - filename ) + 1, end + 1 );

			SDL_Surface* s = IMG_Load( buf );
			if ( s )
			{
				GLOUTPUT( "File '%s' loaded\n", buf );
				surface.PushBack( s );
				actionName.PushBack( action );
			}
			else
			{
				break;
			}
		}
	}

	// Create the surfaces where the work gets done.
	for( unsigned i=startIndex; i<surface.Count(); ++i )
	{
		int width  = ( surface[i]->w + scale - 1 ) / scale;
		int height = ( surface[i]->h + scale - 1 ) / scale;

		SDL_Surface* s = SDL_CreateRGBSurface( SDL_SWSURFACE,
											   width, height,
											   32,
											   0x00ff0000,
											   0x0000ff00,
											   0x000000ff,
											   0xff000000 );
		GLASSERT( s );
		processed.PushBack( s );
	}
}


FrameManager::~FrameManager()
{
	while ( !surface.Empty() )
	{
		SDL_Surface* s = surface.Back();
		SDL_FreeSurface( s );
		surface.PopBack();
	}
	while ( !processed.Empty() )
	{
		SDL_Surface* s = processed.Back();
		SDL_FreeSurface( s );
		processed.PopBack();
	}
}