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
#include "spriteresource.h"
#include "SDL.h"
#include "kyraresource.h"
#include "SDL_endian.h"
#include "tileresource.h"
#include "fontresource.h"
#include "dataresource.h"
#include "../../grinliz/gldebug.h"
#include "../../grinliz/glmemorypool.h"

using namespace grinliz;

KrResourceFactory::KrResourceFactory()
{}

KrResourceFactory::~KrResourceFactory()
{
	instance = 0;
}

KrResourceFactory* KrResourceFactory::instance = 0;

KrResourceFactory* KrResourceFactory::Instance()
{
	if ( !instance )
	{
		instance = new KrResourceFactory;
	}
	return instance;
}


KrResource*	KrResourceFactory::Create(	U32 id, 
										U32 size, 
										SDL_RWops* data )
{
	switch( id )
	{
		case KYRATAG_SPRITE:
		{
			return new KrSpriteResource( size, data );
		}
		break;

		case KYRATAG_TILE:
		{
			return new KrTileResource( size, data );
		}
		break;

		case KYRATAG_FONT:
		{
			return new KrFontResource( size, data );
		}

		case KYRATAG_TEXTDATA:
		{
			return new KrTextDataResource( size, data );
		}

		case KYRATAG_BINARYDATA:
		{
			return new KrBinaryDataResource( size, data );
		}
	}

	GLOUTPUT(( "Warning: could not find resource id=%d in KrResourceFactory\n", id ));
	GLASSERT( 0 );
	return 0;
}


/////////////////////////////// KrCollisionMap


KrCollisionMap::KrCollisionMap( GlFixed _xScale, GlFixed _yScale, int width, int height )
{
	cx = (width  + 31) / 32;
	cy = height;
	map = new U32[ cx * cy ];
	::memset( map, 0, sizeof( U32 ) * cx * cy );
	xScale = _xScale;
	yScale = _yScale;
}


bool KrCollisionMap::Collide( int offsetX, 
							  int offsetY, 
							  const Rectangle2I& boundsIntersect,
							  KrCollisionMap* other )
{
	GLASSERT( offsetX >= 0 );

	int dx = offsetX / 32;			// words to the start of other
	int bitShift = offsetX % 32;	// bits to the start of other

	// Words to check is sort of tricky. It's not just a width,
	// but the span of the pixels over the bit map.
	int wordWidth =   ( offsetX + boundsIntersect.Width() - 1 ) / 32	// high word bound
					- ( offsetX ) / 32									// low word bound == dx
					+ 1;												// make width

	// ...but the wordwidth is never larget than the width of this or other.
	wordWidth = Min(	wordWidth,		// the max it could be
						Min(	cx,					// this bit width
								other->cx + 1 ) );	// other bit width: which can span 2 words of this. The left and right checks.
						
	
	int height = boundsIntersect.Height();

	int otherStartRow = 0;
	int thisStartRow = 0;
	if ( offsetY < 0 )
	{
		otherStartRow = -offsetY;
	}
	else
	{
		thisStartRow = offsetY;
	}

	for( int j=0; j<height; ++j )
	{
		// The other -- since it is to the right -- always start at its right edge.
		// index 0. This starts at the dx offset.
		U32* otherWord = other->GetRow( j + otherStartRow );
		U32* thisWord  = GetRow( thisStartRow + j ) + dx;

		for( int i=0; i<wordWidth; ++i )
		{
			// Check to left.
			if ( i > 0 )			// make sure we can check left.
			{
				// We are comparing the left word of other. So the left
				// lowest bits match with this' highest.
				if ( thisWord[i] & ( otherWord[ i-1 ] << ( 32 - bitShift ) ) )
					return true;
			}
			// Check to the right. 
			if ( i < other->cx )	// make sure there is a right to check.
			{
				// The highest bits of other's right
				// will match with our lower bits, so shift the other direction.
				if ( thisWord[i] & ( otherWord[ i ] >> ( bitShift ) ) )
					return true;
			}
		}
	}
	return false;
}
