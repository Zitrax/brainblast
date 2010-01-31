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

#ifndef KYRA_SPRITERESOURCE_INCLUDED
#define KYRA_SPRITERESOURCE_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "kyraresource.h"
// #include "tags.h"
#include "../engine/krmath.h"
#include "action.h"
#include <map>

/** Practically, a sprite resource is used to create a sprite.
	A sprite resource is a type of resource and a container for 
	actions.

	SpriteResource
		Action
			Frames ( Rle )
*/
class KrSpriteResource : public KrResource
{
  public:
	// Create by reading from a .dat file
	KrSpriteResource( U32 size, SDL_RWops* data );

	/*  Create the resource from the program. (Used by the sprite
		editor for animating and aligning sprites.) 
		After construction, SetAction is called.
	*/
	KrSpriteResource( const std::string& spriteName );
	
	virtual ~KrSpriteResource();

	// Pass in an action to the resource. Used by the sprite editor.
	// Will allocate the action, if necessary.
	void AddAction( KrAction* action );

	virtual	U32 Type()								{ return KYRATAG_SPRITE; }
	virtual const std::string&	TypeName()			{ return spriteName; }
	virtual KrSpriteResource* ToSpriteResource()	{ return this; }

	/// Return the count of actions in this sprite.
	int		  NumActions()					{ return actionArr.size(); }
	/// Fetch an action from this Sprite Resource by the name of the action.
	KrAction* GetAction( const std::string& actionName );
	/// Fetch an action from this Sprite Resource by the id of the action.
	KrAction* GetAction( U32 actionId );
	
	/** It is sometimes useful to get a Canvas Resource that
		is created from a sprite. This method calculates the bounds,
		creates the Resource, and returns it. If there is an error,
		0 is returned.
	*/
	KrCanvasResource* CreateCanvasResource(	const std::string& actionName,
												int frame, int* hotx, int* hoty );

	/// Same functionality, uses the id instead of the name.
	KrCanvasResource* CreateCanvasResource(	U32 actionId,
												int frame, int* hotx, int* hoty );

	// -- internal -- //
	KrAction* GetActionByIndex( int i )		{ return actionArr[i]; }
	const KrAction& ActionByIndex( int i )	{ return *actionArr[i]; }

	/*  Draw a sprite resource.
		@param	surface		Target surface.
		@param  paintInfo	Information about the target surface for drawing (optimizing).
		@param	action		The name of the action to draw.
		@param	frame		A frame # to draw.
		@param	x			X location in pixels.
		@param	y			Y location in pixels.
		@param  cForm		Color transformation applied to the drawing.
		@param	clip		A clipping rectangle, which can be null.
	*/
	void Draw( KrPaintInfo* paintInfo, 
			   const std::string& action,
			   int frame,
			   const KrMatrix2& matrix,
			   const KrColorTransform& cForm,
			   const grinliz::Rectangle2I& clip,
			   int quality );

	/*  Do a HitTest (see KrImageTree::HitTest) in transformed 
		coordinates. So the tree object that made this call
		has already transformed the x and y into local pixel coords.
	*/
	bool HitTestTransformed( int x, int y, int hitFlags )	{	GLASSERT( 0 ); return false; }	// never called directly. Use the action.

	virtual void CacheScale( GlFixed xScale, GlFixed yScale );
	virtual bool IsScaleCached( GlFixed xScale, GlFixed yScale );
	virtual void FreeScaleCache();
	virtual KrCollisionMap* GetCollisionMap( KrImage* state, int window );

	virtual void Save( KrEncoder* );

  protected:
	std::vector< KrAction* >	actionArr;			// the actions are stored here

  private:

	const static std::string spriteName;

	std::map< std::string, KrAction* >  actionMap;		// the map is fast access to the action
	std::map< U32, KrAction* >			actionIdMap;	// the map is fast access to the action
};


#endif
