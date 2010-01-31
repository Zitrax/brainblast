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

#include "SDL_endian.h"

#include "action.h"
#include "sdlutil.h"
#include "canvasresource.h"
#include "kyrabuild.h"
#include "encoder.h"

using namespace grinliz;

KrAction::KrAction( SDL_RWops* data )
{
	ReadString( data, &name );
	id = SDL_ReadLE32( data );
	
	numFrames = SDL_ReadLE32( data );
 	frame     = new KrRle[ numFrames ];

	GLOUTPUT(( "Action '%s' id=%d numFrames=%d\n", name.c_str(), id, numFrames ));

	for( int i=0; i < numFrames; i++ )
	{
		frame[ i ].Read( data );
	}
}


KrAction::KrAction( const std::string& _name )
{
	name = _name;
	numFrames = 0;
	frame = 0;
	id = 0;
}

KrAction::~KrAction()
{
	FreeScaleCache();
	delete [] frame;
}


void KrAction::CalculateBounds( int iframe, const KrMatrix2& matrix, Rectangle2I* bounds )
{
	if ( matrix.IsScaled() )
	{
        KrMatrix2 m;
		for( unsigned i=0; i<cache.size(); ++i )
		{
			if (    cache[i].xScale == matrix.xScale
			     && cache[i].yScale == matrix.yScale )
			{
				m = matrix;
				m.xScale = 1;
				m.yScale = 1;
				cache[i].frame[iframe]->CalculateBounds( m, bounds );
				return;
			}
		}
		#ifdef ASSERT_IF_NOT_CACHED
			GLASSERT( 0 );
		#endif
	}
	frame[iframe].CalculateBounds( matrix, bounds );
}


void KrAction::Draw(	KrPaintInfo* paintInfo,
						int iframe,
						const KrMatrix2& matrix,
					    const KrColorTransform& cForm,	// color transform to use
						const Rectangle2I& clip )
{
	// This happens if an empty action (often "NONE" gets exported.)
	if ( NumFrames() == 0 ) return;

	if (	matrix.IsScaled() 
		 && !paintInfo->OpenGL() )		// Scaling is trivial for openGL
	{
		KrMatrix2 m;
		for( unsigned i=0; i<cache.size(); ++i )
		{
			if ( cache[i].xScale == matrix.xScale && cache[i].yScale == matrix.yScale )
			{
				m = matrix;
				m.xScale = 1;
				m.yScale = 1;
				cache[i].frame[iframe]->Draw( paintInfo, m, cForm, clip );
				return;
			}
		}
		#ifdef ASSERT_IF_NOT_CACHED
			GLASSERT( 0 );
		#endif
	}
	frame[iframe].Draw( paintInfo, matrix, cForm, clip );
}


bool KrAction::HitTestTransformed( int iframe, int x, int y, int hitFlags )
{
	GLASSERT(iframe >=0 && iframe < numFrames );
	return frame[ iframe ].HitTestTransformed( x, y, hitFlags );
}


KrCanvasResource* KrAction::CreateCanvasResource( int frameIndex, int* hotx, int* hoty )
{
	GLASSERT( frameIndex >= 0 && frameIndex < numFrames );
	return frame[ frameIndex ].CreateCanvasResource( hotx, hoty );
}


KrCollisionMap* KrAction::GetCollisionMap( GlFixed xScale, GlFixed yScale, int index )
{
	GLASSERT( index >=0 && index<numFrames );

	unsigned i;
	// Check for existing. We have established that this is a scale that exists.
	if ( xScale == 1 && yScale == 1 )
	{
		return frame[index].GetCollisionMap( xScale, yScale );
	}
	else
	{
		// It absolutely should be in the cache. We won't have gotten here if it
		// isn't.
		for( i=0; i<cache.size(); ++i )
		{
			if ( cache[i].xScale == xScale && cache[i].yScale == yScale )
			{
				return cache[i].frame[index]->GetCollisionMap( xScale, yScale );
			}
		}
	}
	// Should have been 1:1 or in the cache.
	GLASSERT( 0 );
	return 0;
}


void KrAction::CacheScale( GlFixed xScale, GlFixed yScale )
{
	// check for pre-existing:
	unsigned i;

	for( i=0; i<cache.size(); ++i )
	{
		if ( cache[i].xScale == xScale && cache[i].yScale == yScale )
		{
			GLOUTPUT(( "Scale %f %f already cached in KrAction (used by sprite resource).\n", xScale.ToDouble(), yScale.ToDouble() ));
			return;
		}
	}

	// Push an empty one on the back:
	CachedBlock block;
	block.xScale = xScale;
	block.yScale = yScale;
	block.frame  = 0;
	cache.push_back( block );

	CachedBlock* pblock = &cache[ cache.size() - 1 ];
	pblock->frame = new KrRle*[ NumFrames() ];

	for( i=0; i<(unsigned) NumFrames(); ++i )
	{
		int hotx, hoty;
		pblock->frame[ i ] = frame[i].CreateScaledRle( xScale, yScale, &hotx, &hoty );
	}
}


bool KrAction::IsScaleCached( GlFixed xScale, GlFixed yScale )
{
	for( unsigned i=0; i<cache.size(); ++i )
	{
		if ( cache[i].xScale == xScale && cache[i].yScale == yScale )
		{	
			return true;
		}
	}
	return false;
}


void KrAction::FreeScaleCache()
{
	for( unsigned i=0; i<cache.size(); ++i )
	{
		for ( int j=0; j<NumFrames(); ++j )
		{
			delete cache[i].frame[j];
		}
		delete [] cache[i].frame;
	}
	cache.resize( 0 );
}


void KrAction::GrowFrameArray( int newSize )
{
	GLASSERT( newSize > numFrames );
	KrRle* newFrame = new KrRle[ newSize ];
	if ( newSize > 0 )
	{
		memset( newFrame, 0, newSize * sizeof( KrRle ) );
	}
	if ( numFrames > 0 )
	{
		memcpy( newFrame, frame, numFrames * sizeof( KrRle ) );
		memset( frame, 0, numFrames * sizeof( KrRle ) );
	}
	delete [] frame;
	frame = newFrame;
	numFrames = newSize;
}


void KrAction::Save( KrEncoder* encoder )
{
	WriteString( encoder->Stream(), name );
	encoder->WriteCached( name );
	SDL_WriteLE32( encoder->Stream(), numFrames );

	for ( int j=0; j<NumFrames(); ++j )
	{
		frame[j].Write( encoder->Stream() );

		U32 a = 0, b = 0, c = 0;
		frame[j].CountComponents( &a, &b, &c );
		encoder->AddCount( a, b, c );
	}
}
