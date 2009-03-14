/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */
#include "bbc.h"

namespace bbc {
  
	bool debug = 1;
	bool ERRFLAG = false;

	//----------------------------------------------------------

	// The following code implements a Bresenham line drawing
	// algorithm. There are 4 separate routines each optimized
	// for one of the four pixel depths supported by SDL. SDL
	// support many pixel formats, but it only support 8, 16,
	// 24, and 32 bit pixels.

	//----------------------------------------------------------

	// Draw lines in 8 bit surfaces.

	void line8(SDL_Surface *s, 
			   int x1, int y1, 
			   int x2, int y2, 
			   Uint32 color)
	{
		int d;
		int x;
		int y;
		int ax;
		int ay;
		int sx;
		int sy;
		int dx;
		int dy;

		Uint8 *lineAddr;
		Sint32 yOffset;

		dx = x2 - x1;  
		ax = abs(dx) << 1;  
		sx = bb_sign(dx);

		dy = y2 - y1;  
		ay = bb_abs(dy) << 1;  
		sy = bb_sign(dy);
		yOffset = sy * s->pitch;

		x = x1;
		y = y1;

		lineAddr = ((Uint8 *)(s->pixels)) + (y * s->pitch);
		if (ax>ay)
		{                      /* x dominant */
			d = ay - (ax >> 1);
			for (;;)
			{
				*(lineAddr + x) = (Uint8)color;

				if (x == x2)
				{
					return;
				}
				if (d>=0)
				{
					y += sy;
					lineAddr += yOffset;
					d -= ax;
				}
				x += sx;
				d += ay;
			}
		}
		else
		{                      /* y dominant */
			d = ax - (ay >> 1);
			for (;;)
			{
				*(lineAddr + x) = (Uint8)color;

				if (y == y2)
				{
					return;
				}
				if (d>=0) 
				{
					x += sx;
					d -= ay;
				}
				y += sy;
				lineAddr += yOffset;
				d += ax;
			}
		}
	}

	//----------------------------------------------------------

	// Draw lines in 16 bit surfaces. Note that this code will
	// also work on 15 bit surfaces.

	void line16(SDL_Surface *s, 
				int x1, int y1, 
				int x2, int y2, 
				Uint32 color)
	{
		int d;
		int x;
		int y;
		int ax;
		int ay;
		int sx;
		int sy;
		int dx;
		int dy;

		Uint8 *lineAddr;
		Sint32 yOffset;

		dx = x2 - x1;  
		ax = abs(dx) << 1;  
		sx = bb_sign(dx);

		dy = y2 - y1;  
		ay = bb_abs(dy) << 1;  
		sy = bb_sign(dy);
		yOffset = sy * s->pitch;

		x = x1;
		y = y1;

		lineAddr = ((Uint8 *)s->pixels) + (y * s->pitch);
		if (ax>ay)
		{                      /* x dominant */
			d = ay - (ax >> 1);
			for (;;)
			{
				*((Uint16 *)(lineAddr + (x << 1))) = (Uint16)color;

				if (x == x2)
				{
					return;
				}
				if (d>=0)
				{
					y += sy;
					lineAddr += yOffset;
					d -= ax;
				}
				x += sx;
				d += ay;
			}
		}
		else
		{                      /* y dominant */
			d = ax - (ay >> 1);
			for (;;)
			{
				*((Uint16 *)(lineAddr + (x << 1))) = (Uint16)color;

				if (y == y2)
				{
					return;
				}
				if (d>=0) 
				{
					x += sx;
					d -= ay;
				}
				y += sy;
				lineAddr += yOffset;
				d += ax;
			}
		}
	}

	//----------------------------------------------------------

	// Draw lines in 24 bit surfaces. 24 bit surfaces require
	// special handling because the pixels don't fall on even
	// address boundaries. Instead of being able to store a
	// single byte, word, or long you have to store 3
	// individual bytes. As a result 24 bit graphics is slower
	// than the other pixel sizes.

	void line24(SDL_Surface *s, 
				int x1, int y1, 
				int x2, int y2, 
				Uint32 color)
	{
		int d;
		int x;
		int y;
		int ax;
		int ay;
		int sx;
		int sy;
		int dx;
		int dy;

		Uint8 *lineAddr;
		Sint32 yOffset;

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		color <<= 8;
#endif

		dx = x2 - x1;  
		ax = abs(dx) << 1;  
		sx = bb_sign(dx);

		dy = y2 - y1;  
		ay = bb_abs(dy) << 1;  
		sy = bb_sign(dy);
		yOffset = sy * s->pitch;

		x = x1;
		y = y1;

		lineAddr = ((Uint8 *)(s->pixels)) + (y * s->pitch);
		if (ax>ay)
		{                      /* x dominant */
			d = ay - (ax >> 1);
			for (;;)
			{
				Uint8 *p = (lineAddr + (x * 3));
				memcpy(p, &color, 3);

				if (x == x2)
				{
					return;
				}
				if (d>=0)
				{
					y += sy;
					lineAddr += yOffset;
					d -= ax;
				}
				x += sx;
				d += ay;
			}
		}
		else
		{                      /* y dominant */
			d = ax - (ay >> 1);
			for (;;)
			{
				Uint8 *p = (lineAddr + (x * 3));
				memcpy(p, &color, 3);

				if (y == y2)
				{
					return;
				}
				if (d>=0) 
				{
					x += sx;
					d -= ay;
				}
				y += sy;
				lineAddr += yOffset;
				d += ax;
			}
		}
	}

	//----------------------------------------------------------

	// Draw lines in 32 bit surfaces. Note that this routine
	// ignores alpha values. It writes them into the surface
	// if they are included in the pixel, but does nothing
	// else with them.

	void line32(SDL_Surface *s, 
				int x1, int y1, 
				int x2, int y2, 
				Uint32 color)
	{
		int d;
		int x;
		int y;
		int ax;
		int ay;
		int sx;
		int sy;
		int dx;
		int dy;

		Uint8 *lineAddr;
		Sint32 yOffset;

		dx = x2 - x1;  
		ax = bb_abs(dx) << 1;  
		sx = bb_sign(dx);

		dy = y2 - y1;  
		ay = bb_abs(dy) << 1;  
		sy = bb_sign(dy);
		yOffset = sy * s->pitch;

		x = x1;
		y = y1;

		lineAddr = ((Uint8 *)(s->pixels)) + (y * s->pitch);
		if (ax>ay)
		{                      /* x dominant */
			d = ay - (ax >> 1);
			for (;;)
			{
				*((Uint32 *)(lineAddr + (x << 2))) = (Uint32)color;

				if (x == x2)
				{
					return;
				}
				if (d>=0)
				{
					y += sy;
					lineAddr += yOffset;
					d -= ax;
				}
				x += sx;
				d += ay;
			}
		}
		else
		{                      /* y dominant */
			d = ax - (ay >> 1);
			for (;;)
			{
				*((Uint32 *)(lineAddr + (x << 2))) = (Uint32)color;

				if (y == y2)
				{
					return;
				}
				if (d>=0) 
				{
					x += sx;
					d -= ay;
				}
				y += sy;
				lineAddr += yOffset;
				d += ax;
			}
		}
	}

	//----------------------------------------------------------

	// Examine the depth of a surface and select a line
	// drawing routine optimized for the bytes/pixel of the
	// surface.

	void line(SDL_Surface *s, 
			  int x1, int y1, 
			  int x2, int y2, 
			  Uint32 color)
	{
		switch (s->format->BytesPerPixel)
		{
		case 1:
			line8(s, x1, y1, x2, y2, color);
			break;
		case 2:
			line16(s, x1, y1, x2, y2, color);
			break;
		case 3:
			line24(s, x1, y1, x2, y2, color);
			break;
		case 4:
			line32(s, x1, y1, x2, y2, color);
			break;
		}
	}


	template<class T> int round( T value ) {

		int truncated = static_cast<int>(value);
    
		double decimals = value - truncated;
    
		if ( decimals < 0.5 ) return truncated;

		return (truncated+1);

	}

// 	int randint(int max)
// 	{ 
// 		return int(max*rand()/(RAND_MAX+1.0)); 
// 	}


	std::string levelSetToString( LEVEL_SET level_set )
	{
		switch(level_set)
		{
		case RANDOM:
			return "Random";
		case EASY:
			return "Easy";
		case NORMAL:
			return "Normal";
		case HARD:
			return "Hard";
		case UNKNOWN:
			return "Unknown";
		}
		
		return "Unknown";
	}
	
	LEVEL_SET stringToLevelSet(std::string str)
	{
		if( str == "Random" )
			return RANDOM;
		else if( str == "Easy" )
			return EASY;
		else if( str == "Normal" )
			return NORMAL;
		else if( str == "Hard" )
			return HARD;
		else
			return UNKNOWN;
	}

	LEVEL_SET intToLevelSet(int i)
	{
		if( i > HARD )
			return UNKNOWN;
		return static_cast<LEVEL_SET>(i);
	}

}

// Smarter(more common/intuitive) way of doing this ?
void generator() { bbc::round(0.5); }

