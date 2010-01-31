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

#include "box.h"
#include "SDL.h"
#include "engine.h"

using namespace grinliz;

KrBox::KrBox( KrBoxResource* _resource )
{
	resource = _resource;
	owned = false;
}


KrBox::KrBox( int width, int height, KrRGBA color, int boxtype )
{
	resource = new KrBoxResource( "created", width, height, &color, 1, boxtype );
	owned = true;
}

KrBox::~KrBox()
{
	if ( owned )
		delete resource;
}


void KrBox::SetSize( int w, int h )
{
	if ( w == resource->Width() && h == resource->Height() )
		return;

	KrBoxResource* newRes = new KrBoxResource( "created by setsize", 
												w, h,
												resource->ColorArray(), 1,
												resource->BoxType() );
	if ( owned )
		delete resource;
	resource = newRes;
	owned = true;
}


void KrBox::SetBoxColor( KrRGBA color )
{
	KrBoxResource* newRes = new KrBoxResource( "created by setsize", 
												resource->Width(), resource->Height(),
												&color, 1,
												resource->BoxType() );
	if ( owned )
		delete resource;
	resource = newRes;
	owned = true;
}


void KrBox::SetType( int boxtype )
{
	if ( boxtype == resource->BoxType() )
		return;

	KrBoxResource* newRes = new KrBoxResource( "created by setsize", 
												resource->Width(), resource->Height(),
												resource->ColorArray(), 1,
												boxtype );
	if ( owned )
		delete resource;
	resource = newRes;
	owned = true;
}


void KrBox::Draw(	KrPaintInfo* paintInfo, 
					const Rectangle2I& clip,
					int win )
{
	GLASSERT( IsVisible( win ) );
	if ( bounds[win].Intersect( clip ) )
	{
		resource->Draw(	paintInfo,
						CompositeXForm(win),
						CompositeCForm(win),
						clip );
	}
}


void KrBox::CalcTransform( int win )
{
	// Calculate our new coordinates, and then bounding info.
	KrImNode::CalcTransform( win );

	resource->CalculateBounds( CompositeXForm( win ), &bounds[win] );
}


bool KrBox::HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int window )
{
	int i = window;

	if (    IsVisible( i )
		 && CompositeCForm(i).Alpha() != 0
		 && bounds[i].Intersect( x, y ) )
	{
		Vector2< GlFixed > object;
		ScreenToObject( x, y, &object, window );

		// Transform to local, and query the resource:
		if(	resource->HitTestTransformed(	object.x.ToIntRound(), object.y.ToIntRound(),
											flags ) )
		{
			results->push_back( this );
//			#ifdef DEBUG
//				GLOUTPUT( "HIT: Box %x\n", this );
//			#endif
			return true;
		}
	}
	return false;
}


void KrBox::QueryBoundingBox( Rectangle2I* boundingBox, int win )
{
	resource->CalculateBounds( CompositeXForm( win ), boundingBox );
}


KrImNode* KrBox::Clone()
{
	// make owned
	return new KrBox( resource->Width(), resource->Height(), *resource->ColorArray(), resource->BoxType() );
}

