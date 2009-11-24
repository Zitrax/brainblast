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

#include "canvas.h"
#include "SDL.h"
#include "engine.h"
#include "../../grinliz/glgeometry.h"

using namespace grinliz;


KrCanvas::KrCanvas( KrCanvasResource* _resource )
{
	resource = _resource;
	resource->AddCanvas( this );
}


KrCanvas::~KrCanvas()
{
	resource->RemoveCanvas( this );
}


void KrCanvas::Draw(	KrPaintInfo* paintInfo, 
						const Rectangle2I& clip,
						int win )
{
	GLASSERT( IsVisible( win ) );
	GLASSERT( CompositeQuality( win ) != KrQualityNone );

	if ( bounds[win].Intersect( clip ) )
	{
		resource->Draw(	paintInfo,
						CompositeXForm(win),
						CompositeCForm(win),
						clip,
						CompositeQuality(win) );
	}
}


void KrCanvas::CalcTransform( int win )
{
	// Calculate our new coordinates, and then bounding info.
	KrImNode::CalcTransform( win );

	// Tiles don't have hotspots, so this is easier than the sprite.
	bounds[win].min.x = CompositeXForm(win).x.ToIntRound();
	bounds[win].min.y = CompositeXForm(win).y.ToIntRound();
	bounds[win].max.x = ( CompositeXForm(win).x + CompositeXForm(win).xScale * resource->Width()  ).ToIntRound() - 1;
	bounds[win].max.y = ( CompositeXForm(win).y + CompositeXForm(win).yScale * resource->Height() ).ToIntRound() - 1;
}


bool KrCanvas::HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int window )
{
	int i = window;

	if (    IsVisible( i )
		 && CompositeCForm(i).Alpha() != 0
		 && bounds[i].Intersect( x, y ) )
	{
		Vector2< GlFixed > object;
		ScreenToObject( x, y, &object );

		// Transform to local, and query the resource:
		if(	resource->HitTestTransformed(	object.x.ToIntRound(), object.y.ToIntRound(),
											flags ) )
		{
			results->push_back( this );
//			#ifdef DEBUG
//			GLOUTPUT( "HIT: Canvas %x (size=%dx%d)\n", this, Width(), Height() );
//			#endif
			return true;
		}
	}
	return false;
}


void KrCanvas::QueryBoundingBox( Rectangle2I* boundingBox, int window )
{
	resource->CalculateBounds( CompositeXForm( window ), boundingBox );
}


KrImNode* KrCanvas::Clone()
{
	return new KrCanvas( resource );
}
