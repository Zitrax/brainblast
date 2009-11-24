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
#include "painter.h"
#include "SDL.h"
#ifdef KYRA_SUPPORT_OPENGL
	#include "SDL_opengl.h"
#endif
#include "../../grinliz/gldebug.h"
#include "../../grinliz/glperformance.h"
#include "ogltexture.h"
#include "../../grinliz/glgeometry.h"

using namespace grinliz;

//#define COUNT_PERFORMANCE
#define KYRA_ASM

KrPainter::KrPainter( SDL_Surface* _surface ) 
	: surface( _surface )
{
}


KrPainter::KrPainter( KrPaintInfo* _info )
	: surface( _info->surface )
{
}


KrPainter::~KrPainter()
{
// 	if ( mustDeletePaintInfo )
// 		delete paintInfo;
}


void KrPainter::DrawHLine(	int x, int y, int w, 
							U8 red, U8 green, U8 blue ) 
{
	GLASSERT( !(surface->flags & SDL_OPENGL) );
	Rectangle2I targetRect;
	targetRect.min.x = 0;
	targetRect.min.y = 0;
	targetRect.max.x = surface->w  - 1;
	targetRect.max.y = surface->h - 1;

	Rectangle2I lineRect;
	lineRect.min.x = x;
	lineRect.min.y = y;
	lineRect.max.x = x + w - 1;
	lineRect.max.y = y;

	if ( targetRect.Intersect( lineRect ) )
	{
		lineRect.DoIntersection( targetRect );
		w = lineRect.max.x - lineRect.min.x + 1;

		U8* dst;
		int nPixel;

		// Draw the top and bottom h-line
		dst = (U8*) surface->pixels
			  + surface->pitch*lineRect.min.y
			  + surface->format->BytesPerPixel*lineRect.min.x;

		for (	nPixel = 0;
				nPixel < w;
				nPixel++ )
		{
			SetPixel( dst, red, green, blue, KrRGBA::KR_OPAQUE );
			dst += surface->format->BytesPerPixel;
		}
	}
}


void KrPainter::DrawHLine(	int x, int y, int w, 
							const KrRGBA* colors, int nColors ) 
{
	GLASSERT( !(surface->flags & SDL_OPENGL ));
	
	Rectangle2I targetRect;
	targetRect.min.x = 0;
	targetRect.min.y = 0;
	targetRect.max.x = surface->w - 1;
	targetRect.max.y = surface->h - 1;

	Rectangle2I lineRect;
	lineRect.min.x = x;
	lineRect.min.y = y;
	lineRect.max.x = x + w - 1;
	lineRect.max.y = y;

	if ( targetRect.Intersect( lineRect ) )
	{
		lineRect.DoIntersection( targetRect );
		w = lineRect.max.x - lineRect.min.x + 1;

		U8* dst;
		int nPixel;

		// Draw the top and bottom h-line
		dst = (U8*)   surface->pixels
		            + surface->pitch * lineRect.min.y 
					+ surface->format->BytesPerPixel * lineRect.min.x;

		for (	nPixel = 0;
				nPixel < w;
				nPixel++ )
		{
			int index = ( lineRect.min.x + nPixel + lineRect.min.y ) % nColors;
			SetPixel(	dst, 
						colors[index].c.red, 
						colors[index].c.green, 
						colors[index].c.blue, 
						colors[index].c.alpha );

			dst += surface->format->BytesPerPixel;
		}
	}
}


void KrPainter::DrawVLine(	int x, int y, int h, 
							U8 red, U8 green, U8 blue ) 
{
	GLASSERT( !(surface->flags & SDL_OPENGL ));
	
	Rectangle2I targetRect;
	targetRect.min.x = 0;
	targetRect.min.y = 0;
	targetRect.max.x = surface->w - 1;
	targetRect.max.y = surface->h - 1;

	Rectangle2I lineRect;
	lineRect.min.x = x;
	lineRect.min.y = y;
	lineRect.max.x = x;
	lineRect.max.y = y + h - 1;

	if ( targetRect.Intersect( lineRect ) )
	{
		lineRect.DoIntersection( targetRect );
		h = lineRect.max.y - lineRect.min.y + 1;

		U8* dst;
		int nPixel;

		// Draw the top and bottom h-line
		dst = (U8*)   surface->pixels
		            + surface->pitch*lineRect.min.y 
					+ surface->format->BytesPerPixel*lineRect.min.x;

		for (	nPixel = 0;
				nPixel < h;
				nPixel++ )
		{
			SetPixel( dst, red, green, blue, KrRGBA::KR_OPAQUE );
			dst += surface->pitch;
		}
	}
}


void KrPainter::DrawVLine(	int x, int y, int h, 
							const KrRGBA* colors, int nColors ) 
{
	GLASSERT( !(surface->flags & SDL_OPENGL ));
	
	Rectangle2I targetRect;
	targetRect.min.x = 0;
	targetRect.min.y = 0;
	targetRect.max.x = surface->w - 1;
	targetRect.max.y = surface->h - 1;

	Rectangle2I lineRect;
	lineRect.min.x = x;
	lineRect.min.y = y;
	lineRect.max.x = x;
	lineRect.max.y = y + h - 1;

	if ( targetRect.Intersect( lineRect ) )
	{
		lineRect.DoIntersection( targetRect );
		h = lineRect.max.y - lineRect.min.y + 1;

		U8* dst;
		int nPixel;

		// Draw the top and bottom h-line
		dst = (U8*)   surface->pixels 
		            + surface->pitch         * lineRect.min.y 
					+ surface->format->BytesPerPixel * lineRect.min.x;
					
		for (	nPixel = 0;
				nPixel < h;
				nPixel++ )
		{
			int index = ( lineRect.min.x + nPixel + lineRect.min.y ) % nColors;
			SetPixel(	dst, 
						colors[index].c.red, 
						colors[index].c.green, 
						colors[index].c.blue, 
						colors[index].c.alpha );
			dst += surface->pitch;
		}
	}
}


void KrPainter::DrawBox(	int x, int y, int w, int h,
							U8 red, U8 green, U8 blue )
{
	GLASSERT( !(surface->flags & SDL_OPENGL ));
	
	DrawHLine( x, y, w, red, green, blue );
	DrawHLine( x, y+h-1, w, red, green, blue );
	DrawVLine( x, y, h, red, green, blue );
	DrawVLine( x+w-1, y, h, red, green, blue );
}


void KrPainter::DrawBox(	int x, int y, int w, int h,
							const KrRGBA* colors, int nColors )
{
	GLASSERT( !(surface->flags & SDL_OPENGL ));

	DrawHLine( x, y, w, colors, nColors );
	DrawHLine( x, y+h-1, w, colors, nColors );
	DrawVLine( x, y, h, colors, nColors );
	DrawVLine( x+w-1, y, h, colors, nColors );
}


void KrPainter::DrawFilledBox(	int x, int y, int w, int h,
								U8 red, U8 green, U8 blue )
{	
	GLASSERT( !(surface->flags & SDL_OPENGL ));

	Rectangle2I rect;
	rect.Set( x, y, x+w-1, y+h-1 );
	
	Rectangle2I screenBound;
	screenBound.Set( 0, 0, surface->w - 1, surface->h - 1 );

	rect.DoIntersection( screenBound );

	for( y=rect.min.y; y<=rect.max.y; ++y )
	{
		U8* target = (U8*) surface->pixels + y * surface->pitch + rect.min.x * surface->format->BytesPerPixel;

		for( x=rect.min.x; x<=rect.max.x; ++x )
		{
			SetPixel( target, red, green, blue, 255 );
			target += surface->format->BytesPerPixel;
		}
	}
}


// -------------- Surface -----------------------
void KrPainter::BreakPixel( int x, int y, 
							U8* r, U8* g, U8* b, U8* a )
{
	GLASSERT( x >=0 && x < surface->w );
	GLASSERT( y >=0 && y < surface->h );

	U8* bits = ( (U8*) surface->pixels 
					 + y * surface->pitch
					 + x * surface->format->BytesPerPixel );

	U32 color;

	if ( surface->format->BytesPerPixel == 2 )
	{
		color = *((U16*) bits);
		SDL_GetRGBA( color, surface->format, r, g, b, a );
	}
	else if ( surface->format->BytesPerPixel == 3 )
	{
		*r = bits[ surface->format->Rshift / 8 ];
		*g = bits[ surface->format->Gshift / 8 ];
		*b = bits[ surface->format->Bshift / 8 ];
		*a = KrRGBA::KR_OPAQUE;
	}
	else if ( surface->format->BytesPerPixel == 4 )
	{
		color = *((U32*) bits);
		SDL_GetRGBA( color, surface->format, r, g, b, a );
	}		
	else
	{
		GLASSERT( 0 );
	}
}


void KrPainter::SetPixel( int x, int y, const KrRGBA& color )
{
	U8* bits = (U8*) surface->pixels 
				+ y * surface->pitch
				+ x * surface->format->BytesPerPixel;

	SetPixel(	bits, 
				color.c.red, 
				color.c.green, 
				color.c.blue, 
				color.c.alpha );
}


void KrPainter::SetPixel( void* bits, U8 red, U8 green, U8 blue, U8 alpha )
{
	GLASSERT( surface->format->BytesPerPixel > 1 );

	U32 pixel = SDL_MapRGBA( surface->format, red, green, blue, alpha );
	switch( surface->format->BytesPerPixel ) 
	{
		case 2:
		{
			*((Uint16 *)(bits)) = (Uint16)pixel;
		}
		break;

		case 3: 
		{	
			*(( (U8*) bits)+surface->format->Rshift/8) = red; 
			*(( (U8*) bits)+surface->format->Gshift/8) = green;
			*(( (U8*) bits)+surface->format->Bshift/8) = blue;
		}
		break;

		case 4:
		{
			*((Uint32 *)(bits)) = (Uint32)pixel;
		}
		break;

		default:	
			GLASSERT( 0 );
	}
}


int KrPainter::CalcTransparentRun( int xmin, int xmax, int y )
{
	U8 r, g, b, a;
	int length = 0;
	int x = xmin;

	BreakPixel( x+0, y, &r, &g, &b, &a );

	while ( a == KrRGBA::KR_TRANSPARENT )
	{	
		length++;
		if ( x + length > xmax )
			break;

		BreakPixel( x+length, y, &r, &g, &b, &a );
	}
	return length;
}


int KrPainter::CalcTransparentColumn( int ymin, int ymax, int x )
{
	U8 r, g, b, a;
	int length = 0;
	int y = ymin;

	BreakPixel( x, y, &r, &g, &b, &a );

	while ( a == KrRGBA::KR_TRANSPARENT )
	{	
		++length;
		if ( y + length > ymax )
			break;

		BreakPixel( x, y+length, &r, &g, &b, &a );
	}
	return length;
}


int KrPainter::CalcNotTransparentRun( int xmin, int xmax, int y )
{
	U8 r, g, b, a;
	int length = 0;
	int x = xmin;

	BreakPixel( x+0, y, &r, &g, &b, &a );

	while ( a != KrRGBA::KR_TRANSPARENT )
	{	
		length++;
		if ( x + length > xmax )
			break;

		BreakPixel( x+length, y, &r, &g, &b, &a );
	}
	return length;
}


int KrPainter::CalcOpaqueRun( int xmin, int xmax, int y )
{
	U8 r, g, b, a;
	int length = 0;
	int x = xmin;

	BreakPixel( x+0, y, &r, &g, &b, &a );

	while ( a == KrRGBA::KR_OPAQUE )
	{	
		length++;
		if ( x + length > xmax )
			break;

		BreakPixel( x+length, y, &r, &g, &b, &a );
	}
	return length;
}


int KrPainter::CalcTranslucentRun( int xmin, int xmax, int y )
{
	U8 r, g, b, a;
	int length = 0;
	int x = xmin;

	BreakPixel( x+0, y, &r, &g, &b, &a );

	while ( a != KrRGBA::KR_OPAQUE && a != KrRGBA::KR_TRANSPARENT )
	{	
		length++;
		if ( x + length > xmax )
			break;

		BreakPixel( x+length, y, &r, &g, &b, &a );
	}
	return length;
}


int KrPainter::FindPixel( int x, int y, int dx, int dy, KrRGBA color, bool useAlpha, bool invert )
{
	int count = 0;
	if ( !useAlpha ) color.c.alpha = 0;

	while( x >= 0 && x < surface->w && y >= 0 && y < surface->h )
	{
		KrRGBA rgba;
		BreakPixel( x, y, &rgba );
		if ( !useAlpha ) rgba.c.alpha = 0;

		if ( !invert )
		{
			if ( rgba == color )
				return count;
		}
		else
		{
			if ( rgba != color )
				return count;
		}
		
		x += dx;
		y += dy;
		++count;
	}
	return -1;
}


KrPaintInfo::KrPaintInfo( KrRGBA* memory, int _width, int _height )
{
	// Make sure the KrRGBA are okay:
	GLASSERT( sizeof( KrRGBA ) == 4 );

	needToFreeSurface = true;
	openGL = false;

	int redShift   = 8 * ( ( (U8*) &memory->c.red )   - ( (U8*) memory ) );
	int greenShift = 8 * ( ( (U8*) &memory->c.green ) - ( (U8*) memory ) );
	int blueShift  = 8 * ( ( (U8*) &memory->c.blue )  - ( (U8*) memory ) );
	int alphaShift = 8 * ( ( (U8*) &memory->c.alpha ) - ( (U8*) memory ) );

	int redMask    = 0xff << redShift;
	int greenMask  = 0xff << greenShift;
	int blueMask   = 0xff << blueShift;
	int alphaMask  = 0xff << alphaShift;

	surface = SDL_CreateRGBSurfaceFrom( memory,
										_width, _height, 32,
										4 * _width,		// pitch
										redMask,
										greenMask,
										blueMask,
										alphaMask );
	
	// Can always us the fast blitter: we're copying form
	// RGBAs.
	Paint_Simple_NoAlpha = KrPaintRGBA_Simple_NoAlpha;
	Paint_Alpha_NoAlpha  = KrPaintRGBA_Alpha_NoAlpha;
	Paint_Color_NoAlpha  = KrPaintRGBA_Color_NoAlpha;
	Paint_Full_NoAlpha   = KrPaintRGBA_Full_NoAlpha;

	Paint_Simple_Alpha   = KrPaintRGBA_Simple_Alpha;
	Paint_Alpha_Alpha    = KrPaintRGBA_Alpha_Alpha;
	Paint_Color_Alpha    = KrPaintRGBA_Color_Alpha;
	Paint_Full_Alpha     = KrPaintRGBA_Full_Alpha;

	PaintRotated_Simple_NoAlpha = KrPaintRGBARotated_Simple_NoAlpha;
	InitCopies();
}


KrPaintInfo::KrPaintInfo( SDL_Surface* screen )
{
	Paint_Simple_NoAlpha = 0;
	Paint_Color_NoAlpha  = 0;
	Paint_Alpha_NoAlpha  = 0;
	Paint_Full_NoAlpha   = 0;

	Paint_Simple_Alpha   = 0;
	Paint_Color_Alpha    = 0;
	Paint_Alpha_Alpha    = 0;
	Paint_Full_Alpha     = 0;

	GLASSERT( screen );

	surface = screen;
	needToFreeSurface	= false;
	openGL				= ( screen->flags & SDL_OPENGL ) != 0;
	InitCopies();

	if ( bytesPerPixel == 4 )
	{
		// Check for fast 32 bit blit.
		KrRGBA memory;
		int redShiftRgb   = 8 * ( ( (U8*) &memory.c.red )   - ( (U8*) &memory ) );
		int greenShiftRgb = 8 * ( ( (U8*) &memory.c.green ) - ( (U8*) &memory ) );
		int blueShiftRgb  = 8 * ( ( (U8*) &memory.c.blue )  - ( (U8*) &memory ) );

		if ( redShiftRgb == redShift && blueShiftRgb == blueShift && greenShiftRgb == greenShift )
		{
			Paint_Simple_NoAlpha = KrPaint32B_Simple_NoAlpha;
//			#ifdef DEBUG
//				GLOUTPUT( "Fast 32 bit blitter.\n" );	
//			#endif
		}
		else
		{
			Paint_Simple_NoAlpha = KrPaint32_Simple_NoAlpha;
//			#ifdef DEBUG
//				GLOUTPUT( "Standard 32 bit blitter.\n" );
//			#endif
		}
		Paint_Alpha_NoAlpha  = KrPaint32_Alpha_NoAlpha;
		Paint_Color_NoAlpha  = KrPaint32_Color_NoAlpha;
		Paint_Full_NoAlpha   = KrPaint32_Full_NoAlpha;

		Paint_Simple_Alpha   = KrPaint32_Simple_Alpha;
		Paint_Alpha_Alpha    = KrPaint32_Alpha_Alpha;
		Paint_Color_Alpha    = KrPaint32_Color_Alpha;
		Paint_Full_Alpha     = KrPaint32_Full_Alpha;

		PaintRotated_Simple_NoAlpha = KrPaint32Rotated_Simple_NoAlpha;
	}
	else if ( bytesPerPixel == 3 )
	{
 		GLASSERT( screen->format->Rshift % 8 == 0 );
		GLASSERT( screen->format->Gshift % 8 == 0 );
		GLASSERT( screen->format->Bshift % 8 == 0 );

		Paint_Simple_NoAlpha = KrPaint24_Simple_NoAlpha;
		Paint_Alpha_NoAlpha  = KrPaint24_Alpha_NoAlpha;
		Paint_Color_NoAlpha  = KrPaint24_Color_NoAlpha;
		Paint_Full_NoAlpha   = KrPaint24_Full_NoAlpha;

		Paint_Simple_Alpha   = KrPaint24_Simple_Alpha;
		Paint_Alpha_Alpha    = KrPaint24_Alpha_Alpha;
		Paint_Color_Alpha    = KrPaint24_Color_Alpha;
		Paint_Full_Alpha     = KrPaint24_Full_Alpha;

		PaintRotated_Simple_NoAlpha = KrPaint24Rotated_Simple_NoAlpha;
	}		
	else if ( bytesPerPixel == 2 )
	{
// 		GLOUTPUT( "Setting up basic 16 bit blitter.\n" );

		Paint_Simple_NoAlpha = KrPaint16_Simple_NoAlpha;
		Paint_Alpha_NoAlpha  = KrPaint16_Alpha_NoAlpha;
		Paint_Color_NoAlpha  = KrPaint16_Color_NoAlpha;
		Paint_Full_NoAlpha   = KrPaint16_Full_NoAlpha;

		Paint_Simple_Alpha   = KrPaint16_Simple_Alpha;
		Paint_Alpha_Alpha    = KrPaint16_Alpha_Alpha;
		Paint_Color_Alpha    = KrPaint16_Color_Alpha;
		Paint_Full_Alpha     = KrPaint16_Full_Alpha;

		PaintRotated_Simple_NoAlpha = KrPaint16Rotated_Simple_NoAlpha;
	}		
	else
	{
		GLASSERT( 0 );	// mode not supported.
	}
}


void KrPaintInfo::InitCopies()
{
	redShift   = surface->format->Rshift;
	greenShift = surface->format->Gshift;
	blueShift  = surface->format->Bshift;
	alphaShift = surface->format->Ashift;
	redMask    = surface->format->Rmask;
	greenMask  = surface->format->Gmask;
	blueMask   = surface->format->Bmask;
	alphaMask  = surface->format->Amask;
	redByte	   = surface->format->Rshift / 8;
	blueByte   = surface->format->Bshift / 8;
	greenByte  = surface->format->Gshift / 8;
	redLoss	   = surface->format->Rloss;
	greenLoss  = surface->format->Gloss;
	blueLoss   = surface->format->Bloss;

	width	   = surface->w;
	height     = surface->h;
	pitch      = surface->pitch;
	bytesPerPixel = surface->format->BytesPerPixel;
	pixels     = surface->pixels;
}


KrPaintFunc KrPaintInfo::GetBlitter( bool alpha, const KrColorTransform cform )
{
	if ( alpha )
		if ( cform.IsIdentity() )
			return Paint_Simple_Alpha;
		else if ( cform.HasAlpha() )
			if ( cform.HasColor() )
				return Paint_Full_Alpha;
			else 
				return Paint_Alpha_Alpha;
		else
			if ( cform.HasColor() )
				return Paint_Color_Alpha;
			else
			{
				GLASSERT( 0 );
				return 0;
			}
	else
		if ( cform.IsIdentity() )
			return Paint_Simple_NoAlpha;
		else if ( cform.HasAlpha() )
			if ( cform.HasColor() )
				return Paint_Full_NoAlpha;
			else
				return Paint_Alpha_NoAlpha;
		else
			if ( cform.HasColor() )
				return Paint_Color_NoAlpha;
			else
			{
				GLASSERT( 0 );
				return 0;
			}			
}


#ifdef KYRA_SUPPORT_OPENGL

void KrPaintInfo::SetOpenGLTextureMode( bool /*sourceAlpha*/, 
										const KrColorTransform cform,
										bool isScaled,
										KrTexture* texture )
{
	// Only use linear scaling if we need it. If we don't, nearest neighbor
	// looks better and is faster too.
	//
	if ( isScaled || texture->Scale() )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{ 
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//	cn	color of pixel in sprite or canvas
	//	c0  color on framebuffer
	//  c'	new color
	//
	//	an	alpha of pixel in sprite or canvas
	//	at	alpha of the transform
	//	
	//	m (multiplier) b (adder) color transforms
	//
	//	c' = cn an at m + an at b + ( 1 - an at ) c0
	//

	GLfloat environmentColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, environmentColor );

	// There may be alpha in the individual pixels.
	// There isn't alpha in the transformation.
	if ( cform.IsIdentity() )
	{
		// c' = c
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	}
	else
	{
		if (	cform.b.c.red == 0
			 && cform.b.c.green == 0
			 && cform.b.c.blue == 0 )
		{
			// We can do a perfect transformation.
			// full noAlpha:	c' = a cn m + a b + (1-a) c0
			// where b == 0
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );		
			glColor4f(	cform.m.Redf(),	//  + ( cform.b.Greenf() + cform.b.Bluef() ) / 2.0f, 
						cform.m.Greenf(), //+ ( cform.b.Redf()   + cform.b.Bluef() ) / 2.0f,
						cform.m.Bluef(), // + ( cform.b.Greenf() + cform.b.Redf() ) / 2.0f,
						cform.b.Alphaf() );
		}
		else
		{
			/*
				Color systems, with alpha channel removed. (It works out, and just adds terms.)

				OpenGL
				Color[value] =  Color[poly] * ( 1 - Color[tex] ) + Color[env] * Color[tex]

						or		Color[tex] ( Color[env] - Color[poly] ) + Color[poly]

				Kyra
				Color[value] =    Color[texture] * m + b

				m = Color[env] - Color[poly]
				b = Color[poly]

				Color[poly] = b
				Color[env]  = m + b
			*/

			// The fragment color. (b)

			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );		

			glColor4f(	cform.b.Redf(),
						cform.b.Greenf(),
						cform.b.Bluef(),
						cform.b.Alphaf() );

			// Use the environment color (Cc) to get the (m) term
			environmentColor[ 0 ] = cform.m.Redf()   + cform.b.Redf();
			environmentColor[ 1 ] = cform.m.Greenf() + cform.b.Greenf();
			environmentColor[ 2 ] = cform.m.Bluef()  + cform.b.Bluef();
			environmentColor[ 3 ] = 1.0f;
			glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, environmentColor );
			GLASSERT( glGetError() == GL_NO_ERROR );

			#ifdef DEBUG
				GLint mode = -1;
				glGetTexEnviv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &mode );
				GLASSERT( mode == GL_BLEND );

				float test[4];
				glGetTexEnvfv(  GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, test );
				GLASSERT( test[0] == environmentColor[ 0 ] );
				GLASSERT( test[1] == environmentColor[ 1 ] );
				GLASSERT( test[2] == environmentColor[ 2 ] );
				GLASSERT( test[3] == environmentColor[ 3 ] );

			#endif
		}
	}
	GLASSERT( glGetError() == GL_NO_ERROR );
}

#endif

#ifdef _MSC_VER
// Tell the compiler that no aliasing is used.
#pragma optimize ( "w", on )
#endif

// ---------------- 32 Bit Blt Routines -------------------------//

// Utility inlines and macros.

// Calculate the color transform.
// params:
//		val		original color value
//		ch		channel( red, ... )
// uses:
//		cform	the color transform
//

#define CFORM( val, ch ) ((( val*cform.m.c.ch ) >> 8 ) + cform.b.c.ch )

//inline U8 CFORM_I( U8 val, int ch, const KrColorTransform& cform )
//{
//	return ( ( ( val*cform.m.array[ch] ) >> 8 ) + cform.b.array[ch] );
//}

// Caluclate the color using alpha.
// params:
//		ch		channel (red, green...)
//		alpha	the alpha value (0-255)
// uses:
//		source	KrRGBA*
//		target	U32* to the target pixel
//		info	KrPaintInfo

#define ALPHA_CH_32( ch, alpha )	(((( alpha * source->c.ch )) + (( 255-alpha ) * (( *target & info->ch##Mask ) >> info->ch##Shift ) ) ) >> 8 )
#define ALPHA_CH_RGBA( ch, alpha )	(((( alpha * source->c.ch )) + (( 255-alpha ) * (( target->c.ch ) >> info->ch##Shift ) ) ) >> 8 )


// Calculate the color using full.
// params:
//		ch		channel (red, green...)
//		Ch		channel (Red, Green...)
//		alpha	the alpha value (0-255)
// uses:
//		source	KrRGBA*
//		target	U32* to the target pixel
//		info	KrPaintInfo

#define FULL_CH_32( ch, alpha )	(((( alpha * CFORM( source->c.ch, ch ) )) + (( 255-alpha ) * (( *target & info->ch##Mask ) >> info->ch##Shift ) ) ) >> 8 )
#define FULL_CH_RGBA( ch, alpha )	(((( alpha * CFORM( source->c.ch, ch ) )) + (( 255-alpha ) * (( target->c.ch ) >> info->ch##Shift ) ) ) >> 8 )


void KrPaint32_Simple_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Simple_NoAlpha" );
	#endif

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );
	U32* target = (U32*) _target;

	while ( nPixel )
	{
		*target =   ( source->c.red   << info->redShift )
				  + ( source->c.green << info->greenShift )
				  + ( source->c.blue  << info->blueShift );
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint32B_Simple_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32B_Simple_NoAlpha" );
	#endif

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );
	GLASSERT( ( unsigned( _target ) & 3 ) == 0 );
	GLASSERT( ( unsigned( source ) & 3 ) == 0 );

//	#if defined( KYRA_ASM ) && defined( _MSC_VER )
//
//		// Microsoft inline assembly.
//		// This doesn't help at all. Left here to discourage me
//		// from playing with it.
//		_asm
//		{
//			mov ecx, dword ptr [nPixel]
//			mov esi, dword ptr [source]		;// load the source into esi
//			mov edi, dword ptr [_target]	;// and the target into edi
//			rep movsd 
//		}
//
//	#else
		// Standard C
		memcpy( _target, source, nPixel << 2 );

//	#endif
}


void KrPaint32_Color_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Color_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	U32* target = (U32*) _target;

	while ( nPixel )
	{	
		// Works, but not speed tested:
		//*target =   ( CFORM_I( source->c.red, KrRGBA::RED, cform )     << info->redShift ) 
		//		  | ( CFORM_I( source->c.green, KrRGBA::GREEN, cform ) << info->greenShift ) 
		//		  | ( CFORM_I( source->c.blue, KrRGBA::BLUE, cform )   << info->blueShift );

		*target =   ( CFORM( source->c.red, red )     << info->redShift ) 
				  | ( CFORM( source->c.green, green ) << info->greenShift ) 
				  | ( CFORM( source->c.blue, blue )   << info->blueShift );
		--nPixel;
		++target;
		++source;
	}
}


void KrPaint32_Alpha_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Alpha_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	U32* target = (U32*) _target;

	while ( nPixel )
	{	
		* target =    ( ALPHA_CH_32( red,   cform.b.c.alpha ) << info->redShift )
					| ( ALPHA_CH_32( green, cform.b.c.alpha ) << info->greenShift )
					| ( ALPHA_CH_32( blue,  cform.b.c.alpha ) << info->blueShift );

		--nPixel;
		++target;
		++source;
	}
}


void KrPaint32_Full_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Full_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	U32* target = (U32*) _target;

	while ( nPixel )
	{	
// 		U8 alpha = ( cform.Alpha() * source->alpha ) >> 8;

		* target =    ( FULL_CH_32( red,   cform.b.c.alpha   )  << info->redShift )
					| ( FULL_CH_32( green, cform.b.c.alpha )  << info->greenShift )
					| ( FULL_CH_32( blue,  cform.b.c.alpha   ) << info->blueShift );
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint32_Simple_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Simple_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	U32* target = (U32*) _target;

 	while ( nPixel )
	{	
		if ( source->c.alpha == 255 )
		{
			*target =   ( source->c.red   << info->redShift ) 
					  | ( source->c.green << info->greenShift ) 
					  | ( source->c.blue  << info->blueShift );
		}
		else if ( source->c.alpha != 0 )
		{
			*target  =    ( ALPHA_CH_32( red, source->c.alpha )   << info->redShift )
						| ( ALPHA_CH_32( green, source->c.alpha ) << info->greenShift )
						| ( ALPHA_CH_32( blue, source->c.alpha )  << info->blueShift );

// 			*target =
// 				    (((( s->c.red   * s->c.alpha ) + ( 255 - s->c.alpha ) * (( *target & info->redMask   ) >> info->redShift   ) ) >> 8 ) << info->redShift ) 
// 				  | (((( s->c.green * s->c.alpha ) + ( 255 - s->c.alpha ) * (( *target & info->greenMask ) >> info->greenShift ) ) >> 8 ) << info->greenShift )
// 				  | (((( s->c.blue  * s->c.alpha ) + ( 255 - s->c.alpha ) * (( *target & info->blueMask  ) >> info->blueShift  ) ) >> 8 ) << info->blueShift );
		
// 			U32 redo =   (( *target & info->redMask   ) >> info->redShift   );
// 			U32 greeno = (( *target & info->greenMask ) >> info->greenShift );
// 			U32 blueo =  (( *target & info->blueMask  ) >> info->blueShift  );
// 			U32 red =   (((( s->c.red   * s->c.alpha ) + ( 255 - s->c.alpha ) * redo ) >> 8 ) );
// 			U32 green = (((( s->c.green * s->c.alpha ) + ( 255 - s->c.alpha ) * greeno ) >> 8 ) );
// 			U32 blue =  (((( s->c.blue  * s->c.alpha ) + ( 255 - s->c.alpha ) * blueo ) >> 8 ) );
// 			*target = ( red << info->redShift ) | ( green << info->greenShift ) | ( blue << info->blueShift );
		}
		--nPixel;
		++target;
		++source;
	}
}


void KrPaint32_Color_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Color_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	U32* target = (U32*) _target;

	while ( nPixel )
	{	
		if ( source->c.alpha == 255 )
		{
			*target =   ( CFORM( source->c.red, red )       << info->redShift ) 
					  | ( CFORM( source->c.green, green ) << info->greenShift ) 
					  | ( CFORM( source->c.blue, blue )    << info->blueShift );
		}
		else if ( source->c.alpha != 0 )
		{
			* target =    ( FULL_CH_32( red,   source->c.alpha   )  << info->redShift )
						| ( FULL_CH_32( green, source->c.alpha )  << info->greenShift )
						| ( FULL_CH_32( blue,  source->c.alpha   ) << info->blueShift );
		}
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint32_Alpha_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Alpha_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	U32* target = (U32*) _target;

	while ( nPixel )
	{	
		U8 alpha = ( cform.b.c.alpha * source->c.alpha ) >> 8;

		* target =    ( ALPHA_CH_32( red, alpha )   << info->redShift )
					| ( ALPHA_CH_32( green, alpha ) << info->greenShift )
					| ( ALPHA_CH_32( blue, alpha )  << info->blueShift );

		nPixel--;
		target++;
		source++;
	}
}


void KrPaint32_Full_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Full_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	U32* target = (U32*) _target;

	while ( nPixel )
	{	
		U8 alpha = ( cform.b.c.alpha * source->c.alpha ) >> 8;

		* target =    ( FULL_CH_32( red,   alpha   )  << info->redShift )
					| ( FULL_CH_32( green, alpha )  << info->greenShift )
					| ( FULL_CH_32( blue,  alpha   ) << info->blueShift );
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint32Rotated_Simple_NoAlpha( KrPaintInfo* info, 
									  void*		_target,
									  KrRGBA*	source,
									  int sPitch,        	// pitch in RGBAs, not bytes
									  int nPixel )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32Rotated_Simple_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );
	U32* target = (U32*) _target;

	while ( nPixel )
	{	
		*target =   ( source->c.red   << info->redShift )
				  | ( source->c.green << info->greenShift )
				  | ( source->c.blue  << info->blueShift );
		nPixel--;
		target++;
		source += sPitch;
	}
}


// ---------------- RGBA Rountines. (Use alpha channel) -------- //
void KrPaintRGBA_Simple_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaintRGBA_Simple_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );
	KrRGBA* target = (KrRGBA*) _target;

	memcpy( target, source, sizeof( KrRGBA) * nPixel );
}


void KrPaintRGBA_Color_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Color_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	KrRGBA* target = (KrRGBA*) _target;

	while ( nPixel )
	{	
		target->c.red   = CFORM( source->c.red,   red );
		target->c.green = CFORM( source->c.green, green );
		target->c.blue  = CFORM( source->c.blue,  blue );
		target->c.alpha = 255;

		nPixel--;
		target++;
		source++;
	}
}


void KrPaintRGBA_Alpha_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Alpha_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	KrRGBA* target = (KrRGBA*) _target;

	while ( nPixel )
	{	
		target->c.red   = ALPHA_CH_RGBA( red,   cform.b.c.alpha );
		target->c.green = ALPHA_CH_RGBA( green, cform.b.c.alpha );
		target->c.blue  = ALPHA_CH_RGBA( blue,  cform.b.c.alpha );
		target->c.alpha = cform.b.c.alpha;

		nPixel--;
		target++;
		source++;
	}
}


void KrPaintRGBA_Full_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Full_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	KrRGBA* target = (KrRGBA*) _target;

	while ( nPixel )
	{	
		target->c.red   = FULL_CH_RGBA( red,   cform.b.c.alpha );
		target->c.green = FULL_CH_RGBA( green, cform.b.c.alpha );
		target->c.blue  = FULL_CH_RGBA( blue,  cform.b.c.alpha );
		target->c.alpha = cform.b.c.alpha;
		nPixel--;
		target++;
		source++;
	}
}


void KrPaintRGBA_Simple_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Simple_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	KrRGBA* target = (KrRGBA*) _target;

 	while ( nPixel )
	{	
		if ( source->c.alpha == 255 )
		{
			*target = *source;
		}
		else if ( source->c.alpha != 0 )
		{
			target->c.red   = ALPHA_CH_RGBA( red, source->c.alpha );
			target->c.green	= ALPHA_CH_RGBA( green, source->c.alpha );
			target->c.blue  = ALPHA_CH_RGBA( blue, source->c.alpha );
			target->c.alpha = source->c.alpha;
		}
		nPixel--;
		target++;
		source++;
	}
}


void KrPaintRGBA_Color_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Color_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	KrRGBA* target = (KrRGBA*) _target;

	while ( nPixel )
	{	
		if ( source->c.alpha == 255 )
		{
			target->c.red   = CFORM( source->c.red, red );
			target->c.green = CFORM( source->c.green, green );
			target->c.blue	= CFORM( source->c.blue, blue );
			target->c.alpha = 255;
		}
		else if ( source->c.alpha != 0 )
		{
			target->c.red   = FULL_CH_RGBA( red,   source->c.alpha );
			target->c.green = FULL_CH_RGBA( green, source->c.alpha );
			target->c.blue  = FULL_CH_RGBA( blue,  source->c.alpha );
			target->c.alpha = source->c.alpha;
		}
		nPixel--;
		target++;
		source++;
	}
}


void KrPaintRGBA_Alpha_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Alpha_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	KrRGBA* target = (KrRGBA*) _target;

	while ( nPixel )
	{	
		U8 alpha = ( cform.b.c.alpha * source->c.alpha ) >> 8;

		target->c.red   = ALPHA_CH_RGBA( red, alpha );
		target->c.green = ALPHA_CH_RGBA( green, alpha );
		target->c.blue  = ALPHA_CH_RGBA( blue, alpha );
		target->c.alpha = alpha;

		nPixel--;
		target++;
		source++;
	}
}


void KrPaintRGBA_Full_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32_Full_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );

	KrRGBA* target = (KrRGBA*) _target;

	while ( nPixel )
	{	
		U8 alpha = ( cform.b.c.alpha * source->c.alpha ) >> 8;

		target->c.red   = FULL_CH_RGBA( red, alpha );
		target->c.green	= FULL_CH_RGBA( green, alpha );
		target->c.blue  = FULL_CH_RGBA( blue,  alpha   );
		target->c.alpha = alpha;

		nPixel--;
		target++;
		source++;
	}
}


void KrPaintRGBARotated_Simple_NoAlpha( KrPaintInfo* info, 
									  void*		_target,
									  KrRGBA*	source,
									  int sPitch,        	// pitch in RGBAs, not bytes
									  int nPixel )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint32Rotated_Simple_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 4 );
	GLASSERT( nPixel >= 0 );
	KrRGBA* target = (KrRGBA*) _target;

	while ( nPixel )
	{	
		*target = *source;

		nPixel--;
		target++;
		source += sPitch;
	}
}

// ---------------- 24 Bit Blt Routines -------------------------//

// Utility inlines and macros.

// Caluclate the color using alpha.
// params:
//		ch		channel (red, green...)
//		alpha	the alpha value (0-255)
// uses:
//		source	KrRGBA* 
//		target	U24* to the target pixel
//		info	KrPaintInfo

#define ALPHA_CH_24( ch, alpha ) (((( alpha * source->c.ch )) + (( 255-alpha ) * target[ info->ch##Byte ] ) ) >> 8 )


// Caluclate the color using full.
// params:
//		ch		channel (red, green...)
//		Ch		channel (Red, Green...)
//		alpha	the alpha value (0-255)
// uses:
//		source	KrRGBA*
//		target	U24* to the target pixel
//		info	KrPaintInfo

#define FULL_CH_24( ch, Ch, alpha ) (((( alpha * cform.Transform##Ch( source->c.ch ))) + (( 255-alpha ) * target[ info->ch##Byte ] ) ) >> 8 )


void KrPaint24_Simple_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24_Simple_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );
	U8* target = (U8*) _target;

	/* TLW: Old code...
	while ( nPixel )
	{	
		target[ info->redByte ]   = source->c.red;
		target[ info->greenByte ] = source->c.green;
		target[ info->blueByte ]  = source->c.blue;

		nPixel--;
		target+=3;
		source++;
	}
	*/
	
	//first, do simple tests, keep out of the looping

	//TODO: could break these out into seperate functions so we don't have to switch on 
	//	color shifteverytime. i.e.
	//	
	//	KrPaint24_Simple_NoAlpha_RGB
	//	KrPaint24_Simple_NoAlpha_BGR
	//	...

	//now we can loop without if test
	if(info->redByte == 0)  {
		GLASSERT(info->greenByte == 1);	//other version not currently implemented...!
		GLASSERT(info->blueByte == 2);	//other version not currently implemented...!

		//setup a local var, so it gets put into register for loop counting, or (hopefully) compiles away
		int nLoopCounter = nPixel;
		do  {

			//TODO: these could be unrolled, so we move ptrs 2-4 at a time
			target[ 0 ] = source->c.red;
			target[ 1 ] = source->c.green;
			target[ 2 ] = source->c.blue;

			target = &target[3];
			source++;

		}  while(--nLoopCounter);
	}
	else  {
		GLASSERT(info->redByte == 2);	//other version not currently implemented...!
		GLASSERT(info->greenByte == 1);	//other version not currently implemented...!
		GLASSERT(info->blueByte == 0);	//other version not currently implemented...!

		//setup a local var, so it gets put into register for loop counting, or (hopefully) compiles away
		int nLoopCounter = nPixel;
		do  {

			//TODO: these could be unrolled, so we move ptrs 2-4 at a time
			target[ 0 ] = source->c.blue;
			target[ 1 ] = source->c.green;
			target[ 2 ] = source->c.red;

			target = &target[3];
			source++;

		}  while(--nLoopCounter);
	}
}


void KrPaint24_Color_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24_Color_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );

	U8* target = (U8*) _target;

	/*TLW: old code
	while ( nPixel )
	{	
		target[ info->redByte ]   = CFORM( source->c.red, red );
		target[ info->greenByte ] = CFORM( source->c.green, green );
		target[ info->blueByte ]  = CFORM( source->c.blue, blue );

		nPixel--;
		target+=3;
		source++;
	}
	*/
	
	//first, do simple tests, keep out of the looping

	//TODO: could break these out into seperate functions so we don't have to switch on 
	//	color shifteverytime. i.e.
	//	
	//	KrPaint24_Simple_NoAlpha_RGB
	//	KrPaint24_Simple_NoAlpha_BGR
	//	...

	//now we can loop without if test
	if(info->redByte == 0)  {
		GLASSERT(info->greenByte == 1);	//other version not currently implemented...!
		GLASSERT(info->blueByte == 2);	//other version not currently implemented...!

		//setup a local var, so it gets put into register for loop counting, or (hopefully) compiles away
		int nLoopCounter = nPixel;
		do  {

			//TODO: these could be unrolled, so we move ptrs 2-4 at a time
			target[ 0 ] = CFORM( source->c.red, red );
			target[ 1 ] = CFORM( source->c.green, green );
			target[ 2 ] = CFORM( source->c.blue, blue );

			target = &target[3];
			source++;

		}  while(--nLoopCounter);
	}
	else  {
		GLASSERT(info->redByte == 2);	//other version not currently implemented...!
		GLASSERT(info->greenByte == 1);	//other version not currently implemented...!
		GLASSERT(info->blueByte == 0);	//other version not currently implemented...!

		//setup a local var, so it gets put into register for loop counting, or (hopefully) compiles away
		int nLoopCounter = nPixel;
		do  {

			//TODO: these could be unrolled, so we move ptrs 2-4 at a time
			target[ 0 ] = CFORM( source->c.blue, blue );
			target[ 1 ] = CFORM( source->c.green, green );
			target[ 2 ] = CFORM( source->c.red, red );

			target = &target[3];
			source++;

		}  while(--nLoopCounter);
	}
}


void KrPaint24_Alpha_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24_Alpha_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );

	U8* target = (U8*) _target;

	/* TLW: old code
	while ( nPixel )
	{	
		target[ info->redByte ]   = ALPHA_CH_24( red, cform.b.c.alpha );
		target[ info->greenByte ] = ALPHA_CH_24( green, cform.b.c.alpha );
		target[ info->blueByte ]  = ALPHA_CH_24( blue, cform.b.c.alpha );

		nPixel--;
		target += 3;
		source++;
	}
	*/
	
	//first, do simple tests, keep out of the looping

	//TODO: could break these out into seperate functions so we don't have to switch on 
	//	color shifteverytime. i.e.
	//	
	//	KrPaint24_Simple_NoAlpha_RGB
	//	KrPaint24_Simple_NoAlpha_BGR
	//	...

	//now we can loop without if test
	if(info->redByte == 0)  {
		GLASSERT(info->greenByte == 1);	//other version not currently implemented...!
		GLASSERT(info->blueByte == 2);	//other version not currently implemented...!

		//setup a local var, so it gets put into register for loop counting, or (hopefully) compiles away
		int nLoopCounter = nPixel;
		do  {

			//TODO: these could be unrolled, so we move ptrs 2-4 at a time
			target[ 0 ] = ALPHA_CH_24( red, cform.b.c.alpha );
			target[ 1 ] = ALPHA_CH_24( green, cform.b.c.alpha );
			target[ 2 ] = ALPHA_CH_24( blue, cform.b.c.alpha );

			target = &target[3];
			source++;

		}  while(--nLoopCounter);
	}
	else  {
		GLASSERT(info->redByte == 2);	//other version not currently implemented...!
		GLASSERT(info->greenByte == 1);	//other version not currently implemented...!
		GLASSERT(info->blueByte == 0);	//other version not currently implemented...!

		//setup a local var, so it gets put into register for loop counting, or (hopefully) compiles away
		int nLoopCounter = nPixel;
		do  {

			//TODO: these could be unrolled, so we move ptrs 2-4 at a time
			target[ 0 ] = ALPHA_CH_24( blue, cform.b.c.alpha );
			target[ 1 ] = ALPHA_CH_24( green, cform.b.c.alpha );
			target[ 2 ] = ALPHA_CH_24( red, cform.b.c.alpha );

			target = &target[3];
			source++;

		}  while(--nLoopCounter);
	}
}


void KrPaint24_Full_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24_Full_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );

	U8* target = (U8*) _target;

	while ( nPixel )
	{	
		target[ info->redByte ]   = FULL_CH_24( red, Red, cform.b.c.alpha );
		target[ info->greenByte ] = FULL_CH_24( green, Green, cform.b.c.alpha );
		target[ info->blueByte ]  = FULL_CH_24( blue, Blue, cform.b.c.alpha );

		nPixel--;
		target+=3;
		source++;
	}
}


void KrPaint24_Simple_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24_Simple_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );

	U8* target = (U8*) _target;

	while ( nPixel )
	{	
		if ( source->c.alpha == 255 )
		{
			target[ info->redByte ]   = source->c.red;
			target[ info->greenByte ] = source->c.green;
			target[ info->blueByte ]  = source->c.blue;
		}
		else if ( source->c.alpha != 0 )
		{
			target[ info->redByte ]   = ALPHA_CH_24( red, source->c.alpha );
			target[ info->greenByte ] = ALPHA_CH_24( green, source->c.alpha );
			target[ info->blueByte ]  = ALPHA_CH_24( blue, source->c.alpha );
		}
		nPixel--;
		target+=3;
		source++;
	}
}


void KrPaint24_Color_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24_Color_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );

	U8* target = (U8*) _target;

	while ( nPixel )
	{	
		if ( source->c.alpha == 255 )
		{
			target[ info->redByte ]   = CFORM( source->c.red, red );
			target[ info->greenByte ] = CFORM( source->c.green, green );
			target[ info->blueByte ]  = CFORM( source->c.blue, blue );
		}
		else if ( source->c.alpha != 0 )
		{
			target[ info->redByte ]   = FULL_CH_24( red, Red, source->c.alpha );
			target[ info->greenByte ] = FULL_CH_24( green, Green, source->c.alpha );
			target[ info->blueByte ]  = FULL_CH_24( blue, Blue, source->c.alpha );
		}
		nPixel--;
		target+=3;
		source++;
	}
}


void KrPaint24_Alpha_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24_Alpha_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );

	U8* target = (U8*) _target;

	while ( nPixel )
	{	
		U8 alpha = ( cform.b.c.alpha * source->c.alpha ) >> 8;


		target[ info->redByte ]   = ALPHA_CH_24( red, alpha );
		target[ info->greenByte ] = ALPHA_CH_24( green, alpha );
		target[ info->blueByte ]  = ALPHA_CH_24( blue, alpha );

		nPixel--;
		target+=3;
		source++;
	}
}


void KrPaint24_Full_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24_Full_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );

	U8* target = (U8*) _target;

	while ( nPixel )
	{	
		U8 alpha = ( cform.b.c.alpha * source->c.alpha ) >> 8;

		target[ info->redByte ]   = FULL_CH_24( red, Red, alpha );
		target[ info->greenByte ] = FULL_CH_24( green, Green, alpha );
		target[ info->blueByte ]  = FULL_CH_24( blue, Blue, alpha );

		nPixel--;
		target+=3;
		source++;
	}
}


void KrPaint24Rotated_Simple_NoAlpha( KrPaintInfo* info, 
									  void*		_target,
									  KrRGBA*	source,
									  int sPitch,
									  int nPixel )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint24Rotated_Simple_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 3 );
	GLASSERT( nPixel >= 0 );
	U8* target = (U8*) _target;

	while ( nPixel )
	{	
		target[ info->redByte ]   = source->c.red;
		target[ info->greenByte ] = source->c.green;
		target[ info->blueByte ]  = source->c.blue;

		nPixel--;
		target+=3;
		source+=sPitch;
	}
}


// ---------------- 16 Bit Blt Routines -------------------------//


// Caluclate the color using alpha.
// params:
//		ch		channel (red, green...)
//		alpha	the alpha value (0-255)
// uses:
//		source	KrRGBA* 
//		target	U32* to the target pixel
//		info	KrPaintInfo

#define ALPHA_CH_16( ch, alpha ) (((( alpha * source->c.ch )) + (( 255-alpha ) * (( *target & info->ch##Mask ) >> info->ch##Shift << info->ch##Loss ) ) ) >> 8 )


// Caluclate the color using full.
// params:
//		ch		channel (red, green...)
//		Ch		channel (Red, Green...)
//		alpha	the alpha value (0-255)
// uses:
//		source	KrRGBA*
//		target	U32* to the target pixel
//		info	KrPaintInfo

#define FULL_CH_16( ch, alpha )	(((( alpha * CFORM( source->c.ch, ch ) )) + (( 255-alpha ) * (( *target & info->ch##Mask ) >> info->ch##Shift << info->ch##Loss ) ) ) >> 8 )

void KrPaint16_Simple_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16_Simple_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );
	U16* target   = (U16*) _target;
	//U16* end  = target + nPixel;

	while ( nPixel )
	{	
		*target =   ( ( source->c.red >> info->redLoss )     << info->redShift ) 
				  | ( ( source->c.green >> info->greenLoss ) << info->greenShift ) 
				  | ( ( source->c.blue >> info->blueLoss )   << info->blueShift );
		--nPixel;
		++target;
		++source;
	}
}


void KrPaint16_Color_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16_Color_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );

	U16* target = (U16*) _target;

	while ( nPixel )
	{	
		*target =   ( CFORM( source->c.red, red ) >> info->redLoss  << info->redShift ) 
				  | ( CFORM( source->c.green, green ) >> info->greenLoss << info->greenShift ) 
				  | ( CFORM( source->c.blue, blue ) >> info->blueLoss << info->blueShift );
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint16_Alpha_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16_Alpha_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );

	U16* target = (U16*) _target;

	while ( nPixel )
	{	
		* target =    ( ALPHA_CH_16( red, cform.b.c.alpha )   >> info->redLoss << info->redShift )
					| ( ALPHA_CH_16( green, cform.b.c.alpha ) >> info->greenLoss << info->greenShift )
					| ( ALPHA_CH_16( blue, cform.b.c.alpha )  >> info->blueLoss << info->blueShift );

		nPixel--;
		target++;
		source++;
	}
}



void KrPaint16_Full_NoAlpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16_Full_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );

	U16* target = (U16*) _target;

	while ( nPixel )
	{	
// 		U8 alpha = ( cform.Alpha() * source->alpha ) >> 8;

		* target =    ( FULL_CH_16( red,   cform.b.c.alpha   )   >> info->redLoss << info->redShift )
					| ( FULL_CH_16( green, cform.b.c.alpha ) >> info->greenLoss << info->greenShift )
					| ( FULL_CH_16( blue,  cform.b.c.alpha   ) >> info->blueLoss << info->blueShift );
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint16_Simple_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16_Simple_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );

	U16* target = (U16*) _target;

	while ( nPixel )
	{	
		if ( source->c.alpha == 255 )
		{
			*target =   ( source->c.red   >> info->redLoss   << info->redShift ) 
					  | ( source->c.green >> info->greenLoss << info->greenShift ) 
					  | ( source->c.blue  >> info->blueLoss  << info->blueShift );
		}
		else if ( source->c.alpha != 0 )
		{
			* target =    ( ALPHA_CH_16( red, source->c.alpha )   >> info->redLoss   << info->redShift )
						| ( ALPHA_CH_16( green, source->c.alpha ) >> info->greenLoss << info->greenShift )
						| ( ALPHA_CH_16( blue, source->c.alpha )  >> info->blueLoss  << info->blueShift );
		}
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint16_Color_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16_Color_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );

	U16* target = (U16*) _target;

	while ( nPixel )
	{	
		if ( source->c.alpha == 255 )
		{
			*target =   ( CFORM( source->c.red, red )       >> info->redLoss   << info->redShift ) 
					  | ( CFORM( source->c.green, green ) >> info->greenLoss << info->greenShift ) 
					  | ( CFORM( source->c.blue, blue )    >> info->blueLoss  << info->blueShift );
		}
		else if ( source->c.alpha != 0 )
		{
			* target =    ( FULL_CH_16( red,   source->c.alpha   )    >> info->redLoss << info->redShift )
						| ( FULL_CH_16( green, source->c.alpha )  >> info->greenLoss << info->greenShift )
						| ( FULL_CH_16( blue,  source->c.alpha   )  >> info->blueLoss << info->blueShift );
		}
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint16_Alpha_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16_Alpha_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );

	U16* target = (U16*) _target;

	while ( nPixel )
	{	
		U8 alpha = ( cform.b.c.alpha * source->c.alpha ) >> 8;

		* target =    ( ALPHA_CH_16( red, alpha )   >> info->redLoss << info->redShift )
					| ( ALPHA_CH_16( green, alpha ) >> info->greenLoss << info->greenShift )
					| ( ALPHA_CH_16( blue, alpha )  >> info->blueLoss << info->blueShift );

		nPixel--;
		target++;
		source++;
	}
}


void KrPaint16_Full_Alpha( KrPaintInfo* info, void* _target, KrRGBA* source, int nPixel, const KrColorTransform cform )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16_Full_Alpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );

	U16* target = (U16*) _target;

	while ( nPixel )
	{	
		U8 alpha = ( cform.b.c.alpha * source->c.alpha ) >> 8;

		* target =    ( FULL_CH_16( red,   alpha   )   >> info->redLoss << info->redShift )
					| ( FULL_CH_16( green, alpha ) >> info->greenLoss << info->greenShift )
					| ( FULL_CH_16( blue,  alpha   ) >> info->blueLoss << info->blueShift );
		nPixel--;
		target++;
		source++;
	}
}


void KrPaint16Rotated_Simple_NoAlpha( KrPaintInfo* info, 
									  void*		_target,
									  KrRGBA*	source,
									  int sPitch,
									  int nPixel )
{
	#ifdef COUNT_PERFORMANCE
	GlPerformance perf( "KrPaint16Rotated_Simple_NoAlpha" );
	#endif 

	GLASSERT( info->bytesPerPixel == 2 );
	GLASSERT( nPixel >= 0 );
	U16* target = (U16*) _target;

	while ( nPixel )
	{	
		*target =   ( ( source->c.red >> info->redLoss )     << info->redShift ) 
				  | ( ( source->c.green >> info->greenLoss ) << info->greenShift ) 
				  | ( ( source->c.blue >> info->blueLoss )   << info->blueShift );
		--nPixel;
		++target;
		source += sPitch;
	}
}

#ifdef _MSC_VER
#pragma optimize ( "w", off )
#endif

void KrPaintInfo::GetBlitterName( KrPaintFunc func, std::string* name )
{
	if ( func == KrPaint32_Simple_NoAlpha ) 
	{	*name = "32_Simple_NoAlpha"; return; }
	else if ( func == KrPaint32B_Simple_NoAlpha ) 
	{	*name = "32B_Simple_NoAlpha"; return; }
	else if ( func == KrPaint32_Color_NoAlpha ) 
	{	*name = "32_Color_NoAlpha"; return; }
	else if ( func == KrPaint32_Alpha_NoAlpha )
	{	*name = "32_Alpha_NoAlpha"; return; }
	else if ( func == KrPaint32_Full_NoAlpha )
	{	*name = "32_Full_NoAlpha"; return; }
	else if ( func == KrPaint32_Simple_Alpha )
	{	*name = "32_Simple_Alpha"; return; }
	else if ( func == KrPaint32_Color_Alpha )
	{	*name = "32_Color_Alpha"; return; }
	else if ( func == KrPaint32_Alpha_Alpha )
	{	*name = "32_Alpha_Alpha"; return; }
	else if ( func == KrPaint32_Full_Alpha )
	{	*name = "32_Full_Alpha"; return; }
	else if ( func == KrPaint24_Simple_NoAlpha )
	{	*name = "24_Simple_NoAlpha"; return; }
	else if ( func == KrPaint24_Color_NoAlpha )
	{	*name = "24_Color_NoAlpha"; return; }
	else if ( func == KrPaint24_Alpha_NoAlpha )
	{	*name = "24_Alpha_NoAlpha"; return; }
	else if ( func == KrPaint24_Full_NoAlpha )
	{	*name = "24_Full_NoAlpha"; return; }
	else if ( func == KrPaint24_Simple_Alpha )
	{	*name = "24_Simple_Alpha"; return; }
	else if ( func == KrPaint24_Color_Alpha )
	{	*name = "24_Color_Alpha"; return; }
	else if ( func == KrPaint24_Alpha_Alpha )
	{	*name = "24_Alpha_Alpha"; return; }
	else if ( func == KrPaint24_Full_Alpha )
	{	*name = "24_Full_Alpha"; return; }
	else if ( func == KrPaint16_Simple_NoAlpha )
	{	*name = "16_Simple_NoAlpha"; return; }
	else if ( func == KrPaint16_Color_NoAlpha )
	{	*name = "16_Color_NoAlpha"; return; }
	else if ( func == KrPaint16_Alpha_NoAlpha )
	{	*name = "16_Alpha_NoAlpha"; return; }
	else if ( func == KrPaint16_Full_NoAlpha )
	{	*name = "16_Full_NoAlpha"; return; }
	else if ( func == KrPaint16_Simple_Alpha )
	{	*name = "16_Simple_Alpha"; return; }
	else if ( func == KrPaint16_Color_Alpha )
	{	*name = "16_Color_Alpha"; return; }
	else if ( func == KrPaint16_Alpha_Alpha )
	{	*name = "16_Alpha_Alpha"; return; }
	else if ( func == KrPaint16_Full_Alpha )
	{	*name = "16_Full_Alpha"; return; }
	else
	{	
		*name = "error";
	}
	return;
}

