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


#ifndef KYRA_TILERESOURCE_INCLUDED
#define KYRA_TILERESOURCE_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <vector>
#include "kyraresource.h"
#include "../engine/krmath.h"
#include "pixelblock.h"

/** Practically, a tile resource is used to create a tile.
	It contains a pixel block for its data.
*/
class KrTileResource : public KrResource
{
  public:
	// Create by reading from a .dat file
	KrTileResource( U32 size, SDL_RWops* data );

	KrTileResource( const std::string& name,
					KrPaintInfo* surface,
					int x, int y, int size );

	virtual ~KrTileResource();

	virtual	U32 Type()						{ return KYRATAG_TILE; }
	virtual const std::string&	TypeName()	{ return tileName; }
	virtual KrTileResource* ToTileResource(){ return this; }

	/// The width and height must be the same for a tile.
	int     Size() const					{ return pixelBlock[0]->Width(); }
	/// Return true if this Tile supports alpha.
	bool	Alpha()							{ return pixelBlock[0]->Alpha(); }

	// -- internal -- //
	void Draw( KrPaintInfo* paintInfo,
			   const KrMatrix2& matrix,
			   int rotation, 
			   const KrColorTransform& cForm,
			   const grinliz::Rectangle2I& clipping,
			   int quality );

	/*  Do a HitTest (see KrImageTree::HitTest) in transformed 
		coordinates. So the tree object that made this call
		has already transformed the x and y into local pixel coords.
	*/
	bool HitTestTransformed( int rotation, int x, int y, int hitFlags );

	void CalculateBounds( const KrMatrix2&, grinliz::Rectangle2I* bounds ) const;
	
	virtual void CacheScale( GlFixed xScale, GlFixed yScale );
	virtual bool IsScaleCached( GlFixed xScale, GlFixed yScale );
	virtual void FreeScaleCache();

	virtual void Save( KrEncoder* );

	KrRGBA* GetPixels(const int nRotation, int& nPitchX, int& nPitchY);

  private:
	struct CachedBlock
	{
		GlFixed xScale,
				yScale;
		KrPixelBlock* pixelBlock;

		bool operator==( const CachedBlock& rhs )	{ GLASSERT( 0 ); return false; }	// be nice to buggy compilers.
	};
	std::vector< CachedBlock > cache;
	
	void CalcSourceAndPitch( KrPixelBlock* pblock, int rotation, KrRGBA** source, int* sPitchX, int* sPitchY );

	static const std::string tileName;
	KrPixelBlock*	pixelBlock[4];	
};


#endif
