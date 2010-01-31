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

#include <string.h>
#include "painter.h"
#include "SDL.h"
#ifdef KYRA_SUPPORT_OPENGL
	#include "SDL_opengl.h"
#endif
#include "rle.h"
#include "SDL_rwops.h"
#include "SDL_endian.h"
#include "../../grinliz/glutil.h"
#include "sdlutil.h"
#include "kyrabuild.h"
#include "canvasresource.h"
#include "engine.h"
#include "ogltexture.h"
#include "../../grinliz/glmemorypool.h"

using namespace grinliz;


//sanity checks... keep it real
static const int SANITY_SEGMENTS		= 256 * 8;
static const int SANITY_ORIGIN_OFFSET	= 256 * 16;
static const int SANITY_SCALE			= 256 * 4;
static const int SANITY_BOUNDS			= 256 * 50;

#if defined( DRAWDEBUG_RLE ) || defined( DRAWDEBUG_BLTRECTS )
	int debugFrameCounter = 0;
#endif

#ifdef DEBUG
	U32 KrRleSegment::numRGBA = 0;
#endif


void KrRleLine::DrawScaled(	U8* target, 
							KrPaintInfo* paintInfo,
							const Rectangle2I& bounds,
							const Rectangle2I& isect,
							const KrColorTransform& cForm,
							U32 stRatio )	// source / target
{
	GLASSERT( !paintInfo->OpenGL() );

	// Walk through all the segments, figure out where
	// they are and if they need to be drawn.

	int tx = bounds.min.x;
	S32 sErrorF = 0;
	int i = 0;

	// Draw each skip and run if in bounds.
	while ( tx <= isect.max.x && i < nSegments )
	{
		while ( (sErrorF>>16) < segment[i].Start() )
		{
			sErrorF += stRatio;
			++tx;
		}

		// Get a per-segment blitter
		KrPaintFunc blitter = paintInfo->GetBlitter( segment[i].Flags().IsSet( ALPHA ) ? true : false, cForm );
		GLASSERT( blitter );

		// Draw the run
		// Fixme: optimize. Pull things out of inner loop.
		while (    (sErrorF>>16) <= segment[i].End()
				&& tx <= isect.max.x )
		{
			if ( tx >= isect.min.x )
			{
				GLASSERT( tx >= bounds.min.x && tx <= bounds.max.x );
				GLASSERT( tx >= isect.min.x  && tx <= isect.max.x );

				blitter(	paintInfo, 
							target + tx * paintInfo->bytesPerPixel, 
							segment[i].RGBA() + (sErrorF>>16) - segment[i].Start(), 
							1, 
							cForm );
			}
			sErrorF += stRatio;
			++tx;
		}
		++i;
	}
}


void KrRleLine::CalcRange( KrRGBA* minColor, KrRGBA* maxColor )
{
	for( int i=0; i<nSegments; ++i )
	{
		segment[i].CalcRange( minColor, maxColor );
	}
}


// -------------- Segment ------------------
KrRleSegment::~KrRleSegment()
{ 
	if ( ! ( flags.IsSet(  MEMORYPOOL ) ) )
		delete [] rgba; 
}


bool KrRleSegment::Create( KrPaintInfo* surface, 
						   int x, int xMax, int y, int objectOriginX )
{
	// Use surface coordinates until the very end, where the
	// objectOriginX is used to adjest to object coords.

	KrPainter painter( surface );
	bool alpha = false;
	U8 r, g, b, a;

	start = end = x;

	// calculate the length
	while ( end < xMax )
	{
		painter.BreakPixel( end, y, &r, &g, &b, &a );
		if ( a == 0 )
			break;
		if ( a < 255 )
			alpha = true;
		++end;
	}
	// End is an index, not a length. The above loop will
	// always go by once, after all:
	--end;

	if ( alpha )
	{
		flags.Set( ALPHA );
	}
	//GLASSERT( length > 0 );
	rgba = new KrRGBA[ end - start + 1 ];

	start -= objectOriginX;
	end   -= objectOriginX;

	for ( int i=0; i<Len(); ++i )
	{
		painter.BreakPixel( x + i, y, &r, &g, &b, &a );
		rgba[i].c.alpha = a;
		rgba[i].c.red   = r;
		rgba[i].c.green = g;
		rgba[i].c.blue  = b;
	}

	return true;
}


bool KrRleSegment::Write(	GlWriteBitStream* writer, 
							KrRGBA minColor,
							KrRGBA bits )
{
	// Put compression where it counts: in the 
	// most common types. The segment and line.
	//GLASSERT( end - start < 0 );
//	GlFlag<U32> lowerFlags = flags;
//
//	if (	start < 256 
//		 && end   < 256 )
//	{
//		lowerFlags.Set( COMPRESS8BIT );
//	}

	//WriteByte( fp, lowerFlags.ToU32() );
	writer->WriteBits( flags.ToU32(), BITS_USED );
	
	int bitsInRange = GlBitStream::BitsNeeded( Max( start, end ) );
	GLASSERT( bitsInRange < 0x10000 );
	// The range is from 0 - 64k. bitsInRange will be from 0-15. 
	// Therefore 4 bits are needed to store the range.
	writer->WriteBits( bitsInRange, 4 ); 
	writer->WriteBits( start, bitsInRange );
	writer->WriteBits( end, bitsInRange );

//	if ( lowerFlags.IsSet( COMPRESS8BIT ) )
//	{
//		WriteByte( fp, start );
//		WriteByte( fp, end );
//	}
//	else
//	{	
//		SDL_WriteLE16( fp, start );
//		SDL_WriteLE16( fp, end );
//	}
			
	for ( int i=0; i<Len(); ++i )
	{
//		SDL_RWwrite( fp, &rgba[i].c.red,   1, 1);
//		SDL_RWwrite( fp, &rgba[i].c.green, 1, 1);
//		SDL_RWwrite( fp, &rgba[i].c.blue,  1, 1);
//		if ( flags.IsSet( ALPHA ) )
//			SDL_RWwrite( fp, &rgba[i].c.alpha, 1, 1);
		writer->WriteBits( rgba[i].c.red   - minColor.c.red,   bits.c.red );
		writer->WriteBits( rgba[i].c.green - minColor.c.green, bits.c.green );
		writer->WriteBits( rgba[i].c.blue  - minColor.c.blue,  bits.c.blue );
		writer->WriteBits( rgba[i].c.alpha - minColor.c.alpha, bits.c.alpha );
	}
	return true;
}


bool KrRleSegment::Read(	GlReadBitStream* reader, 
							KrRGBA minColor,
							KrRGBA bits )
{
//	flags.FromU32( ReadByte( data ) );
	flags.FromU32( reader->ReadBits( BITS_USED ) );

//	if ( flags.IsSet( COMPRESS8BIT ) )
//	{
//		flags.Clear( COMPRESS8BIT );
//		start = ReadByte( data );
//		end   = ReadByte( data );
//	}
//	else
//	{
//		start  = SDL_ReadLE16( data );
//		end    = SDL_ReadLE16( data );
//	}

	int bitsInRange = reader->ReadBits( 4 );
	start = reader->ReadBits( bitsInRange );
	end   = reader->ReadBits( bitsInRange );

	GLASSERT( Len() > 0 );
	GLASSERT( Len() < 2000 );	// not true, but good reality check

	// Allocate the RGBA. Use a memory pool, if available.
	rgba = 0;
	if ( KrRle::memoryPoolRGBA )
	{
		rgba = (KrRGBA*) KrRle::memoryPoolRGBA->Alloc( Len() * sizeof( KrRGBA ) );
		flags.Set( MEMORYPOOL );
	}
	if ( !rgba )
	{
		rgba = new KrRGBA[ Len() ];
	}
	#ifdef DEBUG
		numRGBA += Len();
	#endif

	for ( int i=0; i<Len(); ++i )
	{
//		SDL_RWread( data, &rgba[i].c.red,   1, 1);
//		SDL_RWread( data, &rgba[i].c.green, 1, 1);
//		SDL_RWread( data, &rgba[i].c.blue,  1, 1);
//
//		if ( flags.IsSet( ALPHA ) )
//			SDL_RWread( data, &rgba[i].c.alpha, 1, 1);
//		else
//			rgba[i].c.alpha = 255;
		rgba[i].c.red   = minColor.c.red   + reader->ReadBits( bits.c.red );
		rgba[i].c.green = minColor.c.green + reader->ReadBits( bits.c.green );
		rgba[i].c.blue  = minColor.c.blue  + reader->ReadBits( bits.c.blue );
		rgba[i].c.alpha = minColor.c.alpha + reader->ReadBits( bits.c.alpha );
	}

	return true;
}


void KrRleSegment::CalcRange( KrRGBA* minColor, KrRGBA* maxColor )
{
	int length = Len();
	for( int i=0; i<length; ++i )
	{
		for( int j=0; j<4; ++j )
		{
			minColor->array[j] = Min( minColor->array[j], rgba[i].array[j] );
			maxColor->array[j] = Max( maxColor->array[j], rgba[i].array[j] );
		}
	}
}

// -------------- Line ------------------
bool KrRleLine::Create( KrPaintInfo* surface, int x, int y, int w )
{
	const int MAX_SEG = 256;
	GLASSERT( nSegments == 0 );
	KrPainter painter( surface );

	// Assume we have less than MAX_SEG segments:
	segment = new KrRleSegment[ MAX_SEG ];
	int currentX = x;
	int xmax = x + w - 1;

	// Check for non-transparent pixels, and create a line if needed.
	while ( currentX < x + w )
	{
		int skip = painter.CalcTransparentRun( currentX, xmax, y );
		currentX += skip;

		if ( currentX < x + w )
		{
			segment[nSegments].Create( surface, currentX, x+w, y, x );

			if ( segment[nSegments].Alpha() )
			{
				flags.Set( ALPHA );
			}

			currentX += segment[nSegments].Len();
			nSegments++;
		}
	}
	// completely arbitrary check:
	GLASSERT( nSegments >= 0 && nSegments < 200 );
	return true;
}


bool KrRleLine::Write( GlWriteBitStream* writer, KrRGBA minColor, KrRGBA bits )
{
// 	KrOUTPUT( "  Line flags=%d nSegments=%d\n",
// 			  flags, nSegments );

//	GlFlag<U32> lowerFlags = flags;
//	if ( nSegments < 256 )
//		lowerFlags.Set( COMPRESS8BIT );
//	WriteByte( fp, lowerFlags.ToU32() );
//
//	if ( lowerFlags.IsSet( COMPRESS8BIT ) )
//		WriteByte( fp, nSegments );
//	else
//		SDL_WriteLE16( fp, nSegments );

	GLASSERT( nSegments >= 0 );

	writer->WriteBits( flags.ToU32(), BITS_USED );
	
	int bitsNeeded = GlBitStream::BitsNeeded( nSegments );
	// There can be 64k segments. 0-15 bits. We need 4 bits to represent that range.
	writer->WriteBits( bitsNeeded, 4 );
	writer->WriteBits( nSegments, bitsNeeded );

	for ( int i=0; i<nSegments; i++ )
	{
		segment[i].Write( writer, minColor, bits );
	}
	return true;
}


bool KrRleLine::Read( GlReadBitStream* reader, KrRGBA minColor, KrRGBA bits )
{
//	flags.FromU32( ReadByte( data ) );
// 	int is8Bit = ( flags & COMPRESS8BIT );

//	if ( flags.IsSet( COMPRESS8BIT ) )
//	{
//		flags.Clear( COMPRESS8BIT );
//		nSegments = ReadByte( data );
//	}
//	else
//	{
//		nSegments = SDL_ReadLE16( data );
//	}

	flags.FromU32( reader->ReadBits( BITS_USED ) );
	int bitsNeeded = reader->ReadBits( 4 );
	nSegments = reader->ReadBits( bitsNeeded );

	// Lines can have no segments: any blank line.
	GLASSERT( nSegments >= 0 );
	GLASSERT( nSegments < SANITY_SEGMENTS );	// sanity check

	if ( nSegments > 0 )
	{
		if ( KrRle::memoryPoolSegment )
		{
			segment = (KrRleSegment*) KrRle::memoryPoolSegment->Alloc( nSegments * sizeof( KrRleSegment ) );
			flags.Set( MEMORYPOOL );
		}
		else		
		{
			segment = new KrRleSegment[ nSegments ];
		}

		for ( int i=0; i<nSegments; i++ )
		{
			segment[i].Clear();
			segment[i].Read( reader, minColor, bits );
		}
	}
	return true;
}


// -------------- Rle ------------------

grinliz::LinearMemoryPool*	KrRle::memoryPoolRGBA    = 0;
grinliz::LinearMemoryPool*	KrRle::memoryPoolLine    = 0;
grinliz::LinearMemoryPool*	KrRle::memoryPoolSegment = 0;


KrRle::~KrRle()
{
	if ( !flags.IsSet( MEMORYPOOL ) )
		delete [] line; 
	delete texture; 
	delete collisionMap;
}


bool KrRle::Create(	KrPaintInfo* surface, 
					int x, int y, 
					int width, int height,
					int hotspotX, int hotspotY,
					int deltax, int deltay )
{
	KrPainter painter( surface );

	#ifdef DEBUG
		// Check our inputs:
		GLASSERT( x >=0 );
		GLASSERT( x + width - 1 < surface->width );
		GLASSERT( y >=0 );
		GLASSERT( y + height - 1 < surface->height );
	#endif

	int j;
	GLASSERT( line == 0 );
	delta.x = deltax;
	delta.y = deltay;

	int minX = 0;
	int maxX = 0;
	int minY = 0;
	int maxY = 0;
	int calcWidth = 0, calcHeight = 0;
	bool empty = true;
	
	// Start by finding the bounding box for this Rle.
	for( minX = x; minX < x + width; ++minX )
	{
		// If the column is not transparent, stop looking.
		if ( height != painter.CalcTransparentColumn( y, y+height-1, minX )	 )
			break;
	}

	if ( minX != x + width )
	{
		// Not empty. (Happens -- often -- with fonts.)
		empty = false;
		for( maxX = x + width - 1; maxX >= minX; --maxX )
		{
			if ( height != painter.CalcTransparentColumn( y, y+height-1, maxX )	 )
				break;
		}
		GLASSERT( maxX >= minX );
		calcWidth = maxX - minX + 1;

		for( minY = y; minY < y + height; ++minY )
		{
			if ( calcWidth != painter.CalcTransparentRun( minX, maxX, minY ) )
				break;
		}
		GLASSERT( minY < y + height );

		for( maxY = y + height - 1; maxY >= minY; --maxY )
		{
			if ( calcWidth != painter.CalcTransparentRun( minX, maxX, maxY ) )
				break;
		}
		GLASSERT( maxY >= minY );
		calcHeight = maxY - minY + 1;
	}

	if ( !empty )
	{
		// Allocate a line for each y pixel. We may
		// not use them all.
		line = new KrRleLine[ calcHeight ];

		for ( j=0; j<calcHeight; j++ )
		{
			// We have an RLE line.
			line[j].Create( surface, minX, minY + j, calcWidth );

			// Set our affected flags:
			if ( line[j].Alpha() )
				flags.Set( ALPHA );
		}
	}

	
	// We have the delta from the hotspot to the user origin -
	// adjust for the caculated origin.
	deltaHotToOrigin.x = minX - hotspotX;
	deltaHotToOrigin.y = minY - hotspotY;

	GLASSERT( deltaHotToOrigin.x > -SANITY_ORIGIN_OFFSET );	// sanity
	GLASSERT( deltaHotToOrigin.y > -SANITY_ORIGIN_OFFSET );	// sanity
	GLASSERT( deltaHotToOrigin.x < SANITY_ORIGIN_OFFSET );	// sanity
	GLASSERT( deltaHotToOrigin.y < SANITY_ORIGIN_OFFSET );	// sanity

	if ( empty )
	{
		size.x = 0;
		size.y = 0;
	}
	else
	{
		size.x = calcWidth;
		size.y = calcHeight;

		#ifdef DEBUG

		int mins, maxs;
		for( int j=0; j<calcHeight; ++j )
		{
			for( int i=0; i<line[j].nSegments; ++i )
			{
				if ( i == 0 && j == 0 )
				{
					mins = line[j].segment[i].start;
					maxs = line[j].segment[i].end;
				}
				else
				{
					mins = Min( mins, (int) line[j].segment[i].start );
					maxs = Max( maxs, (int) line[j].segment[i].end );
				}
			}
		}
		GLASSERT( maxs - mins + 1 == calcWidth );

		#endif
	}

	return ( !empty );
}


bool KrRle::Write( SDL_RWops* fp )
{
//	#ifdef DEBUG
//		GLOUTPUT( "Writing RLE: ("  );
//		if ( flags.IsSet( ALPHA ) )
//			GLOUTPUT( "ALPHA " );
//	#endif

	GLASSERT( deltaHotToOrigin.x > -SANITY_ORIGIN_OFFSET );	// sanity
	GLASSERT( deltaHotToOrigin.y > -SANITY_ORIGIN_OFFSET );	// sanity
	GLASSERT( deltaHotToOrigin.x < SANITY_ORIGIN_OFFSET );	// sanity
	GLASSERT( deltaHotToOrigin.y < SANITY_ORIGIN_OFFSET );	// sanity

	SDL_WriteLE16( fp, flags.ToU32() );
	SDL_WriteLE32( fp, deltaHotToOrigin.x );
	SDL_WriteLE32( fp, deltaHotToOrigin.y );
	SDL_WriteLE32( fp, size.x );
	SDL_WriteLE32( fp, size.y );
	SDL_WriteLE32( fp, delta.x );
	SDL_WriteLE32( fp, delta.y );

	// Get the ranges of the image data:
	KrRGBA minColor, maxColor, bits;
	int i;
	if ( size.y > 0 )
	{
		minColor.Set( 255, 255, 255, 255 );
		maxColor.Set( 0, 0, 0, 0 );
		for( i=0; i<size.y; ++i )
		{
			line[i].CalcRange( &minColor, &maxColor );
		}
	}
	else
	{
		// (#*%(@ empty font sprites
		minColor.Set( 0, 0, 0, 0 );
		maxColor.Set( 0, 0, 0, 0 );
	}

	GLASSERT( minColor.c.red <= maxColor.c.red );
	GLASSERT( minColor.c.green <= maxColor.c.green );
	GLASSERT( minColor.c.blue <= maxColor.c.blue );
	GLASSERT( minColor.c.alpha <= maxColor.c.alpha );

	// Write min and bits needed for each one:
	WriteByte( fp, minColor.c.red );
	bits.c.red = GlBitStream::BitsNeeded( maxColor.c.red - minColor.c.red );
	WriteByte( fp, bits.c.red );

	WriteByte( fp, minColor.c.green );
	bits.c.green = GlBitStream::BitsNeeded( maxColor.c.green - minColor.c.green );
	WriteByte( fp, bits.c.green );

	WriteByte( fp, minColor.c.blue );
	bits.c.blue = GlBitStream::BitsNeeded( maxColor.c.blue - minColor.c.blue );
	WriteByte( fp, bits.c.blue );

	WriteByte( fp, minColor.c.alpha );
	bits.c.alpha = GlBitStream::BitsNeeded( maxColor.c.alpha - minColor.c.alpha );
	WriteByte( fp, bits.c.alpha );

	GlWriteBitStream writer( fp );

	for ( i=0; i<size.y; i++ )
	{
		line[i].Write( &writer, minColor, bits );
	}

	writer.Flush();

//	#ifdef DEBUG
//		GLOUTPUT( ")\n size cx=%d cy=%d\n",
//				  size.x, size.y );
//	#endif
	return true;
}


bool KrRle::Read( SDL_RWops* data )
{
//	#ifdef DEBUG
//	int minx = 100;	// just for error checking - should go to 0.
//	#endif

	flags.FromU32( SDL_ReadLE16( data ) );
	deltaHotToOrigin.x = SDL_ReadLE32( data );
	deltaHotToOrigin.y = SDL_ReadLE32( data );
	size.x = SDL_ReadLE32( data );
	size.y = SDL_ReadLE32( data );
	delta.x = SDL_ReadLE32( data );
	delta.y = SDL_ReadLE32( data );

	GLASSERT( InRange(  deltaHotToOrigin.x, -SANITY_ORIGIN_OFFSET, SANITY_ORIGIN_OFFSET ) );	// sanity
	GLASSERT( InRange(  deltaHotToOrigin.y, -SANITY_ORIGIN_OFFSET, SANITY_ORIGIN_OFFSET ) );	// sanity
	GLASSERT( InRange(  size.x, 0, SANITY_ORIGIN_OFFSET ) );				// sanity
	GLASSERT( InRange(  size.y, 0, SANITY_ORIGIN_OFFSET ) );				// sanity

	KrRGBA minColor, bits;
	minColor.c.red = ReadByte( data );
	bits.c.red = ReadByte( data );
	minColor.c.green = ReadByte( data );
	bits.c.green = ReadByte( data );
	minColor.c.blue = ReadByte( data );
	bits.c.blue = ReadByte( data );
	minColor.c.alpha = ReadByte( data );
	bits.c.alpha = ReadByte( data );

	GlReadBitStream reader( data );

	// We get 0 sizes for fonts, so be careful.
	if ( size.y > 0 )
	{
		if ( memoryPoolLine )
		{
			line = (KrRleLine*) memoryPoolLine->Alloc( sizeof( KrRleLine ) * size.y );
			flags.Set( MEMORYPOOL );
		}
		else
		{
			line = new KrRleLine[ size.y ];
		}

		for ( int i=0; i<size.y; i++ )
		{
			line[i].Clear();
			line[i].Read( &reader, minColor, bits );
//			#ifdef DEBUG
//			if ( line[i].NumSegments() > 0 && line[i].Segment( 0 )->Skip() < minx )
//				minx = line[i].Segment( 0 )->Skip();
//			#endif
			GLASSERT( line[i].CalcSize() <= size.x );
		}
//		GLASSERT( minx == 0 );
	}
	else
	{
		line = 0;
	}

	reader.Flush();
	//GLOUTPUT( "RLE: alpha=%d\n", Alpha() );
	return ( size.y > 0 );
}


void KrRle::CalculateBounds( const KrMatrix2& xForm, Rectangle2I* bounds ) const
{
	GLASSERT( xForm.xScale >= 0 );
	GLASSERT( xForm.xScale < SANITY_SCALE );	// sanity check
	GLASSERT( xForm.yScale >= 0 );
	GLASSERT( xForm.yScale < SANITY_SCALE );	// sanity check

	if ( xForm.IsScaled() )
	{
		S32 xmin = GlFixedMult( xForm.xScale.v, GlIntToFixed( deltaHotToOrigin.x ) );
		S32 ymin = GlFixedMult( xForm.yScale.v, GlIntToFixed( deltaHotToOrigin.y ) );

		bounds->min.x = GlFixedToIntRound( xmin + xForm.x.v );
		bounds->min.y = GlFixedToIntRound( ymin + xForm.y.v );
//		bounds->max.x = bounds->min.x + GlFixedToIntRound( GlFixedMult( xForm.xScale.v, GlIntToFixed( size.x )) ) - 1;
//		bounds->ymax = bounds->ymin + GlFixedToIntRound( GlFixedMult( xForm.yScale.v, GlIntToFixed( size.y )) ) - 1;

		bounds->max.x = GlFixedToIntRound( xForm.x.v + xmin
                                          + GlFixedMult( xForm.xScale.v, GlIntToFixed( size.x ))) - 1;
		bounds->max.y = GlFixedToIntRound( xForm.y.v + ymin
                                          + GlFixedMult( xForm.yScale.v, GlIntToFixed( size.y ))) - 1;

    }
	else
	{
		bounds->min.x = deltaHotToOrigin.x + GlFixedToInt( xForm.x.v );
		bounds->min.y = deltaHotToOrigin.y + GlFixedToInt( xForm.y.v );
		bounds->max.x = bounds->min.x + size.x - 1;
		bounds->max.y = bounds->min.y + size.y - 1;
	}

	#ifdef DEBUG
		// Can be invalid in the encoder.
		//GLASSERT( bounds->IsValid() );

		if ( xForm.xScale == 1 && xForm.yScale == 1 )
		{
			GLASSERT( bounds->Height() == size.y );
			GLASSERT( bounds->Width()  == size.x );
		}
		else
		{
			GLASSERT( bounds->Width() <= ( xForm.xScale * size.x ).ToIntRoundUp() );
			GLASSERT( bounds->Width() >= ( xForm.xScale * size.x ).ToInt() );
			GLASSERT( bounds->Height()<= ( xForm.yScale * size.y ).ToIntRoundUp() );
			GLASSERT( bounds->Height()>= ( xForm.yScale * size.y ).ToInt() );
		}
		GLASSERT( bounds->min.x > -SANITY_BOUNDS );	// sanity
		GLASSERT( bounds->min.y > -SANITY_BOUNDS );	// sanity
		GLASSERT( bounds->max.x < SANITY_BOUNDS );	// sanity
		GLASSERT( bounds->max.y < SANITY_BOUNDS );	// sanity

	#endif
}


void KrRle::DrawOpenGL( KrPaintInfo* paintInfo,
						const KrMatrix2& xForm,
						const KrColorTransform& cForm,
						const Rectangle2I& clipping )
{
	#ifdef KYRA_SUPPORT_OPENGL
	if ( !texture )
	{
		KrTextureManager* manager = KrTextureManager::Instance();
		if ( manager )
		{
			// Create a canvas resource with the data we need.
			int hotx, hoty;
			KrCanvasResource* resource = CreateCanvasResource( &hotx, &hoty );
			texture = manager->CreateTexture( resource->Pixels(), resource->Width(), resource->Height() );
			delete resource;
		}
	}

	GLASSERT( texture );
	if ( !texture ) return;

	Rectangle2I bounds;
	CalculateBounds( xForm, &bounds );

	glBindTexture( GL_TEXTURE_2D, texture->Id() );

	paintInfo->SetOpenGLTextureMode( Alpha(), cForm, xForm.IsScaled(), texture );
//	GLASSERT( openGLZ >= 1 );
//	GLASSERT( openGLZ <= kKrOglDistance );

	glBegin( GL_QUADS );
	{
		glTexCoord2f( texture->Bounds().min.x,	texture->Bounds().min.y );
		glVertex3i( bounds.min.x,				bounds.min.y,					0.0f );

		glTexCoord2f( texture->Bounds().max.x,	texture->Bounds().min.y );
		glVertex3i( bounds.min.x+bounds.Width(), bounds.min.y,					0.0f );

		glTexCoord2f( texture->Bounds().max.x,	texture->Bounds().max.y );
		glVertex3i( bounds.min.x+bounds.Width(), bounds.min.y+bounds.Height(),	0.0f );

		glTexCoord2f( texture->Bounds().min.x,	texture->Bounds().max.y );
		glVertex3i( bounds.min.x,				bounds.min.y+bounds.Height(),	0.0f );
	}
	glEnd();
	#endif
}


void KrRle::DrawScaled( KrPaintInfo* paintInfo,
						const KrMatrix2& xForm,
						const KrColorTransform& cForm,
						const Rectangle2I& clipping )
{
	GLASSERT( !paintInfo->OpenGL() );

// 	#ifdef ASSERT_IF_NOT_CACHED
// 		GLASSERT( 0 );
// 	#endif

	Rectangle2I bounds, isect;

	CalculateBounds( xForm, &bounds );
	isect = bounds;
	isect.DoIntersection( clipping );

	if ( isect.IsValid() )
	{
		U32 xInc = 0x10000 * Width()  / bounds.Width();
		U32 yInc = 0x10000 * Height() / bounds.Height();

		U32 yError = ( (isect.min.y - bounds.min.y) * yInc );
		U32 syOffset = yError >> 16;
		yError &= 0xffff;

		// Now set up the target:
		U8* target = (U8*) paintInfo->pixels
					 + paintInfo->pitch * isect.min.y;

		KrRleLine* currentLine = &line[ syOffset ];

		#ifdef DEBUG
			int yadvance = 0;
		#endif

		for( int j=0; j<isect.Height(); ++j )
		{
			GLASSERT( currentLine < &line[ size.y ] );
			GLASSERT( target >= (U8*) paintInfo->pixels + paintInfo->pitch * isect.min.y );
			GLASSERT( target <= (U8*) paintInfo->pixels + paintInfo->pitch * isect.max.y );
		
			currentLine->DrawScaled(	target, paintInfo,
										bounds,
										isect,
										cForm,
										xInc );

			target += paintInfo->pitch;
			yError += yInc;			
			while ( yError & 0xffff0000 )
			{
				yError -= 0x10000;
				++currentLine;	// also pitch, for canvases
				#ifdef DEBUG
					++yadvance;
				#endif
			}
		}
	}				
}
				  
				  
void KrRle::Draw( KrPaintInfo* paintInfo,
				  const KrMatrix2& xForm,
				  const KrColorTransform& cForm,
				  const Rectangle2I& clipping )
{
	if ( paintInfo->OpenGL() )
	{
		#ifdef KYRA_SUPPORT_OPENGL
			DrawOpenGL( paintInfo, xForm, cForm, clipping );
		#else
			GLASSERT( 0 );
		#endif
		return;
	}

	if ( xForm.IsScaled() )
	{
		DrawScaled( paintInfo, xForm, cForm, clipping );
		return;
	}

	#ifdef DRAWDEBUG_RLE
		KrPainter painter( paintInfo );
		painter.DrawBox( xForm.x.ToIntRound(), xForm.y.ToIntRound(), 
						 ( xForm.xScale * size.x ).ToIntRound(), 
						 ( xForm.yScale * size.y ).ToIntRound(), 
						 0, 150*(debugFrameCounter&0x01), 200 );
		KrRGBA red = { 255, 150*(debugFrameCounter&0x01), 0, 0 };
	#endif

	// -------- Unscaled drawing -------------------
	// The bounds min is the local origin of the RLE.
	Rectangle2I bounds;
	CalculateBounds( xForm, &bounds );

	Rectangle2I isect = bounds;
	isect.DoIntersection( clipping );

	if ( !isect.IsValid() )
		return;					// No overlap.

	// Set up variables that apply to rle < clip and rle >= clip

	// Note that the target starts on the first scan line of the
	// intersection, at the beginning of the rle bounding box.
	// This could put it off screen which technically is illegal,
	// but you're not running this on a segmented architecture.
	U8* targetLine = (U8*)   paintInfo->pixels
						   + paintInfo->bytesPerPixel * bounds.min.x
						   + paintInfo->pitch * isect.min.y;

		
	int count = isect.Height();
	KrRleLine* pLine = &line[ isect.min.y - bounds.min.y ];

	U32 currentAlpha = 0;		// It's temping to get this from the first line, but
								// it's not guarenteed to exists. #(*#$ fonts.
	KrPaintFunc blitter = paintInfo->GetBlitter( ( currentAlpha != 0 ), cForm );

//	{	
//		int dcount = rand() % 5;
//		
//		KrPainter painter( paintInfo );
//		painter.DrawFilledBox( isect.min.x, isect.ymin, isect.Width(), isect.Height(),
//							   50 * dcount, 0, 200 - 50 * dcount );
//	}

	if ( bounds.min.x < clipping.min.x )
	{
		// LEFT HAND CASE. 
		// The rle is less than the intersection box.

		// The starting draw pixel in object coordinates.
		U16 objectStart = isect.min.x - bounds.min.x;
		// The last pixel drawn in object coordinates.
		U16 objectEnd   = isect.max.x - bounds.min.x;

		GLASSERT( objectStart <= objectEnd );
		GLASSERT( objectEnd < bounds.Width() );

		for (	; 
				count; 
				--count, targetLine += paintInfo->pitch, ++pLine )
		{
			U8* target = targetLine;
			GLASSERT( pLine >= &line[0] );
			GLASSERT( pLine <= &line[ size.y - 1 ] );
			
			KrRleSegment* seg	 = pLine->segment;
			KrRleSegment* segEnd = pLine->segment + pLine->nSegments;

			// First group: toss out everything left of the clipping box.
			while (    seg < segEnd 
					&& seg->end < objectStart )
				++seg;

			if ( seg == segEnd )
				continue;

			// Second pass: might clip left, might clip right.
			// Might have complete jumped over the clipping box.
			if ( seg->start > objectEnd )
				continue;

			int dx = 0;
			if ( seg->start < objectStart )
				dx = objectStart - seg->start;
			int width = Min( seg->end, objectEnd ) - ( seg->start + dx ) + 1;

			GLASSERT( width > 0 );
				
			if ( currentAlpha != seg->Alpha() )
			{
				currentAlpha = seg->Alpha();
				blitter = paintInfo->GetBlitter( ( currentAlpha != 0 ), cForm );
			}

			GLASSERT( seg->start + dx + width - 1 <= seg->end );
			GLASSERT( width > 0 );

			blitter(	paintInfo, 
						target + (seg->start + dx) * paintInfo->bytesPerPixel, 
						seg->rgba + dx, 
						width, 
						cForm );

			++seg;
			
			// Third pass: won't clip left, won't clip right.
			// SKIPPED
			
			// Fourth pass: won't clip left, might clip right.			
			while (    seg < segEnd 
					&& seg->start <= objectEnd )
			{
				width = Min( seg->end, objectEnd ) - seg->start + 1;
				GLASSERT( width > 0 );

				if ( currentAlpha != seg->Alpha() )
				{
					currentAlpha = seg->Alpha();
					blitter = paintInfo->GetBlitter( ( currentAlpha != 0 ), cForm );
				}

				GLASSERT( seg->start + width - 1 <= seg->end );
				GLASSERT( width > 0 );

				blitter(	paintInfo, 
							target + (seg->start) * paintInfo->bytesPerPixel, 
							seg->rgba, 
							width, 
							cForm );

				++seg;
			}
		}
	}
	else
	{
		// RIGHT HAND CASE. 
		// The rle is greater than or equal to the clip box.
		U16 objectEnd   = isect.max.x - bounds.min.x;

		GLASSERT( objectEnd < bounds.Width() );

		for ( ; count; --count )
		{
			U8* target = targetLine;

			GLASSERT( pLine >= &line[0] );
			GLASSERT( pLine <= &line[ size.y - 1 ] );
			
			KrRleSegment* seg	 = pLine->segment;
			KrRleSegment* segEnd = pLine->segment + pLine->nSegments;

			// Only pass: won't clip left, might clip right.			
			while (    seg < segEnd 
					&& seg->start <= objectEnd )
			{
				int width = Min( seg->end, objectEnd ) - seg->start + 1;

				if ( currentAlpha != seg->Alpha() )
				{
					currentAlpha = seg->Alpha();
					blitter = paintInfo->GetBlitter( ( currentAlpha != 0 ), cForm );
				}

				GLASSERT( seg->start + width - 1 <= seg->end );
				GLASSERT( width > 0 );
				blitter(	paintInfo, 
							target + (seg->start) * paintInfo->bytesPerPixel, 
							seg->rgba, 
							width, 
							cForm );

				++seg;
			}
			targetLine += paintInfo->pitch;
			++pLine;
		}
	}


	#ifdef DRAWDEBUG_RLE
		if ( xForm.x.ToIntRound() >=0 && xForm.y.ToIntRound() >= 0 && xForm.x.ToIntRound() < paintInfo->width && xForm.y.ToIntRound() < paintInfo->height )
 			painter.SetPixel( xForm.x.ToIntRound(), xForm.y.ToIntRound(), red );
	#endif
}


bool KrRle::HitTestTransformed( int _x, int _y, int hitFlags)
{
	int i;

	// We are in untransformed coordinates, so check the bounds
	// against the identity matrix
	KrMatrix2 matrix;
	matrix.Set();
	Rectangle2I b;
	CalculateBounds( matrix, &b );
	if ( !b.Intersect( _x, _y ) )
	{
		return false;
	}

	int x = _x - deltaHotToOrigin.x;
	int y = _y - deltaHotToOrigin.y;

	if ( y >= 0 && y < size.y )
	{
		// Get a pixel in the RLE.
		for ( i=0; i<line[y].NumSegments(); i++ )
		{
			KrRleSegment* segment = line[y].Segment( i );
			if ( x >= segment->Start() && x <= segment->End() )
			{
				KrRGBA* rgba = segment->RGBA() + x;
				return ( rgba->c.alpha != KrRGBA::KR_TRANSPARENT );
			}
		}
	}				
	return false;
}


void KrRle::CountComponents( U32* numLines, U32* numSegments, U32* numRGBA )
{
	int i, j;

	*numLines    = size.y;
	*numSegments = 0;
	*numRGBA     = 0;

	for( i=0; i<size.y; ++i )
	{
		*numSegments += line[i].NumSegments();

		for( j=0; j<line[i].NumSegments(); ++j )
		{
			*numRGBA += line[i].Segment( j )->Len();
		}
	}
	GLASSERT( *numSegments <= *numRGBA );
	//GLASSERT( *numLines <= *numSegments );	// Freakin fonts.
}


KrCanvasResource* KrRle::CreateCanvasResource( int* hotx, int* hoty )
{
	// Originally, this code created a canvas resource and did
	// a Draw() to it. Seems to work well, but actually introduces
	// subtle alpha errors if there is alpha in the source, since
	// alpha will composite to the black and transparent canvas,
	// instead of overwriting it.

	KrMatrix2 xForm;
	xForm.Set();
	Rectangle2I bounds;

 	CalculateBounds( xForm, &bounds );
	*hotx = -bounds.min.x;
	*hoty = -bounds.min.y;

	int width = bounds.Width();
	//int height = bounds.Height();
	//GLASSERT( height == size.y );
	GLASSERT( width == size.x );

	KrCanvasResource* canvasRes = new KrCanvasResource( "fromSprite",
														bounds.Width(),
														bounds.Height(),
														true );

	// Very simple code compared to most blits and copies, because
	// there isn't any clipping. Clipping adds lots of complexity.

	// Walk the y value, in lines.
	for( int j=0; j<size.y; ++j )
	{
		KrRGBA* pTarget = canvasRes->Pixels() + j*width;
		KrRleLine* pLine = &line[j];

		if ( pLine )
		{
			// Walk the x, in segments.
			for( int segIndex=0; segIndex < pLine->NumSegments(); ++segIndex )
			{
				KrRleSegment* pSegment = pLine->Segment( segIndex );

				// This is not time critical code, so I'm being cautious
				// about checking the lines and segments.
				if ( pSegment )
				{
					GLASSERT( pSegment->Len() > 0 );
					GLASSERT( pTarget >= canvasRes->Pixels() );
					//GLASSERT( pTarget < canvasRes->Pixels() + height*width );

					memcpy( pTarget + pSegment->Start(),
							pSegment->RGBA(),
							pSegment->Len()*sizeof( KrRGBA ) );
				}
			}
		}
	}
	return canvasRes;
}


KrRle* KrRle::CreateScaledRle( GlFixed xScale, GlFixed yScale, int* hotx, int* hoty )
{
	int normalx, normaly;
	KrCanvasResource* normal = CreateCanvasResource( &normalx, &normaly );
	GLASSERT( normal );

	// Now we need something to draw the scaled canvas to.
	// First get the size and then make a pixel block.
	KrMatrix2 scaledXForm;
	scaledXForm.Set( 0, 0, xScale, yScale );
	Rectangle2I scaledBounds;
	normal->CalculateBounds( scaledXForm, &scaledBounds );

	KrPixelBlock pixelBlock;
	pixelBlock.Create( scaledBounds.Width(), scaledBounds.Height(), true );

	// Compute a new hotspot and rle.
	GlFixed scaledHotX   = -xScale * deltaHotToOrigin.x;
	GlFixed scaledHotY   = -yScale * deltaHotToOrigin.y;
	GlFixed scaledDeltaX = xScale * delta.x;
	GlFixed scaledDeltaY = yScale * delta.y;

	// Draw to the pixel block from the canvas.
	KrPaintInfo info( pixelBlock.Pixels(), pixelBlock.Width(), pixelBlock.Height() );

	KrColorTransform color;
	Rectangle2I clip;
	clip.Set( 0, 0, pixelBlock.Width()-1, pixelBlock.Height()-1 );
	normal->Draw( &info, scaledXForm, color, clip, KrQualityAdaptive );

	// Encode it!
	KrRle* rle = new KrRle();
	rle->Create( &info, 0, 0, 
				 pixelBlock.Width(),        pixelBlock.Height(),
				 scaledHotX.ToIntRound(),   scaledHotY.ToIntRound(),
				 scaledDeltaX.ToIntRound(), scaledDeltaY.ToIntRound() );

	// Clean up and return.
	delete normal;
	return rle;
}


KrCollisionMap* KrRle::GetCollisionMap( GlFixed xScale, GlFixed yScale )
{
	if ( collisionMap )
	{
		GLASSERT( collisionMap->XScale() == xScale && collisionMap->YScale() == yScale );
	}
	else
	{
		collisionMap = new KrCollisionMap( xScale, yScale, size.x, size.y );

		for( int j=0; j<size.y; ++j )
		{
			HighBitWriter<U32> writer( collisionMap->GetRow( j ) );

			// Walk the line.
			int prevX = 0;
			for( int i=0; i<line[j].NumSegments(); ++i )
			{
				KrRleSegment* segment = line[j].Segment( i );
				writer.Skip_N( segment->Start() - prevX );
				writer.Push_1N( segment->End() - segment->Start() + 1 );
				prevX = segment->End() + 1;
			}
		}
	}	
	return collisionMap;
}


/*static*/ void KrRle::SetMemoryPools(	grinliz::LinearMemoryPool*	_memoryPoolRGBA,
										grinliz::LinearMemoryPool*	_memoryPoolLine,
										grinliz::LinearMemoryPool*	_memoryPoolSegment )
{
	memoryPoolRGBA = _memoryPoolRGBA;
	memoryPoolLine = _memoryPoolLine;
	memoryPoolSegment = _memoryPoolSegment;	
}


/*static*/ bool KrRle::PoolOut()
{
	GLASSERT( memoryPoolRGBA->OutOfMemory() );
	GLASSERT( memoryPoolLine->OutOfMemory() );
	GLASSERT( memoryPoolSegment->OutOfMemory() );

	return memoryPoolRGBA->OutOfMemory() && memoryPoolLine->OutOfMemory() && memoryPoolSegment->OutOfMemory();
}
