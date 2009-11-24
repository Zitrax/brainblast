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
#include "tile.h"
#include "SDL.h"
#include "tileresource.h"
#include "engine.h"
#include "../../grinliz/glgeometry.h"

using namespace grinliz;

KrTile::KrTile( KrTileResource* _resource )
{
	resource = _resource;
	rotation = 0;
}


KrTile::~KrTile()
{}


void KrTile::Draw(	KrPaintInfo* paintInfo, const Rectangle2I& clip, int win )
{
	GLASSERT( IsVisible(win) );
	if ( bounds[win].Intersect( clip ) )
	{
		resource->Draw(	paintInfo,
						CompositeXForm(win),
						rotation,
						CompositeCForm(win),
						clip,
						CompositeQuality(win) );
	}
}


void KrTile::SetRotation( int _r )
{
	_r = _r & 7;
	if ( _r != rotation )
	{
		rotation = _r;
		Invalidate( KR_ALL_WINDOWS );
	}
}


void KrTile::CalcTransform( int win )
{
	// Calculate our new coordinates, and then bounding info.
	KrImNode::CalcTransform( win );

	if(resource == NULL)  
	{
		bounds[win].SetInvalid();
	}
	else  
	{
	resource->CalculateBounds( CompositeXForm(win), &bounds[win] );
	}
}


bool KrTile::HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int window )
{
	int i = window;
	if (    IsVisible( i )
		 && CompositeCForm(i).Alpha() != 0
		 && bounds[i].Intersect( x, y ) )
	{
		Vector2< GlFixed > object;
		ScreenToObject( x, y, &object, i );

		// Transform to local, and query the resource:
		if(	resource->HitTestTransformed(	rotation,
											object.y.ToIntRound(), object.y.ToIntRound(),
											flags ) )
		{
			results->push_back( this );
			return true;
		}
	}
	return false;
}


void KrTile::QueryBoundingBox( Rectangle2I* boundingBox, int win )
{
	// Tiles don't have hotspots, so this is easier than the sprite.
	// Make sure to look for scaling.
	resource->CalculateBounds( CompositeXForm( win ), boundingBox );
}


KrImNode* KrTile::Clone()
{
	KrTile* tile = new KrTile( resource );
	tile->SetRotation( rotation );
	return tile;
}

