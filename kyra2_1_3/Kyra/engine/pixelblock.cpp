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
#include "pixelblock.h"
#include "SDL.h"
#include "SDL_rwops.h"
#include "SDL_endian.h"
#include "../../grinliz/glmemorypool.h"
#include "engine.h"
#include "ogltexture.h"
#include "../../grinliz/glgeometry.h"

using namespace grinliz;

#ifdef DEBUG
	U32 KrPixelBlock::numRGBA = 0;
#endif


KrPixelBlock::KrPixelBlock()
{
	flags = 0;
	block = NULL;
	size.x = size.y = 0;
	texture = 0;
}


KrPixelBlock::~KrPixelBlock()
{
	if ( ! ( flags & MEMORYPOOL ) )
		delete [] block;
	delete texture;
}


bool KrPixelBlock::Create(	KrPaintInfo* surface,
							int x, int y, int width, int height )
{
	int i, j;

	size.x = width;
	size.y = height;

	KrPainter painter( surface );
	GLASSERT( block == 0 );
	block = new KrRGBA[ width * height ];
	flags = 0;

	for( i=0; i<width; i++ )
	{
		for( j=0; j<height; j++ )
		{
			KrRGBA rgba;
			painter.BreakPixel( x+i, y+j, &rgba );

			// Check for alpha:
			if ( rgba.c.alpha != KrRGBA::KR_OPAQUE )
			{
				flags |= ALPHA;
			}
			block[ j*width + i ] = rgba;
		}
	}
	return true;
}


bool KrPixelBlock::Create( int width, int height, bool alphaSupport )
{
	flags = 0;
	if ( alphaSupport) flags |= ALPHA;
	size.x = width;
	size.y = height;
	GLASSERT( block == 0 );

	block = new KrRGBA[ width * height ];	
	//memset( block, 0, width * height * sizeof( KrRGBA ) );
	
	// Need to get alpha channel correct for openGL
	KrRGBA color;
	color.Set( 0, 0, 0, alphaSupport ? 0 : 255 );	// if alpha not supported, surface is black, else transparent
	U32 v = color.all;
	U32 count = width * height;
	U32* block32 = (U32*) block;

	for( U32 i=0; i<count; ++i )
		block32[i] = v;

	return true;
}


bool KrPixelBlock::Write( SDL_RWops* fp )
{
	#ifdef DEBUG
		GLOUTPUT(( "Writing tile size=(%d,%d) (", size.x, size.y ));
		if ( flags & ALPHA ) GLOUTPUT(( "ALPHA " ));
		GLOUTPUT(( ")\n" ));
	#endif
	SDL_WriteLE32( fp, flags );
	SDL_WriteLE32( fp, size.x );
	SDL_WriteLE32( fp, size.y );

	for ( int i=0; i<( size.x * size.y ); i++ )
	{
		SDL_RWwrite( fp, &block[i].c.red,   1, 1);
		SDL_RWwrite( fp, &block[i].c.green, 1, 1);
		SDL_RWwrite( fp, &block[i].c.blue,  1, 1);
		if ( flags & ALPHA )
			SDL_RWwrite( fp, &block[i].c.alpha, 1, 1);
	}
	return true;
}


bool KrPixelBlock::Read( SDL_RWops* data )
{
	flags = SDL_ReadLE32( data );
	size.x = SDL_ReadLE32( data );
	size.y = SDL_ReadLE32( data );
	GLASSERT( block == 0 );

	block = 0;
	if ( KrRle::memoryPoolRGBA )
	{
		block = (KrRGBA*) KrRle::memoryPoolRGBA->Alloc( size.x * size.y * sizeof( KrRGBA ) );
		flags |= MEMORYPOOL;
	}
	if ( !block )
	{
		block = new KrRGBA[ size.x * size.y ];
	}
	#ifdef DEBUG
		numRGBA += size.x * size.y;
	#endif

// 	block = new KrRGBA[ size.x * size.y ];

	for ( int i=0; i<size.x*size.y; i++ )
	{
		SDL_RWread( data, &block[i].c.red,   1, 1);
		SDL_RWread( data, &block[i].c.green, 1, 1);
		SDL_RWread( data, &block[i].c.blue,  1, 1);

		if ( flags & ALPHA )
			SDL_RWread( data, &block[i].c.alpha, 1, 1);
		else
			block[i].c.alpha = 255;
	}
	return true;
}


void KrPixelBlock::CalculateBounds( const KrMatrix2& xForm, Rectangle2I* bounds ) const
{
	bounds->min.x = xForm.x.ToIntRound();
	bounds->min.y = xForm.y.ToIntRound();
	bounds->max.x = ( xForm.xScale * size.x + xForm.x ).ToIntRound() - 1;
	bounds->max.y = ( xForm.yScale * size.y + xForm.y ).ToIntRound() - 1;

	#ifdef DEBUG
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
	#endif	
}


void KrPixelBlock::DrawScaledDown(	KrPaintInfo* info,
									const KrMatrix2& xForm,
									const KrColorTransform& cForm,
									const Rectangle2I& clipping )
{
	GLASSERT( !info->OpenGL() );

	Rectangle2I tBounds;
	CalculateBounds( xForm, &tBounds );

	Rectangle2I isect = tBounds;
	isect.DoIntersection( clipping );

	if ( isect.IsValid() )
	{
		int txOffset = isect.min.x - tBounds.min.x;
		int tyOffset = isect.min.y - tBounds.min.y;

		// A square blit won't introduce an alpha:
		KrPaintFunc blitter = info->GetBlitter( ( flags & ALPHA ), cForm );

		// Every pixel in the target 'increments' the source
		// by a certain amount. This is the integer scaling factor
		// between source and target.
		U32 xInc = 0x10000 * Width()  / tBounds.Width();
		U32 yInc = 0x10000 * Height() / tBounds.Height();

		// The 'error' terms. Every time the exceed x10000,
		// its time to move a pixel in source. The distance
		// traveled on the target is *always* one pixel.
		U32 xError = ( txOffset * xInc );
		U32 yError = ( tyOffset * yInc );
		
		// Every new row will have the same x-error:
		U32 xErrorPerRow = xError & 0xffff;
		
		// The low part of error contains the current error;
		// the high part the # of pixels to move in source.
		KrRGBA* rowSource = block
							+ ( ( xError & 0xffff0000 ) >> 16 )
							+ ( ( yError & 0xffff0000 ) >> 16 ) * size.x;
		xError &= 0xffff;
		yError &= 0xffff;
		KrRGBA* source;

		// Now set up the target:
		U8* rowTarget = (U8*) info->pixels 
						+ isect.min.y * info->pitch
						+ isect.min.x * info->bytesPerPixel;
		U8* target;


		int iWidth  = isect.Width();
		int iHeight = isect.Height();

		for ( int j=0; j<iHeight; ++j )
		{
			target = rowTarget;
			source = rowSource;

			for( int i=0; i<iWidth; ++i )
			{
				// This one is a little different. It is always a
				// downsample, and sums all the colors over the downsample
				// rectangle.
				U32 color[4] = { 0, 0, 0, 0 };

				GLASSERT( xInc >= 0x1000 );
				GLASSERT( yInc >= 0x1000 );
				U32 dx = ( xInc + xError ) >> 16;
				U32 dy = ( yInc + yError ) >> 16;
				int subSize = 0;	// For anti-artifacting, we don't necessary use every pixel.

				KrRGBA* row = source;
				KrRGBA* scan;

				for( unsigned jj=0; jj<dy; ++jj )
				{
					scan = row;
					for( unsigned ii=0; ii<dx; ++ii )
					{
						++scan;
						if ( scan->c.alpha != 0 )
						{
							++subSize;
							for( unsigned k=0; k<4; ++k )
							{
								color[k] += scan->array[k];
							}
						}
					}
					row += size.x;	// the scanline for a canvas
				}
				if ( subSize != 0 )
				{
					for( int k=0; k<4; ++k )
					{
						color[k] /= subSize;
					}
				}
				else
				{
					color[0] = 0;
					color[1] = 0;
					color[2] = 0;
					color[3] = 0;
				}
				GLASSERT( color[0] <= 255 );
				GLASSERT( color[1] <= 255 );
				GLASSERT( color[2] <= 255 );
				GLASSERT( color[3] <= 255 );
				#ifdef DEBUG
					if ( !(flags & ALPHA) ) GLASSERT( color[ KrRGBA::ALPHA ] == 255 );
				#endif

				KrRGBA buffer;
				buffer.Set( color[ KrRGBA::RED ],
							color[ KrRGBA::GREEN ],
							color[ KrRGBA::BLUE ],
							color[ KrRGBA::ALPHA ] );

				blitter( info, target, &buffer, 1, cForm );

				target += info->bytesPerPixel;
				xError += xInc;
				while ( xError & 0xffff0000 )
				{
					xError -= 0x10000;
					++source;
				}
			}
			
			rowTarget += info->pitch;
			
			yError += yInc;			
			while ( yError & 0xffff0000 )
			{
				yError -= 0x10000;
				rowSource += size.x;	// also pitch, for canvases
			}

			xError = xErrorPerRow;
		}
	}
}


void KrPixelBlock::DrawScaledLinear(	KrPaintInfo* info,
										const KrMatrix2& xForm,
										const KrColorTransform& cForm,
										const Rectangle2I& clipping )
{
	GLASSERT( !info->OpenGL() );
	
	Rectangle2I tBounds;
	CalculateBounds( xForm, &tBounds );

	Rectangle2I isect = tBounds;
	isect.DoIntersection( clipping );

	if ( isect.IsValid() )
	{
		int txOffset = isect.min.x - tBounds.min.x;
		int tyOffset = isect.min.y - tBounds.min.y;

		// A square blit won't introduce an alpha:
		KrPaintFunc blitter = info->GetBlitter( ( flags & ALPHA ), cForm );

		// Every pixel in the target 'increments' the source
		// by a certain amount. This is the integer scaling factor
		// between source and target. Note however, that we need
		// one less source, since the interpolation will be between
		// the source pixel and the one to the right and down.
		U32 xInc = 0x10000 * ( Width() - 1 )  / tBounds.Width();
		U32 yInc = 0x10000 * ( Height() - 1 ) / tBounds.Height();

		// The 'error' terms. Every time the exceed x10000,
		// its time to move a pixel in source. The distance
		// traveled on the target is *always* one pixel.
		U32 xError = ( txOffset * xInc );
		U32 yError = ( tyOffset * yInc );
		
		// Every new row will have the same x-error:
		U32 xErrorPerRow = xError & 0xffff;
		
		// The low part of error contains the current error;
		// the high part the # of pixels to move in source.
		KrRGBA* rowSource = block
							+ ( ( xError & 0xffff0000 ) >> 16 )
							+ ( ( yError & 0xffff0000 ) >> 16 ) * size.x;
		xError &= 0xffff;
		yError &= 0xffff;
		KrRGBA* source0;
		KrRGBA* source1;

		// Now set up the target:
		U8* rowTarget = (U8*) info->pixels 
						+ isect.min.y * info->pitch
						+ isect.min.x * info->bytesPerPixel;
		U8* target;


		int iWidth  = isect.Width();
		int iHeight = isect.Height();

		for ( int j=0; j<iHeight; ++j )
		{
			target = rowTarget;
			source0 = rowSource;
			source1 = rowSource + size.x;

			KrRGBA color, c00, c01, c10, c11;

			for( int i=0; i<iWidth; ++i )
			{
				// We now copy and process, for anti-artifacting
				c00 = *source0;
				c10 = *(source0+1);
				c01 = *source1;
				c11 = *(source1+1);
				int k;

// 				for( k=KrRGBA::START; k<KrRGBA::END; ++k )
// 				{
// 					U32 g0, g1;
// 
// 					// interpelate on scanline, top
// 					g0 = (   (( c00.array[k] * ( 0x10000 - xError ) ) >> 16 ) * ( c00.c.alpha + 1 )
// 						   + (( c10.array[k] * ( xError ) ) >> 16 ) * ( c10.c.alpha + 1 ) ) / ( ( c00.c.alpha + c10.c.alpha ) / 2 + 1);
// 					// interpolate on scanline, bottom
// 					g1 = (   (( c01.array[k] * ( 0x10000 - xError ) ) >> 16 ) * ( c01.c.alpha + 1 )
// 						   + (( c11.array[k] * ( xError ) ) >> 16 ) * ( c11.c.alpha + 1 ) ) / ( ( c01.c.alpha + c11.c.alpha ) / 2 + 1);
// 					// interpolate between scanlines.
// 					color.array[k]  = (   (( g0 * ( 0x10000 - yError ) ) >> 16 )
// 										+ (( g1 * ( yError ) ) >> 16 ) );
// 				}
// 
// 				k = KrRGBA::ALPHA;
				for( k=0; k<4; ++k )
				{
					U32 g0, g1;

					// interpelate on scanline, top
					g0 = (   (( c00.array[k] * ( 0x10000 - xError ) ) >> 16 )
						   + (( c10.array[k] * ( xError ) ) >> 16 ) );
					// interpolate on scanline, bottom
					g1 = (   (( c01.array[k] * ( 0x10000 - xError ) ) >> 16 )
						   + (( c11.array[k] * ( xError ) ) >> 16 ) );
					// interpolate between scanlines.
					color.array[k]  = (   (( g0 * ( 0x10000 - yError ) ) >> 16 )
										+ (( g1 * ( yError ) ) >> 16 ) );
				}

				blitter( info, target, &color, 1, cForm );
				target += info->bytesPerPixel;
				
				xError += xInc;
				while ( xError & 0xffff0000 )
				{
					xError -= 0x10000;
					++source0;
					++source1;
					GLASSERT( source0 < rowSource + size.x );
					GLASSERT( source1 < rowSource + size.x * size.y );
				}
			}
			
			rowTarget += info->pitch;
			
			yError += yInc;			
			while ( yError & 0xffff0000 )
			{
				yError -= 0x10000;
				rowSource += size.x;	// also pitch, for canvases
			}

			xError = xErrorPerRow;
		}
	}
}


void KrPixelBlock::DrawScaledFast(	KrPaintInfo* info,
									const KrMatrix2& xForm,
									const KrColorTransform& cForm,
									const Rectangle2I& clipping,
									bool invert )
{
	GLASSERT( !info->OpenGL() );

	Rectangle2I tBounds;
	CalculateBounds( xForm, &tBounds );

	Rectangle2I isect = tBounds;
	isect.DoIntersection( clipping );

	if ( isect.IsValid() )
	{
		int txOffset = isect.min.x - tBounds.min.x;
		int tyOffset = isect.min.y - tBounds.min.y;

		// A square blit won't introduce an alpha:
		KrPaintFunc blitter = info->GetBlitter( ( flags & ALPHA ), cForm );

		// Every pixel in the target 'increments' the source
		// by a certain amount. This is the integer scaling factor
		// between source and target.
		U32 xInc = 0x10000 * Width()  / tBounds.Width();
		U32 yInc = 0x10000 * Height() / tBounds.Height();

		// The 'error' terms. Every time the exceed x10000,
		// its time to move a pixel in source. The distance
		// traveled on the target is *always* one pixel.
		U32 xError = ( txOffset * xInc );
		U32 yError = ( tyOffset * yInc );
		
		// Every new row will have the same x-error:
		U32 xErrorPerRow = xError & 0xffff;
		
		int yBias = 1;
		int yStartBias = 0;
		if ( invert )
		{
			yBias = -1;
			yStartBias = size.y - 1;
		}

		// The low part of error contains the current error;
		// the high part the # of pixels to move in source.
		KrRGBA* rowSource = block
							+ ( ( xError & 0xffff0000 ) >> 16 )
							+ ( yBias * ( ( yError & 0xffff0000 ) >> 16 ) + yStartBias ) * size.x;
		GLASSERT( rowSource >= block );
		GLASSERT( rowSource <  block + size.y * size.x );

		xError &= 0xffff;
		yError &= 0xffff;
		KrRGBA* source;

		// Now set up the target:
		U8* rowTarget = (U8*) info->pixels 
						+ isect.min.y * info->pitch
						+ isect.min.x * info->bytesPerPixel;
		U8* target;


		int iWidth  = isect.Width();
		int iHeight = isect.Height();

		for ( int j=0; j<iHeight; ++j )
		{
			target = rowTarget;
			source = rowSource;

			for( int i=0; i<iWidth; ++i )
			{
				GLASSERT( source >= block );
				GLASSERT( source <  block + size.y * size.x );
				GLASSERT( target >= (U8*) info->pixels );
				GLASSERT( target <  (U8*) info->pixels + info->pitch * info->height );

				blitter( info, target, source, 1, cForm );
				target += info->bytesPerPixel;
				
				xError += xInc;
				while ( xError & 0xffff0000 )
				{
					xError -= 0x10000;
					++source;
				}
			}
			
			rowTarget += info->pitch;
			
			yError += yInc;			
			while ( yError & 0xffff0000 )
			{
				yError -= 0x10000;
				rowSource += yBias * size.x;	// also pitch, for canvases
			}

			xError = xErrorPerRow;
		}
	}
}


void KrPixelBlock::DrawScaled(	KrPaintInfo* info,
								const KrMatrix2& xForm,
								const KrColorTransform& cForm,
								const Rectangle2I& clipping,
								int quality,
								bool invert )
{
	GLASSERT( !info->OpenGL() );

	if ( invert || quality == KrQualityFast )
	{
		DrawScaledFast( info, xForm, cForm, clipping, invert );
	}
	else if ( quality == KrQualityLinear )
	{
		DrawScaledLinear( info, xForm, cForm, clipping );
	}
	else
	{
		GLASSERT( quality == KrQualityAdaptive );
		if (    xForm.xScale.v <= GlFixed_1 / 2
		     && xForm.yScale.v <= GlFixed_1 / 2 )
		{
			DrawScaledDown( info, xForm, cForm, clipping );
		}
		else
		{
			DrawScaledLinear( info, xForm, cForm, clipping );
		}
	}
}


void KrPixelBlock::LoadNewTexture()
{
	if ( texture )
	{
		texture->SetTexture( block, size.x, size.y );
	}
}


void KrPixelBlock::DrawOpenGL(		KrPaintInfo* paintInfo,
									const KrMatrix2& xForm,
									const KrColorTransform& cForm,
									const Rectangle2I& clipping,
									int rotation )
{
	#ifdef KYRA_SUPPORT_OPENGL
	GLASSERT( paintInfo->OpenGL() );

	if ( !texture )
	{
		KrTextureManager* manager = KrTextureManager::Instance();
		if ( manager )
		{
			// Create a canvas resource with the data we need.
			texture = manager->CreateTexture( block, size.x, size.y );
		}
	}

	GLASSERT( texture );
	if ( !texture ) return;

	glBindTexture( GL_TEXTURE_2D, texture->Id() );

	paintInfo->SetOpenGLTextureMode( Alpha(), cForm, xForm.IsScaled(), texture );

	Rectangle2I bounds;
	CalculateBounds( xForm, &bounds );

	Vector2F texCoord[ 4 ];

	texCoord[ (rotation+0) % 4 ].x = texture->Bounds().min.x;
	texCoord[ (rotation+0) % 4 ].y = texture->Bounds().min.y;

	texCoord[ (rotation+1) % 4 ].x = texture->Bounds().max.x;
	texCoord[ (rotation+1) % 4 ].y = texture->Bounds().min.y;

	texCoord[ (rotation+2) % 4 ].x = texture->Bounds().max.x;
	texCoord[ (rotation+2) % 4 ].y = texture->Bounds().max.y;

	texCoord[ (rotation+3) % 4 ].x = texture->Bounds().min.x;
	texCoord[ (rotation+3) % 4 ].y = texture->Bounds().max.y;

	if ( rotation > 3 )
	{
		Swap( &texCoord[0], &texCoord[3] );
		Swap( &texCoord[1], &texCoord[2] );
	}

	glBegin( GL_QUADS );
	{
		glTexCoord2f( texCoord[0].x,	texCoord[0].y );
		glVertex3i( bounds.min.x,				bounds.min.y,					0 );

		glTexCoord2f( texCoord[1].x,	texCoord[1].y );
		glVertex3i( bounds.min.x+bounds.Width(), bounds.min.y,					0 );

		glTexCoord2f( texCoord[2].x,	texCoord[2].y );
		glVertex3i( bounds.min.x+bounds.Width(), bounds.min.y+bounds.Height(),	0 );
		
		glTexCoord2f( texCoord[3].x,	texCoord[3].y );
		glVertex3i( bounds.min.x,				bounds.min.y+bounds.Height(),	0 );
	}
	glEnd();
	GLASSERT( glGetError() == GL_NO_ERROR );
	#endif
}


void KrPixelBlock::Draw(	KrPaintInfo* paintInfo,
							const KrMatrix2& xForm,
							bool invert,
							const KrColorTransform& cForm,
							const Rectangle2I& clipping,
							int quality )
{
	if ( paintInfo->OpenGL() )
	{
		#ifdef KYRA_SUPPORT_OPENGL
			GLASSERT( invert == false );	// This path should only come from a canvas, which does no invert.
			DrawOpenGL( paintInfo, xForm, cForm, clipping, 0 );
		#else
			GLASSERT( 0 );
		#endif
		return;
	}

	if ( xForm.IsScaled() )
	{
		//GLASSERT( invert == false );	// we don't scale tiles.
		DrawScaled( paintInfo, xForm, cForm, clipping, quality, invert );
		return;
	}

	Rectangle2I bounds;
	CalculateBounds( xForm, &bounds );

	Rectangle2I isect = bounds;
	isect.DoIntersection( clipping );

	int j;
	int yOffset		= isect.min.y - bounds.min.y;
	int xOffset		= isect.min.x - bounds.min.x;
	int width		= isect.Width();
	int height		= isect.Height();
	int sourcePitch	= size.x;

	U8* target =   (U8*) paintInfo->pixels 
				   + isect.min.y * paintInfo->pitch
				   + isect.min.x * paintInfo->bytesPerPixel;

	// for rotations 4-7, we run the source in reverse.
	KrRGBA* source;
	if ( !invert )
	{
		source =   block
				 + yOffset * size.x
				 + xOffset;
	}
	else
	{
		source =   block
				 + ( size.y - yOffset - 1) * size.x
				 + xOffset;
		sourcePitch = -sourcePitch;
	}

	if ( width > 0 && height > 0 )
	{
		KrPaintFunc blitter = paintInfo->GetBlitter( ( flags & ALPHA ), cForm );
		if ( blitter )
		{
			for ( j=0; 
				  j<height; 
				  j++, source += sourcePitch, target += paintInfo->pitch )
			{
				blitter( paintInfo, target, source, width, cForm );
			}
		}
	}
}


void KrPixelBlock::CountComponents( U32* numRGBA )
{
	*numRGBA = size.x * size.y;
}
