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

#ifndef KYRA_ACTION_INCLUDED
#define KYRA_ACTION_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif 

#include <string>
#include <vector>

#include "SDL.h"
#include "rle.h"

class KrCanvasResource;
class KrEncoder;
class KrCollisionMap;


/** An action is contained by a sprite resource and is itself a
	container for frames.
	@sa KrSprite
*/
class KrAction
{
  public:
	/*  Create an empty KrAction. Used with the AddFrame method
		to create an action.
	*/
	KrAction( const std::string& name );	
	
	/// Create the action by reading from a file. Used by the sprite constructor.
	KrAction( SDL_RWops* data );

	~KrAction();

	/// Each action has a name (ex, "Walking")
	const std::string& Name() const		{ return name; }
	/// Each action has a unique id. (ex, WALKING)
	U32				   Id() const		{ return id; }

	/*  Draw the action to the surface.
		Normally called by the Sprite.
		@param paintInfo	Target surface information
		@param frame		The frame number to draw.
		@param x			x value to draw -- measured at hotspot.
		@param y			y value to draw -- measured at hotspot.
		@param cForm		The color transformation to use.
		@param clip			A clipping rectangle. (Can be NULL)
	*/
	void Draw( KrPaintInfo* paintInfo,	
			   int frame,				
			   const KrMatrix2& matrix,		
			   const KrColorTransform& cForm,
			   const grinliz::Rectangle2I& clip );
	
	/// Total number of frames.
	int   NumFrames() const 			{ return numFrames; }

	/// Get a frame.
	const KrRle& Frame( int i ) const	{ GLASSERT( i >= 0 );
										  GLASSERT( i < numFrames );
										  return frame[ i ]; }

	/*  Get a non-conts pointer to the frame. A special case call.
	*/
	KrRle* GetFrame( int i ) const		{ GLASSERT( i >= 0 );
										  GLASSERT( i < numFrames );
										  return &frame[ i ]; }

	bool HitTestTransformed( int frame, int x, int y, int hitFlags );

	// internal
	void CacheScale( GlFixed xScale, GlFixed yScale );
	bool IsScaleCached( GlFixed xScale, GlFixed yScale );
	void FreeScaleCache();
	KrCollisionMap* GetCollisionMap( GlFixed xScale, GlFixed yScale, int frame );
	
	void AddFrame()		{ GrowFrameArray( numFrames + 1 ); }

	/** A strange -- but sometimes useful function. Creates
		a canvas from this action and returns it.
	*/
	KrCanvasResource* CreateCanvasResource( int frame, int* hotx, int* hoty );

	void CalculateBounds( int frame, const KrMatrix2& xForm, grinliz::Rectangle2I* bounds );

	void Save( KrEncoder* encoder );

  private:
	struct CachedBlock
	{
		GlFixed xScale, 
				yScale;
		KrRle** frame;

		bool operator==( const CachedBlock& )	{ GLASSERT( 0 ); return false; }		// be nice to buggy compilers.
	};
	void GrowFrameArray( int newSize );

	std::vector< CachedBlock > cache;

	std::string		name;
	U32				id;
	KrRle*			frame;			// Can't use dynamic array since it doesn't have proper copy
	int				numFrames;
};


#endif
