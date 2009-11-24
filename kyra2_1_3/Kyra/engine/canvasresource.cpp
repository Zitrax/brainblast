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

using namespace grinliz;


const std::string KrCanvasResource::canvasName = "Canvas";
int KrCanvasResource::canvasId = 0;

KrCanvasResource::KrCanvasResource(	const std::string& _name,
									int width,
									int height,
									int alphaSupport )
{
	pixelBlock.Create( width, height, alphaSupport !=0 );
	++canvasId;
	SetNameAndId( _name, canvasId );

	numClients = 0;
}


KrCanvasResource::~KrCanvasResource()
{
	GLASSERT( numClients == 0 );
	FreeScaleCache();

	for ( unsigned i=0; i<collisionMaps.size(); ++i )
		delete collisionMaps[i];
}


void KrCanvasResource::Draw(	KrPaintInfo* paintInfo,
								const KrMatrix2& matrix,
								const KrColorTransform& cForm,
								const Rectangle2I& clipping,
								int quality )
{
	pixelBlock.Draw(	paintInfo,
						matrix,
						false,
						cForm,
						clipping,
						quality );
}


void KrCanvasResource::Refresh()
{
	GlCircleListIterator<KrCanvas*> it( clients );
	for( it.Begin(); !it.Done(); it.Next() )
	{
		it.Current()->Invalidate( KR_ALL_WINDOWS );
	}
	pixelBlock.LoadNewTexture();

	// This trashes the collision maps, if alpha channel is used.
	if ( pixelBlock.Alpha() )
	{
		for ( unsigned i=0; i<collisionMaps.size(); ++i )
			delete collisionMaps[i];
		collisionMaps.resize( 0 );
	}
}


void KrCanvasResource::AddCanvas( KrCanvas* canvas )
{
	clients.PushBack( canvas );
	++numClients;
}


void KrCanvasResource::RemoveCanvas( KrCanvas* canvas )
{
	GlCircleListIterator<KrCanvas*> it( clients );

	#ifdef DEBUG	
		// check the list
		int count = 0;
		for( it.Begin(); !it.Done(); it.Next() )
			count++;
		GLASSERT( count == numClients );
	#endif
	--numClients;

	for( it.Begin(); !it.Done(); it.Next() )
	{
		if ( it.Current() == canvas )
		{
			it.Remove();
			return;
		}
	}
	// Remove failed!
	GLASSERT( 0 );
}


bool KrCanvasResource::HitTestTransformed( int x, int y, int hitFlags )
{
	if ( x >=0 && x < Width() && y >=0 && y < Height() )
	{
		KrRGBA* pixel = pixelBlock.Pixels() + x + y * pixelBlock.Width();

		if ( !pixelBlock.Alpha() )
		{
			return true;
		}

		if ( pixel->c.alpha != KrRGBA::KR_TRANSPARENT )
		{
			return true;
		}
	}
	return false;
}


void KrCanvasResource::CalculateBounds( const KrMatrix2& xForm, Rectangle2I* bounds ) const
{
	pixelBlock.CalculateBounds( xForm, bounds );
}


KrCollisionMap* KrCanvasResource::GetCollisionMap( KrImage* state, int window )
{
	// First, check for the existence of one that is cached already:
	unsigned i;
	int j;

	for( i=0; i<collisionMaps.size(); ++i )
	{
		if (	collisionMaps[i]->XScale() == state->XScale( window )
			 && collisionMaps[i]->YScale() == state->YScale( window ) )
		{
			// Match found.
			return collisionMaps[i];
		}
	}

	KrCollisionMap* map = 0;

	if ( pixelBlock.Alpha() )
	{
		KrRGBA *pixel	= 0;
		KrRGBA *memory  = 0;
		int     cx = 0, cy = 0;

		// Handle scaling. If not scale, then easy. Else generate the fast rendered version.
		if ( state->XScale( window ) == 1 && state->YScale( window ) == 1 )
		{
			pixel = pixelBlock.Pixels();
			cx = pixelBlock.Width();
			cy = pixelBlock.Height();
		}
		else
		{
			KrMatrix2 xForm;
			xForm.Set( 0, 0, state->XScale( window ), state->YScale( window ) );
			Rectangle2I bounds;
			CalculateBounds( xForm, &bounds );
			KrColorTransform cForm;

			memory = new KrRGBA[ bounds.Width() * bounds.Height() ];
			::memset( memory, 0, bounds.Width() * bounds.Height() * sizeof( KrRGBA ) );

			KrPaintInfo info( memory, bounds.Width(), bounds.Height() );
			Rectangle2I clip;
			clip.Set( 0, 0, bounds.Width() - 1, bounds.Height() - 1 );

			pixelBlock.DrawScaledFast( &info, xForm, cForm, clip, false );

			pixel = memory;
			cx = bounds.Width();
			cy = bounds.Height();
		}

		map = new KrCollisionMap( state->XScale( window ), state->YScale( window ), cx, cy );		// For alpha, we need to check each pixel to see if it is turned

		// on in the map or not.
		for( j=0; j<cy; ++j )
		{
			HighBitWriter<U32> writer( map->GetRow( j ) );
			KrRGBA* row = pixel + cx * j;

			for( i=0; i<(unsigned)cx; ++i )
			{
				if ( row[i].c.alpha == KrRGBA::KR_TRANSPARENT )
					writer.Skip();
				else
					writer.Push_1();
			}
		}
		delete [] memory;	// clean up the tempory buffer, if we used one.
	}
	else
	{
		// For no alpha, every pixel is turned on.
		KrMatrix2 xForm;
		xForm.Set( 0, 0, state->XScale( window ), state->YScale( window ) );
		Rectangle2I bounds;
		CalculateBounds( xForm, &bounds );
		map = new KrCollisionMap( state->XScale( window ), state->YScale( window ), bounds.Width(), bounds.Height() );

		for( j=0; j<bounds.Height(); ++j )
		{
			HighBitWriter<U32> writer( map->GetRow( j ) );
			writer.Push_1N( bounds.Width() );
		}
	}
	collisionMaps.push_back( map );
	return map;
}
