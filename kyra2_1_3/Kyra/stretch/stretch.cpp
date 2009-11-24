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
#include "SDL_Image.h"
#include "../engine/kyra.h" 

int gQuality = 0;


void SoftStretch( SDL_Surface* source, SDL_Surface* screen, 
				  int targetW, int targetH )
{
	GLASSERT( screen->format->BytesPerPixel == 3);
	GLASSERT( source->format->BytesPerPixel == 3);

	U8* targetRow		= (U8*) screen->pixels;
	U8* targetPixel     = (U8*) screen->pixels;

	U8* sourceRow		= (U8*) source->pixels;
	U8* lastRow         = ( (U8*) source->pixels + source->pitch * source->h );
	U8* sourcePixel    = (U8*) source->pixels;

	bool downsize = (    targetW < source->w / 2
					  && targetH < source->h / 2);

	int i, j, k;

	U32 xInc = 0x10000 * source->w / targetW;
	U32 yInc = 0x10000 * source->h / targetH;
	U32 xFraction = 0;
	U32 yFraction = 0;

	for ( j=0; j<targetH; ++j )
	{
		for( i=0; i<targetW; ++i )
		{
			for( k=0; k<3; ++k )
			{
				if ( gQuality == 0)
				{
					// No interpolation
					*targetPixel = sourcePixel[k];
					++targetPixel;
				}
				else
				{
					if ( !downsize || gQuality < 2 )
					{
						// Bi-linear
						U8* s00 = sourcePixel + k;
						U8* s10 = sourcePixel + k;
						U8* s01 = sourcePixel + k;
						U8* s11 = sourcePixel + k;

						if ( sourcePixel < ( sourceRow + ( ( source->w - 1 ) * 3 )) )
						{
							s10 += 3;
							s11 += 3;
						}

						if ( sourceRow != lastRow )
						{
							s01 += source->pitch;
							s11 += source->pitch;
						}

						U32 g0, g1, g;

						g0 = (   (( *s00  * ( 0x10000 - xFraction ) ) >> 16 )
							   + (( *s10 * ( xFraction ) ) >> 16 ) );
						g1 = (   (( *s01  * ( 0x10000 - xFraction ) ) >> 16 )
							   + (( *s11 * ( xFraction ) ) >> 16 ) );
						g  = (   (( g0 * ( 0x10000 - yFraction ) ) >> 16 )
							   + (( g1 * ( yFraction ) ) >> 16 ) );
						
						*targetPixel = g;
						++targetPixel;
					}
					else
					{
						// Downsize
						GLASSERT( xInc >= 0x1000 );
						GLASSERT( yInc >= 0x1000 );
						U32 dx = ( xInc + xFraction ) >> 16;
						U32 dy = ( yInc + yFraction ) >> 16;
						U32 g = 0;

						U8* row = sourcePixel;

						U32 ii, jj;

						for( jj=0; jj<dy; jj++ )
						{
							for( ii=0; ii<dx; ii++ )
							{
								g += row[ii*3+k];
							}
							row += source->pitch;
						}
						g = g / ( dx * dy );
						*targetPixel = g;
						++targetPixel;
					}
				}
			}

			xFraction += xInc;
			while( xFraction >= 0x10000 )
			{
				sourcePixel += 3;
				xFraction -= 0x10000;
			}
		}

		yFraction += yInc;
		while( yFraction >= 0x10000 )
		{
			sourceRow += source->pitch;
			yFraction -= 0x10000;
		}

		sourcePixel = sourceRow;

		targetRow += screen->pitch;
		targetPixel = targetRow;

		xFraction = 0;
	}
}

/*
void SoftStretch( SDL_Surface* source, SDL_Surface* screen, 
				  int targetW, int targetH )
{
	GLASSERT( screen->format->BytesPerPixel == 3);
	GLASSERT( source->format->BytesPerPixel == 3);

	U8* targetRow		= (U8*) screen->pixels;
	U8* targetPixel	    = (U8*) screen->pixels;
	U8* sourceRow		= (U8*) source->pixels;
	U8* sourcePixel	    = (U8*) source->pixels;

	int i, j, k;

	U32 xInc = 0x10000 * source->w / targetW;
	U32 yInc = 0x10000 * source->h / targetH;
	U32 xFraction = 0;
	U32 yFraction = 0;

	for ( j=0; j<targetH; ++j )
	{
		for( i=0; i<targetW; ++i )
		{
			for( k=0; k<3; ++k )
			{
				*targetPixel = sourcePixel[k];
				++targetPixel;
			}

			xFraction += xInc;
			while( xFraction >= 0x10000 )
			{
				sourcePixel += 3;
				xFraction -= 0x10000;
			}
		}

		yFraction += yInc;
		while( yFraction >= 0x10000 )
		{
			sourceRow += source->pitch;
			yFraction -= 0x10000;
		}

		sourcePixel = sourceRow;
		targetRow += screen->pitch;
		targetPixel = targetRow;

		xFraction = 0;
	}
}
*/

int main(int argc, char *argv[])
{
	SDL_Surface* screen;

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(255);
	}

	/* Create a display for the image */
// 	int depth = SDL_VideoModeOK(600, 400, 32, SDL_SWSURFACE);
// 	if ( depth < 16 )
// 		depth = 24;

	screen = SDL_SetVideoMode( 600, 400, 24, SDL_SWSURFACE);
	if ( screen == NULL ) {
		fprintf(stderr,"Couldn't set video mode: %s\n", SDL_GetError());
		exit(3);
	}


	SDL_Event event;

	SDL_Surface* source;
	SDL_Surface* sourceUnknown;
	sourceUnknown = IMG_Load( "stretchtest.tga" );
	source = SDL_CreateRGBSurface(	SDL_SWSURFACE,
									sourceUnknown->w,
									sourceUnknown->h,
									screen->format->BitsPerPixel,
									screen->format->Rmask,
									screen->format->Gmask,
									screen->format->Bmask,
									screen->format->Amask );

	SDL_BlitSurface( sourceUnknown, 0, source, 0 );
	SDL_BlitSurface( source, 0, screen, 0 );

	int w = source->w;
	int h = source->h;

	while ( SDL_WaitEvent( &event ) )
	{
		if ( event.type == SDL_QUIT )
			break;

		switch ( event.type )
		{
			case SDL_MOUSEMOTION:
			{
				w = GlClamp( event.motion.x + 1, 1, screen->w );
				h = GlClamp( event.motion.y + 1, 1, screen->h );

				SDL_FillRect( screen, 0, 0 );
 				SoftStretch( source, screen, w, h );
				SDL_UpdateRect( screen, 0, 0, 0, 0 );
			}
			break;

			case SDL_MOUSEBUTTONDOWN:
			{
				SDL_FillRect( screen, 0, 0 );
				SDL_BlitSurface( source, 0, screen, 0 );
				SDL_UpdateRect( screen, 0, 0, 0, 0 );
			}
			break;

			case SDL_KEYDOWN:
			{
				if (	event.key.keysym.sym >= '1' 
					 && event.key.keysym.sym <= '3' )
				{
					gQuality = event.key.keysym.sym - '1';
				}
 				SoftStretch( source, screen, w, h );
				SDL_UpdateRect( screen, 0, 0, 0, 0 );
			}
			break;

			default:
				break;
		}
	}

		
	/* We're done! */
	SDL_Quit();
	return 0;
}


