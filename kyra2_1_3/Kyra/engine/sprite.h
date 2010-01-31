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

#ifndef KYRA_SPRITE_INCLUDED
#define KYRA_SPRITE_INCLUDED

#include "image.h"
#include "spriteresource.h"


/**	The most obvious class for a sprite image, describes on
	on screen image, potentially with multiple actions and 
	frames.

	Almost all pre-generated images are sprites. The only reason
	to use a tile is for rotation.

	A Sprite contains an arbitrary number of actions, each of which
	has any number of frames.

	@verbatim
	An example:
		Sprite:	"DRONE", an alien creature
			Action: "WALK.DIR7", walking NW
				Frames: 0, 1, 2, 3, 4, 5
			Action: "WALK.DIR9", walking NE
				Frames: 0, 1, 2, 3, 4, 5
			Action: "STANDING", no motion
				Frames: 0

	@endverbatim
*/
class KrSprite : public KrImage
{
  public:
	KrSprite( KrSpriteResource* resource );
	virtual ~KrSprite()		{}

	/// Set the current frame for the current action.
	void SetFrame( int i );

	/// Set the current action by its name. "STANDING"
	void SetAction( const std::string& actionName );

	/**	[Experimental, added 1.6]
		Rotations applied by the encoder change the action name to reflect
		the rotation. ROT000, where 000 is the rotation, in integer degrees.
		It as appended as the last segment of the name. For instance,

			"STANDING.ROT090" and STANDING | ROT090 
			
		would be the string and integer representations. This method, giving
		the leading string ("STANDING") will add the rotation automatically.
	*/
	void SetActionRotated( const std::string& actionName, int rotation );

	/** Note this is the action Id (written by the encoder: STANDING),
		NOT the index (whatever that happens to be.
	*/
	void SetAction( U32 id );

	/// Get a pointer to the current action.
	KrAction* GetAction()	{ return action; }
	/// Get the name of the current action.
	std::string ActionName() const	{ return action->Name(); }

	/// Return the number of frames in the current action.
	int  NumFrames() const		{ return action->NumFrames(); }
	/// Return the current frame of the current action.
	int  Frame() const			{ return frame; }
	/// Get a pointer to the current frame
	KrRle* GetFrame() const		{ return action->GetFrame( frame ); };

	/** Take one step: advance the frame and move the position of the sprite.
		Not all sprites have steps. They are configured in the Sprite
		editor.
	*/
	void DoStep();	
	
	/// Get the current step (x and y delta to the next frame) 
	grinliz::Vector2I StepSize() const	{ return action->GetFrame( frame )->StepSize(); }

	/// Get the sum of distance covered by all the DoSteps
	grinliz::Vector2I Stride();		

	virtual void QueryBoundingBox( grinliz::Rectangle2I* boundingBox, int window = 0 );

	/// Get the resource for this Sprite.
	KrSpriteResource*   SpriteResource()	{ return resource; }
	virtual KrResource* Resource()			{ return resource; }


	virtual bool HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int window );
	virtual KrImNode* Clone();

	// -- internal -- //
	// Recursive call to draw a node and all its children.
	virtual void Draw(	KrPaintInfo* paintInfo,
						const grinliz::Rectangle2I& clip,
						int window );

	virtual KrSprite* ToSprite()	{ return this; }
	virtual void	CalcTransform( int win );

  private:
    KrSprite();                                 // not defined and not allowed
    KrSprite& operator=( const KrSprite& );    // not defined and not allowed.

	KrAction*			action;
	KrSpriteResource*	resource;
	int					frame;
};


#endif
