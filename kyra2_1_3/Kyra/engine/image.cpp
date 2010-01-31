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

#include "image.h"
#include "SDL.h"
#include "engine.h"
#include "../../grinliz/glgeometry.h"

using namespace grinliz;


KrImage::KrImage() 
{
	for( int i=0; i<KR_MAX_WINDOWS; ++i )
	{
		wasVisibleAtLastFlush[i] = true;
//		haveCacheRect[i] = false;
	}
}


void KrImage::AddedtoTree()
{
	KrImNode::AddedtoTree();
	GLASSERT( bounds[0].IsValid() );
}


void KrImage::LeavingTree()
{
	GLASSERT( Engine() );
	for( int i=0; i<Engine()->NumWindows(); ++i )
	{
		if ( wasVisibleAtLastFlush[i] )
		{
			Engine()->DirtyRectangle(i)->AddRectangle( bounds[i] );
		}
	}
	KrImNode::LeavingTree();
}


void KrImage::FlushInvalid( int window, bool cache )
{
	// This whole cache mess takes advantage that the 2 rectangles generated
	// by a particular sprite will almost certainly overlap and can be treated
	// as one. Cache the first. When the 2nd comes in, if they intersect, union
	// together and add both. Else add seperately. Doesn't make much of a 
	// difference, pulling for now.
	//
//	if ( cache )
//	{
//		// Make sure our bounds get re-painted.
//		if ( wasVisibleAtLastFlush[window] || IsVisible(window) )
//		{
//			GLASSERT( haveCacheRect[window] == false );
//			haveCacheRect[window] = true;
//			cacheRect[window] = bounds[window];
//		}
//	}
//	else
//	{
//		if ( haveCacheRect[window] )
//		{
//			// Make sure our bounds get re-painted.
//			if ( wasVisibleAtLastFlush[window] || IsVisible(window) )
//			{
//				if ( cacheRect[window].Intersect( bounds[window] ) )
//				{
//					cacheRect[window].DoUnion( bounds[window] );
//					Engine()->DirtyRectangle(window)->AddRectangle( cacheRect[window] );
//				}
//				else
//				{
//					Engine()->DirtyRectangle(window)->AddRectangle( cacheRect[window] );
//					Engine()->DirtyRectangle(window)->AddRectangle( bounds[window] );
//				}
//			}
//			haveCacheRect[window] = false;
//		}
//		else
//		{
			// The entirety of "the old code."
			// Make sure our bounds get re-painted.
			//GLASSERT( bounds[window].IsValid() );
			bool vis = IsVisible(window);
			GLASSERT( !vis || bounds[window].IsValid() );

			if ( wasVisibleAtLastFlush[window] || vis )
			{
				Engine()->DirtyRectangle(window)->AddRectangle( bounds[window] );
				wasVisibleAtLastFlush[window] = vis;
			}
//		}
//	}
}


bool KrImage::CheckCollision( KrImage* other, int window )
{
	// Only sprites and canvases collide.
	if (    !( other->ToSprite() || other->ToCanvas() )
		 || !( this->ToSprite() || this->ToCanvas()   ) )
	{
		return false;
	}

	// If the bounding boxes don't overlap, they don't collide.
	if ( !this->Bounds( window ).Intersect( other->Bounds( window ) ) )
	{
		return false;
	}

	// Visiibily
	if ( !this->IsVisible( window ) || !other->IsVisible( window ) )
	{
		return false;
	}

	// Transparency
	if (    this->CompositeCForm( window ).Alpha() == 0
		 || other->CompositeCForm( window ).Alpha() == 0 )
	{
		return false;
	}

//	GLOUTPUT( "BoundingBoxPass: %s [%d,%d]-[%d,%d]\n",
//			  other->NodeName().c_str(),
//			  other->Bounds( 0 ).xmin,
//			  other->Bounds( 0 ).ymin,
//			  other->Bounds( 0 ).xmax,
//			  other->Bounds( 0 ).ymax );

	
	// Not colliding with text is a good idea -- but I fear I'll be chasing
	// special cases and widget behavior forever. So removing check.

	//	// Don't collide with text.
	//	if (	other->Parent() 
	//		 && other->Parent()->Parent()
	//		 && other->Parent()->Parent()->ToTextBox() )
	//	{
	//		return false;
	//	}
	//	if (	this->Parent()
	//		 &&	this->Parent()->Parent()
	//		 &&	this->Parent()->Parent()->ToTextBox() )
	//	{
	//		return false;
	//	}

	// The entire collision thing is base on a left-right model. Very important
	// to get that correct.
	KrImage* left  = 0;
	KrImage* right = 0;

	if ( other->Bounds( window ).min.x > this->Bounds( window ).min.x )
	{
		right = other;
		left  = this;
	}
	else
	{
		right = this;
		left = other;
	}

	KrCollisionMap *leftMap, *rightMap;

	leftMap  = left->Resource()->GetCollisionMap( left, window );
	rightMap = right->Resource()->GetCollisionMap( right, window );

	Rectangle2I isect = left->Bounds( window );
	isect.DoIntersection( right->Bounds( window ) );

	if ( leftMap && rightMap )
	{
		// Okay, we might actually collide after all that.
		return leftMap->Collide( right->Bounds( window ).min.x - left->Bounds( window ).min.x,
								 right->Bounds( window ).min.y - left->Bounds( window ).min.y,
								 isect,
								 rightMap );
				
	}
	return false;
}


