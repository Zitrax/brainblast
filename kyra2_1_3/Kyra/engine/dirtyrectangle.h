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


#ifndef DIRTY_RECTANGLE_INCLUDED
#define DIRTY_RECTANGLE_INCLUDED


#include "../engine/krmath.h"
#include "../util/gllist.h"


#ifdef DEBUG
	#include "SDL.h"
#endif


class KrDirtyRectangle;
struct SDL_Surface;


struct KrMappedRectInfo
{
	int xmin;
	int ymin;
	int hPixelsPerBit;
	int vPixelsPerRow;

	void Set( const grinliz::Rectangle2I& bounds );
};


/*	Specialized rectangles.
	This adds a 4x8 pixel map (32 bits) that can be used to "quick compare" 
	the rectangles.

	Per byte, the min bit is at x=0, the max bit the right x.
	Each byte is a row, starting with y=0.
*/
class KrMappedRect : public grinliz::Rectangle2I
{
  public:
	KrMappedRect() { min.x = -1; min.y = -1; max.x = -2; max.y = -2; }
	KrMappedRect( const grinliz::Rectangle2I& from, const KrMappedRectInfo& info ) 
		{ min.x = from.min.x; min.y = from.min.y; max.x = from.max.x; max.y = from.max.y; CalcMap( info ); }
	KrMappedRect( const KrMappedRect& from ) 
		{ min.x = from.min.x; min.y = from.min.y; max.x = from.max.x; max.y = from.max.y; map = from.map; }

	void Set( const grinliz::Rectangle2I& from, const KrMappedRectInfo& info )
	{
		min.x = from.min.x;
		min.y = from.min.y;
		max.x = from.max.x;
		max.y = from.max.y;
		CalcMap( info ); 
	}

	void operator=( const KrMappedRect& from )
	{
		min.x = from.min.x;
		min.y = from.min.y;
		max.x = from.max.x;
		max.y = from.max.y;
		map  = from.map; 
	}

	void CalcMap( const KrMappedRectInfo& );

	bool Intersect( const KrMappedRect& rect ) const
	{
		GLASSERT( rect.map );

		if (    ( ( rect.map & map ) == 0 )
			     || rect.max.x < min.x
				 || rect.min.x > max.x
				 || rect.max.y < min.y
				 || rect.min.y > max.y )
		{
			return false;
		}
		return true;
	}	

	U32 Map() const		{ return map; }
  private:
	U32 map;
};

/**	DirtyRectangle manages the rectangle made invalid when Images
	move or change. A movement typically generates 2 dirty rects:
	the old and new location.

	Managing and merging these rectangles is potentially a tricky
	problem. Kyra has some good code to check for overlaps,
	and condense the DR list when they happen.

	Generally speaking, this is used solely by the engine and
	does not need to be directly used. If you need to force an 
	area of the screen to repaint (because you've draw there
	without Kyra's knowledge) you can add dirty rectangles to
	force a redraw.
*/
class KrDirtyRectangle
{
  public:
	KrDirtyRectangle();
	~KrDirtyRectangle();

	/// Turn on clipping bounds. All subsequent rectangles will be clipped to rect.
	void SetClipping( const grinliz::Rectangle2I& rect )		
					{ clippingRect = rect; clipping = true; mappedInfo.Set( clippingRect ); }
	/// Returns true if clipping has been set.
	bool IsClipping()							{ return clipping; }

	/// Adds an invalid rectangle.
	void AddRectangle( const grinliz::Rectangle2I& rect );
	/// Clear the object
	void Clear()								{	//GLOUTPUT( "Rects cleared.\n" );
													nRect = 0; }
	int			  NumRect()						{ return nRect; 	}
	const KrMappedRect& Rect( int i )			{	GLASSERT( i>=0 && i<nRect );
													return rectArray[i];	}

	// The DR maintains both a blit list and a dirty rectangle list.
	// The blit rects are >= the dirty rectangle list.
	// This blits the blit rects to the screen.
	//void UpdateScreen( SDL_Surface* screen );

	#ifdef DEBUG
		void DrawAllRects( SDL_Surface* surface );
//		void DrawBlitRects( SDL_Surface* surface );
		void DrawRects( SDL_Surface* surface );
		void DrawWindow( int y0, int y1 );

		void PrintRects( const char* message );
	#endif

	enum
	{
		// Should be greater than 32. 35+ for some
		// working room. The test I have seem to settle
		// at the best performance at this point, but
		// other apps might be happier with a different
		// number.
		MAX_DIRTY_RECTANGLES = 128,
	};

  private:
	void Remove( int index );
	void HandleOutOfRect( const KrMappedRect& rect );
	
	KrMappedRect rectArray[ MAX_DIRTY_RECTANGLES ];
	
	grinliz::Rectangle2I	clippingRect;
	bool	clipping;

	int nRect;
	KrMappedRectInfo mappedInfo;
};



#endif
