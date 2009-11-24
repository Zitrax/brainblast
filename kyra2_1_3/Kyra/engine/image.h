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

#ifndef KYRA_IMAGE_INCLUDED
#define KYRA_IMAGE_INCLUDED

#include "imnode.h"

struct KrPaintInfo;
class KrResource;

#ifdef _MSC_VER
// Disables warning about using a this pointer in a constructor initialization list.
#pragma warning ( disable : 4355 )
#endif

/**	The parent class for any Node that actually draws to the screen.
	@sa KrImNode
*/
class KrImage : public KrImNode
{
  public:
	KrImage();
	virtual ~KrImage()	{}

	/*  Draw to a surface
		@param surface		Target surface.
		@param paintInfo	Drawing information.
		@param clip			Cliping rectangle for the surface.
	*/
	virtual void Draw(	KrPaintInfo* paintInfo, const grinliz::Rectangle2I& clip, int window ) = 0;

	/**	Get the bounding box of the image, in its current state.

		If this is a sprite, it will be the bounding box of the
		current frame and action. The bounds are relative to the
		hotspot, so xmin and ymin will often be not zero.

		The current transformation matrix will be applied, so
		the result may be transformed bounds.
	*/
	virtual void QueryBoundingBox( grinliz::Rectangle2I* boundingBox, int window ) = 0;

	/**	Check to see if this object collides with another object.

		Only Sprites and Canvases can collide. (Boxes, Tiles, etc. are ignored.)

		Composite Objects -- TextBox and Widgets -- are built on Sprites.
		Their components *will* collide. Something to be aware of.

		This is a pixel perfect (or near perfect, if scaled) 2D collision
		detection. 2D collision detection is a necessary -- but not
		sufficient -- collision criteria for 3D collision, if you are
		using an isometric view. For top down or side views, this is a
		complete detection scheme.

		See KrImageTree::CheckChildCollision and KrImageTree::CheckAllCollision
		for possibily more useful Tree variants of this call.

		The ImageTree must be in an consistent state in order to for collision
		checking to work. The function Engine()->Tree()->Walk() will bring the
		tree into a consistent state. The following operations will invalidate
		the state of the Tree, causing collision checking to fail:

			- Setting any X, Y, or Scale property
			- Adding anything to the Tree. (Deleting from the tree is safe,
			  however.)

		Walk can be a somewhat expensive call. And you should avoid calling it
		more than once per frame. A normal sequence of events would be something like:

			- Move sprites, canvases, etc. Game & logic actions.
			- Call Walk
			- Check collisions. Call deletes as you go, queue up additional
			  move and scaling actions.
			- Apply queued move / scaling actions.
			- Draw()

		Note that if you aren't using collision detection, you should never need
		to call Walk() directly. Also, if you check collisions *after* Draw(), but
		before any x/y/scale transformations, you don't need to call Walk().

		Returns true if the objects collide.

		A window can be specified. Depending on the transformations appiled,
		collisions can be different per window.

		Collisions with scaled sprites and canvases is supported with one caveat:
		un-cached sprites can not be collision detected. For a full discussion of
		scaling, see the HTML docs. In brief, a scaled image can be cached
		(pre-calculated at a give x and y scale) or real time scaled. Real time
		scaled sprites can not be used in a collision test.
	*/
	bool CheckCollision( KrImage* other, int window = 0 );

	virtual KrResource* Resource() = 0;

	// -- Internal: -- //
	virtual KrImage* ToImage()						{ return this; }

	virtual void	FlushInvalid( int window, bool cache );
	virtual void	CalcTransform( int window ) = 0;

	virtual void	AddedtoTree();
	virtual void	LeavingTree();

  protected:
	// Was the image visibile (IsVisible) at the last draw?
	// Used for dirty rectangle management.
	bool wasVisibleAtLastFlush[ KR_MAX_WINDOWS ];
//	KrRect	cacheRect[ KR_MAX_WINDOWS ];
//	bool	haveCacheRect[ KR_MAX_WINDOWS ];
};


#endif
