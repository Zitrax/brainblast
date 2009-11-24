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

#ifndef KYRA_RLE_INCLUDED
#define KYRA_RLE_INCLUDED

#include <stdlib.h>

#include "../../grinliz/gltypes.h"
#include "../../grinliz/gldebug.h"
#include "../engine/krmath.h"
#include "../../grinliz/glmemorypool.h"
#include "../util/glbitstream.h"
#include "../../grinliz/glgeometry.h"
#include "SDL.h"
#include "painter.h"

class KrCanvasResource;
class KrTexture;
class KrCollisionMap;



class KrRleSegment
{
	friend class KrRle;
  public:
	KrRleSegment()	: start( 0 ), end( 0 ), rgba( 0 )	{}
	~KrRleSegment();

	void Clear()	{ start = 0; end = 0; rgba = 0; }
	
	U32				Alpha()		{ return flags.IsSet( ALPHA ); }
	U16				Start()		{ return start; }
	U16				End()		{ return end; }
	U16				Len()		{ return end - start + 1; }

	KrRGBA*			RGBA()      { return rgba; }
	grinliz::Flag<U32>		Flags()		{ return flags;	}

	// For creating the RLE in the reader.
	bool Read( GlReadBitStream* reader, KrRGBA minColor, KrRGBA bits );

	// For creating the RLE
	bool Create( KrPaintInfo* surface, 
				 int x, int xMax, int y, int objectOriginX );

	bool Write( GlWriteBitStream* writer, KrRGBA minColor, KrRGBA bits );
	void CalcRange( KrRGBA* minColor, KrRGBA* maxColor );

	#ifdef DEBUG
		static U32 numRGBA;
	#endif

  protected:
//	static LinearMemoryPool*	memoryPool;

	enum {
		ALPHA =			0x01,
		//COMPRESS8BIT =	0x02,	// Set if we can write the length and skip in 8 bits.

		BITS_USED =     3,			// For compression.
		MEMORYPOOL =	0x02,		// Is the rgba out of a memory pool?
	};

	grinliz::Flag<U32>	flags;			
	U16			start;		// measured from the start of the line, which can be a surprise.	
	U16			end;		// measured from the start of the line
	KrRGBA*		rgba;
};


class KrRleLine
{
	friend class KrRle;
  public:
	KrRleLine()	:		nSegments( 0 ), segment( 0 ) {}
	~KrRleLine()		{ if (!flags.IsSet( MEMORYPOOL ) ) delete [] segment; }

	void			Clear()			{	nSegments = 0; segment = 0; }
	U32				Alpha()			{	return flags.IsSet( ALPHA ); }
	int				NumSegments()	{	return nSegments; }
	KrRleSegment*	Segment( int i ){	GLASSERT( i < nSegments );
										GLASSERT( segment );
										return &segment[i]; }
	int				CalcSize()		{	if ( nSegments )
											return ( segment[ nSegments-1 ].End() - segment[ 0 ].Start() + 1 );
										else
											return 0; }

	/*
	Pushed to the RLE
	void Draw( U8* target,				// pointer to beginning of scanline
			   KrPaintInfo* paintInfo,
			   const grinliz::Rectangle2I& bounds,	// bounds of the RLE
			   const grinliz::Rectangle2I& isect,		// intersection of the RLE and DirtyRectangle
			   const KrColorTransform& cForm );	// color transform to use
	*/

	void DrawScaled(U8* target,				// pointer to beginning of line (will be offset from here.)
					KrPaintInfo* paintInfo,
					const grinliz::Rectangle2I& bounds,	// bounds of the RLE
					const grinliz::Rectangle2I& isect,		// intersection of the RLE and DirtyRectangle
					const KrColorTransform& cForm,	// color transform to use
					U32 xInc );

	// For creating the RLE in the reader.
	bool Read( GlReadBitStream* writer, KrRGBA minColor, KrRGBA bits );

	// For creating the RLE
	bool Create( KrPaintInfo* surface, int x, int y, int w );
	bool Write( GlWriteBitStream* writer, KrRGBA minColor, KrRGBA bits );
	void CalcRange( KrRGBA* minColor, KrRGBA* maxColor );

  protected:
	enum {
		ALPHA =			0x01,
		MEMORYPOOL =	0x02,		// Is the semgment allocated out of a pool?

		BITS_USED = 2
	};

	grinliz::Flag<U32>		flags;			
	int				nSegments;		
	
	KrRleSegment*	segment;
};


/*	A basic drawing thing. A Run length encoded image.

	WARNING: The KrAction::GrowFrameArray copies these things
	like structures. It is careful to 0 out old memory, but if
	anything ever has a pointer *back* to the Rle, the pointers
	will be trashed. Could be fixed with a proper copy constructor,
	although that would be difficult.
*/
class KrRle
{
  public:
	// The flags are never used directly by the engine code using
	// this class. However, the tile code is borrowing id's
	// for consistency.
	enum {
		ALPHA =		0x01,
		MEMORYPOOL = 0x02,	// is the line out of a memory pool?
	};

	KrRle()	: line( 0 ), texture( 0 ), collisionMap( 0 )
								{ deltaHotToOrigin.Zero(); size.Zero(); delta.Zero(); }
	~KrRle();
	
	void Draw( KrPaintInfo* paintInfo,
			   const KrMatrix2& matrix,
			   const KrColorTransform& cForm,	// color transform to use
			   const grinliz::Rectangle2I& clipping );
		
	// Reads the RLE from a stream. Returns true if not empty.
	bool Read( SDL_RWops* stream );

	// The encoder uses this to create the sprite. Returns true if not empty.
	bool Create( KrPaintInfo* surface, 
				 int x, int y, int width, int height,
				 int hotx, int hoty,
				 int deltax, int deltay );

	// Writes the RLE sprite to the stream.
	bool Write( SDL_RWops* stream );

	// The x and y "step" for this sprite.
	const grinliz::Vector2I& StepSize()	const	{ return delta; }

	int Width()	const	{ return size.x; }	///< Width, in pixels.
	int Height() const 	{ return size.y; }	///< Height, in pixels.

	bool Alpha() const	{ return flags.IsSet( ALPHA ) ? true : false; }
	const grinliz::Vector2I& Delta() const			{ return delta; }
	grinliz::Vector2I		 Hotspot() const		{	grinliz::Vector2I hot = deltaHotToOrigin;
												hot.x = -hot.x; hot.y = -hot.y;
												return hot;
											}
		
	/*	Given a hotspot x and y, return the bounding box
		for the rle.
	*/
	void CalculateBounds( const KrMatrix2& xForm,
						  grinliz::Rectangle2I* bounds ) const;

	bool HitTestTransformed( int x, int y, int hitFlags );

	// Count all the parts of this object. Used by the encoder.
	void CountComponents( U32* numLines, U32* numSegments, U32* numRGBA );

	KrCanvasResource* CreateCanvasResource( int* hotx, int* hoty );
	KrRle*            CreateScaledRle( GlFixed xScale, GlFixed yScale, int* hotx, int* hoty );
	KrCollisionMap*	  GetCollisionMap( GlFixed xScale, GlFixed yScale );

	// Scary function: basically sets the global memory allocator.
	// The vault will set this, allocate a bunch of sprites,
	// then clear it. Will cause problems in a multi-threaded version...
	static void SetMemoryPools(		grinliz::LinearMemoryPool*	_memoryPoolRgba,
									grinliz::LinearMemoryPool*	_memoryPoolLine,
									grinliz::LinearMemoryPool*	_memoryPoolSegment );
	static bool PoolOut();

	// Treat as private:
	// These are NOT owned by the Rle, but the vault. Set here temporarily.
	static grinliz::LinearMemoryPool*	memoryPoolRGBA;
	static grinliz::LinearMemoryPool*	memoryPoolLine;
	static grinliz::LinearMemoryPool*	memoryPoolSegment;

  protected:
	void DrawScaled(	KrPaintInfo* paintInfo,
						const KrMatrix2& xForm,
						const KrColorTransform& cForm,
						const grinliz::Rectangle2I& clipping );

	void DrawOpenGL(	KrPaintInfo* paintInfo,
						const KrMatrix2& matrix,
						const KrColorTransform& cForm,
						const grinliz::Rectangle2I& clipping );

	grinliz::Flag<U32>	flags;				
	KrRleLine*	line;				// An array of lines, each which can (somewhat) draw themselves.
	grinliz::Vector2I	deltaHotToOrigin;	// Offset between the hotspot and the start of the image
	grinliz::Vector2I	size;				// x and y extends of this RLE
	grinliz::Vector2I   delta;				// motion delta for this RLE
	KrTexture*	texture;			// used for openGL texturing
	KrCollisionMap* collisionMap;	
};



#endif
