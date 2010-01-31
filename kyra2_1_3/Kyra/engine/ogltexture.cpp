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

#include <math.h>
#include "canvasresource.h"
#include "ogltexture.h"
#include "kyrabuild.h"
#include "engine.h"

using namespace grinliz;

#ifdef KYRA_SUPPORT_OPENGL

///////////////// KrOglTexture
KrOglTexture::KrOglTexture( const KrRGBA* image,
							int width, int height,
							Rectangle2F* textCoords,
							Rectangle2I* pixelCoords )
{
	refCount = 0;
	scaled = false;

	// First, create the OGL texture itself. Need to get size information.
	GLint sizeX, sizeY, maxSize;
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &sizeX );
	maxSize = KrEngine::MaxOglTextureSize();
	if ( maxSize > 0 )
		sizeY = sizeX = Min( sizeX, maxSize );
	else
		sizeY = sizeX;

	if ( (width+2) > sizeX || (height+2) > sizeY )
	{
		scaled = true;
	}

	// Number of texels -- both width and height -- must be a power of 2.
	// Compress as much as possible, because even with texture re-use,
	// the re-use isn't very efficient. Also, the border isn't needed if
	// the width and height are exactly equal to the texture size, but
	// the extra space is always used.
	//
	// On the other hand scaled textures consume the entire thing.
	//
	if ( !scaled )
	{
		while ( ( sizeX / 2 ) >= (width+2) )
		{
			sizeX /= 2;
			if ( sizeX == 2 ) break;
		}
		while ( ( sizeY / 2 ) >= (height+2) )
		{
			sizeY /= 2;
			if ( sizeY == 2 ) break;
		}
	}
	// Now create the new image, and map from BGRA --> RGBA
	U8* image2 = new U8[ sizeX * sizeY * 4 + 4 + 4];
	#ifdef DEBUG
		image2[ 0 ] = 'f';
		image2[ 1 ] = 'i';
		image2[ 2 ] = 'r';
		image2[ 3 ] = 'e';
		image2 += 4;

		image2[ sizeX * sizeY * 4 + 0 ] = 'k';
		image2[ sizeX * sizeY * 4 + 1 ] = 'y';
		image2[ sizeX * sizeY * 4 + 2 ] = 'r';
		image2[ sizeX * sizeY * 4 + 3 ] = 'a';
	#endif

	GLASSERT( image2 );
	if ( !image2 ) return;

	//int				x = 1;
	//int				y = 1;
	const int		scan = sizeX*4;
	//U8*				target = image2 + x*4 + y*scan;
	const KrRGBA*	source = image;

	// copy over image data
	if ( scaled )
	{
		CreateScaledTexture(	image2, sizeX, sizeY, scan,
								source, width, height );
	}
	else
	{
		CreateTexture(	image2 + scan + 4,	// one row down, one pixel over
						width, height, scan,
						source, width, height );
	}

	#ifdef DEBUG
		GLASSERT( image2[ -4 ] == 'f' );
		GLASSERT( image2[ -3 ] == 'i' );
		GLASSERT( image2[ -2 ] == 'r' );
		GLASSERT( image2[ -1 ] == 'e' );

		GLASSERT( image2[ sizeX * sizeY * 4 + 0 ] == 'k' );
		GLASSERT( image2[ sizeX * sizeY * 4 + 1 ] == 'y' );
		GLASSERT( image2[ sizeX * sizeY * 4 + 2 ] == 'r' );
		GLASSERT( image2[ sizeX * sizeY * 4 + 3 ] == 'a' );
	#endif

	glGenTextures( 1, &textureId );
	glBindTexture( GL_TEXTURE_2D, textureId );

	glTexImage2D(	GL_TEXTURE_2D,
					0,					// no mip mapping
					GL_RGBA,			// internal format
					sizeX,
					sizeY,
					0,					// no border
					GL_RGBA,			// format
					GL_UNSIGNED_BYTE,
					image2 );

	GLASSERT( glGetError() == GL_NO_ERROR );

	#ifdef DEBUG
		image2 -= 4;
	#endif
	delete [] image2;

	if ( scaled )
	{
		// Scaled textures always take up the entire texture map.
		textCoords->min.x = 0.0f;
		textCoords->min.y = 0.0f;
		textCoords->max.x = 1.0f;
		textCoords->max.y = 1.0f;
		pixelCoords->Set( 0, 0, sizeX - 1, sizeY - 1 );
	}
	else
	{
		textCoords->min.x = float( 1 ) / float( sizeX );
		textCoords->min.y = float( 1 ) / float( sizeY );
		textCoords->max.x = float( width+1 )  / float( sizeX );
		textCoords->max.y = float( height+1 ) / float( sizeY );
		pixelCoords->Set( 1, 1, width, height );
	}

	GLOUTPUT(( "Texture %d created\n", textureId ));
}


void KrOglTexture::SetTexture(	const KrRGBA* pixels, 
								const Rectangle2I& pixelCoords  )
{
	glBindTexture( GL_TEXTURE_2D, textureId );

	/* sigh. Still have to flip bytes. */
	int byteSize = pixelCoords.Width() * pixelCoords.Height() * 4;
	int colorSize = pixelCoords.Width() * pixelCoords.Height();

	U8* data = new U8[ byteSize ];
	
	GLASSERT( data  );
	if ( !data ) return;

	memcpy( data, pixels, byteSize );

	#ifdef KYRA_FLIP_COLORS
		U8* p = data;
		for ( int i=0; i<colorSize; ++i )
		{
			Swap( &p[0], &p[2] );
			p += 4;
		}
	#endif

	glTexSubImage2D(GL_TEXTURE_2D, 
					0,						// no mip mapping
					pixelCoords.min.x,		// coords on the texture
					pixelCoords.min.y,
					pixelCoords.Width(),	// coords on the source -- must be equal
					pixelCoords.Height(),
					GL_RGBA,				// format
					GL_UNSIGNED_BYTE,
					data );

	GLASSERT( glGetError() == GL_NO_ERROR );
	delete [] data;
}


void KrOglTexture::SetScaledTexture( const KrRGBA* pixels, int w, int h )
{
	glBindTexture( GL_TEXTURE_2D, textureId );
	GLint sizeX, sizeY;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &sizeX );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &sizeY );

	U8* target = new U8[ sizeX * sizeY * 4 ];
	
	GLASSERT( target  );
	if ( !target ) return;

	CreateScaledTexture( target, sizeX, sizeY, sizeX*4, pixels, w, h );

	glTexSubImage2D(GL_TEXTURE_2D, 
					0,						// no mip mapping
					0,		// xoffset
					0,		// yoffset
					sizeX,
					sizeY,
					GL_RGBA,				// format
					GL_UNSIGNED_BYTE,
					target );

	GLASSERT( glGetError() == GL_NO_ERROR );
	delete [] target;
}


void KrOglTexture::CreateTexture(	U8* target, int width, int height, int targetScan,
										const KrRGBA* source, int sourceW, int sourceH )
{
	GLASSERT( height == sourceH );
	GLASSERT( width == sourceW );

	int j;
	U8* t = target;
	const KrRGBA* s = source;

	for( j=0; j<height; ++j )
	{
		GLASSERT( s >= source && s < (source + sourceW * sourceH ) );
		FillRow( t, s, sourceW );
		t += targetScan;
		s += width;
	}
	// Create the border for linear interpolation.
	// top
	memcpy( target - targetScan,
			target,
			width*4 );

	// bottom
	memcpy( target + height * targetScan,
			target + (height-1) * targetScan,
			width*4 );

	// sides and corners
//	target = image2 + x*4 + (y-1)*scan;
	U32* target32 = (U32*) (target-targetScan);
	for( j=-1; j<=height; ++j )
	{
		*(target32-1) = *(target32);
		*(target32+width) = *(target32+width-1);
		target32 += targetScan / 4;
	}
}


void KrOglTexture::CreateScaledTexture(	U8* target, int targetW, int targetH, int targetScan,
										const KrRGBA* source, int sourceW, int sourceH )
{
	U32 errorX = 0;
	U32 errorY = 0;
	GlFixed xStep = double( sourceW ) / double( targetW );
	GlFixed yStep = double( sourceH ) / double( targetH );

	U8* targetLine = target;
	U8* t = targetLine;
	const KrRGBA* sourceLine = source;
	const KrRGBA* s = sourceLine;

	int j;
	for ( j=0; j<targetW; ++j )
	{
		for( int i=0; i<targetH; ++i )
		{
			GLASSERT( t >= target && t < target + targetScan*targetH );
			*t = s->c.red;		++t;
			*t = s->c.green;	++t;
			*t = s->c.blue;		++t;
			*t = s->c.alpha;	++t;
			GLASSERT( i == (targetH-1) || t >= target && t < target + targetScan*targetH );

			errorX += xStep.v;
			if( errorX & 0xffff0000 )
			{
				s += ( errorX >> 16 );
				GLASSERT(    i == targetW -1 
						  || s < source + sourceW * sourceH );
				errorX &= 0x0000ffff;
			}	
		}
		targetLine += targetScan;
		t = targetLine;

		errorY += yStep.v;
		if( errorY & 0xffff0000 )
		{
			sourceLine += ( errorY >> 16 ) * sourceW;
			errorY &= 0x0000ffff;
		}	
		s = sourceLine;
		GLASSERT(    j == targetH - 1
				  || s < source + sourceW * sourceH );
	}
}


KrOglTexture::~KrOglTexture()
{
	glDeleteTextures( 1, &textureId );

	// Tell the manager we're going away.
	KrTextureManager* manager = KrTextureManager::Instance();
	GLASSERT( manager );
	manager->OglTextureDeleting( this );
}


void KrOglTexture::FillRow( U8* target, const KrRGBA* source, int width )
{
	int i;
	for( i=0; i<width; ++i )
	{
		*target = source->c.red;	++target;
		*target = source->c.green;	++target;
		*target = source->c.blue;	++target;
		*target = source->c.alpha;	++target;
		++source;
	}
}
#endif

///////////////// KrTexture
KrTexture::KrTexture( KrOglTexture* _oglTexture, const Rectangle2F& _bounds, const Rectangle2I& _pb )
{
	#ifdef KYRA_SUPPORT_OPENGL
	oglTexture = _oglTexture;
	bounds = _bounds;
	pixelBounds = _pb;

	oglTexture->AddRef();
	#endif
}


KrTexture::~KrTexture()
{
	#ifdef KYRA_SUPPORT_OPENGL
	oglTexture->RemoveRef();
	#endif
}


void KrTexture::SetTexture( const KrRGBA* image, int width, int height )
{
	#ifdef KYRA_SUPPORT_OPENGL
	if ( Scale() )
	{
		oglTexture->SetScaledTexture( image, width, height );
	}
	else
	{
		GLASSERT( width  == pixelBounds.Width() );
		GLASSERT( height == pixelBounds.Height() );
		oglTexture->SetTexture( image, pixelBounds );
	}
	#endif
}


///////////////// KrTextureManager

/*static*/ KrTextureManager* KrTextureManager::Instance()
{
	if ( !instance )
		instance = new KrTextureManager;
	return instance;
}


KrTextureManager* KrTextureManager::instance = 0;


KrTexture* KrTextureManager::CreateTexture( const KrRGBA* data, int width, int height )
{
	KrTexture* ret = 0;

	#ifdef KYRA_SUPPORT_OPENGL
	Rectangle2F coords;
	Rectangle2I pixel;

	KrOglTexture* newtex = new KrOglTexture( data, width, height, &coords, &pixel );
//	if ( newtex )
//		texArray.PushBack( newtex );
	GLASSERT( newtex );

	if ( newtex )
	{
		ret = new KrTexture( newtex, coords, pixel );
		GLASSERT( ret );
	}
	++oglTextureCount;
	#endif
	return ret;
}


KrTextureManager::~KrTextureManager()
{
	GLASSERT( oglTextureCount == 0 );
	oglTextureCount = 0;

	GLOUTPUT(( "KrTextureManager destructor.\n" ));
}


void KrTextureManager::OglTextureDeleting( KrOglTexture* goingAway )
{
	--oglTextureCount;

	GLASSERT( oglTextureCount >= 0 );
	GLOUTPUT(( "Texture %d deleting. Count=%d\n", goingAway->Id(), oglTextureCount ));

	if( oglTextureCount == 0 )
	{
		delete this;
		instance = 0;
	}

	// TODO. Implement re-using the OglTexture
//	for( unsigned i=0; i<texArray.Count(); ++i )
//	{
//		if ( texArray[i] == goingAway )
//		{
//			texArray.Remove( i );
//			#ifdef DEBUG
//				GLOUTPUT( "Texture manager delete: %d left\n", texArray.Count() );
//			#endif
//	
//			// This is strange...delete the manager if we are out of textures.
//			if ( texArray.Empty() )
//			{
//				instance = 0;
//				delete this;
//			}
//			return;
//		}
//	}
}

