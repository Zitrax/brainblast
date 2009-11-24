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

#ifndef SURFACE_INCLUDED
#define SURFACE_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <string>

#include "SDL.h"
#include "../../grinliz/gltypes.h"
#include "color.h"
#include "../engine/krmath.h"
#include "../engine/kyrabuild.h"

struct KrPaintInfo;
class KrTexture;

 
/** Functions that draw on a surface.
	This class in generally quite slow, compared to the engine
	itself, but useful for general utility functions that don't
	need to be fast.
*/
class KrPainter
{
  public:
	KrPainter( SDL_Surface* _surface );
  	KrPainter( KrPaintInfo* _info );

	~KrPainter();

	/**  A slow but universal pixel set. 
		 NOTE: Uses the alpha channel for 32 bit color.
		       Alpha is in conventional units.
	*/
	void SetPixel( int x, int y, const KrRGBA& color );

	/// Same as above, alternate parameters.
	void SetPixel( void* target, U8 red, U8 green, U8 blue, U8 alpha );

	/// Draw a hollow box.
	void DrawBox(	int x, int y, int w, int h,
					U8 red, U8 green, U8 blue );

	/// Draw a hollow box.
	void DrawBox(	int x, int y, int w, int h,
					const KrRGBA* colors, int nColors );

	/// Draw a hollow box.
	void DrawFilledBox(	int x, int y, int w, int h,
						U8 red, U8 green, U8 blue );

	/// Draw a vertical line.
	void DrawVLine(	int x, int y, int h, 
					U8 red, U8 green, U8 blue );

	/// Draw a vertical line.
	void DrawVLine(	int x, int y, int h, 
					const KrRGBA* colors, int nColors );


	/// Draw a horizontal line.
	void DrawHLine(	int x, int y, int h, 
					U8 red, U8 green, U8 blue ); 		

	/// Draw a horizontal line.
	void DrawHLine(	int x, int y, int h, 
					const KrRGBA* colors, int nColors ); 		

	/*  Given the starting location, and a maximum width,
		returns the number of consectutive transparent pixels.
	*/
	int CalcTransparentRun( int xmin, int xmax, int y );

	/*  Given the starting location, and a maximum width,
		returns the number of consectutive transparent pixels.
	*/
	int CalcTransparentColumn( int ymin, int ymax, int x );

	/*  Given the starting location, and a maximum width,
		returns the number of consectutive pixels which are
		not transparent.
	*/
	int CalcNotTransparentRun( int xmin, int xmax, int y );

	/*  Given the starting location, and a maximum width,
		returns the number of consectutive pixels which are
		not transparent.
	*/
	int CalcOpaqueRun( int xmin, int xmax, int y );

	/*  Given the starting location, and a maximum width,
		returns the number of consectutive pixels which are
		not transparent.
	*/
	int CalcTranslucentRun( int xmin, int xmax, int y );

	/*	Finds a pixel of a given color.
		x, y:	where to start looking
		dx, dy: the step to travel. (1,1) would be a diagonal to the upper left
		color:	the pixel color to look for
		useAlpha: if true, will use the alpha channel as part of the comparison
		invert: flip the logic. Find the first pixel that is NOT == color

		returns: -1 if not found, or the number of steps taken if the pixel was found.
	*/
	int FindPixel( int x, int y, int dx, int dy, KrRGBA color, bool useAlpha, bool invert = false );

	/** Breaks a pixel into r, g, b, and a components. 
		NOTE: alpha is returned in conventional units, 
			  0=transparent, 255=opaque.
	*/
	void BreakPixel( int x, int y, U8* r, U8* g, U8* b, U8* a );

	/** Breaks a pixel into r, g, b, and a components. 
		NOTE: alpha is returned in conventional units, 
			  0=transparent, 255=opaque.
	*/
	void BreakPixel( int x, int y, KrRGBA* rgba )	{ BreakPixel( x, y, &rgba->c.red, &rgba->c.green, &rgba->c.blue, &rgba->c.alpha ); }

  private:
	SDL_Surface* surface;
};


typedef void (*KrPaintFunc)(	KrPaintInfo* info, 
								void* target, 
								KrRGBA* source, 
								int nPixel, 
								const KrColorTransform cform );

typedef void (*KrPaintFuncRotated)( KrPaintInfo* info, 
								    void*		target,
									KrRGBA*	source,
									int sPitch,
									int nPixel );


struct KrPaintInfo
{
	// Initialize to paint to a surface:
	KrPaintInfo( SDL_Surface* screen );

	// Initialize to paint to a block of KrRGBA:
	KrPaintInfo( KrRGBA* memory, int width, int height );

	~KrPaintInfo()	{ if ( needToFreeSurface ) SDL_FreeSurface( surface ); }

	// Based on the cform and the source alpha,
	// get the correct Bltter. 'sourceAlpha' should be true
	// if there can be alpha anywhere in the source.
	KrPaintFunc GetBlitter( bool sourceAlpha, 
							const KrColorTransform cform );

	// Like get blitter, for openGl. Sets up the texture and color of the poly.
	void SetOpenGLTextureMode(	bool sourceAlpha,
								const KrColorTransform cform,
								bool isScaled,
								KrTexture* texture );

	void GetBlitterName( KrPaintFunc func, std::string* name );

	bool OpenGL()	{ return openGL; }

	int width;
	int height;
	int pitch;
	int bytesPerPixel;
	void* pixels;
	bool openGL;

	// Copies of the SDL stuff to avoid dereferencing in the blitters
	// 32 and 16 bit modes.
	U8  redShift;
	U8  greenShift;
	U8  blueShift;
	U8  alphaShift;

	U32	redMask;		
	U32	greenMask;		
	U32	blueMask;
	U32 alphaMask;		
	
	U8  redLoss;
	U8	greenLoss;
	U8  blueLoss;
	U8	alphaLoss;
 	
	// 24 bit mode.
	U8	redByte;
	U8	greenByte;
	U8	blueByte;

	KrPaintFunc Paint_Simple_NoAlpha;
	KrPaintFunc Paint_Color_NoAlpha;
	KrPaintFunc Paint_Alpha_NoAlpha;
	KrPaintFunc Paint_Full_NoAlpha;

	KrPaintFunc Paint_Simple_Alpha;
	KrPaintFunc Paint_Color_Alpha;
	KrPaintFunc Paint_Alpha_Alpha;
	KrPaintFunc Paint_Full_Alpha;

	KrPaintFuncRotated PaintRotated_Simple_NoAlpha;

	SDL_Surface*	surface;

  private:
	bool needToFreeSurface;
	void InitCopies();
};

// Format:
// KrPaint #Bits Target Source
//
// where Target can be:
//		Simple:	no color transform
//		Color:  color transform, but alpha==255
//		Alpha:  no RGB transform, but has alpha
//		Full:	RGB and alpha transform
//
// where Source image can be:
//		NoAlpha: all alpha values in source == 255
//		Alpha: some alpha values are not 255
//

void KrPaint32_Simple_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint32B_Simple_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );

void KrPaint32_Color_NoAlpha(  KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint32_Alpha_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint32_Full_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint32_Simple_Alpha(   KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint32_Color_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint32_Alpha_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint32_Full_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );

void KrPaintRGBA_Simple_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaintRGBA_Color_NoAlpha(  KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaintRGBA_Alpha_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaintRGBA_Full_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaintRGBA_Simple_Alpha(   KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaintRGBA_Color_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaintRGBA_Alpha_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaintRGBA_Full_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );

void KrPaint24_Simple_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint24_Color_NoAlpha(  KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint24_Alpha_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint24_Full_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint24_Simple_Alpha(   KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint24_Color_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint24_Alpha_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint24_Full_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );

void KrPaint16_Simple_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint16_Color_NoAlpha(  KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint16_Alpha_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint16_Full_NoAlpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint16_Simple_Alpha(   KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint16_Color_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint16_Alpha_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );
void KrPaint16_Full_Alpha( KrPaintInfo* info, void* target, KrRGBA* source, int nPixel, const KrColorTransform cform );

void KrPaintRGBARotated_Simple_NoAlpha(	KrPaintInfo* info, 
										void*		target,
										KrRGBA*	source,
										int sPitch,
										int nPixel );
void KrPaint32Rotated_Simple_NoAlpha( KrPaintInfo* info, 
									  void*		target,
									  KrRGBA*	source,
									  int sPitch,
									  int nPixel );
void KrPaint24Rotated_Simple_NoAlpha( KrPaintInfo* info, 
									  void*		target,
									  KrRGBA*	source,
									  int sPitch,
									  int nPixel );
void KrPaint16Rotated_Simple_NoAlpha( KrPaintInfo* info, 
									  void*		target,
									  KrRGBA*	source,
									  int sPitch,
									  int nPixel );

#endif

