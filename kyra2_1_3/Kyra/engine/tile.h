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


#ifndef KYRA_TILE_INCLUDED
#define KYRA_TILE_INCLUDED

#include "image.h"
#include "tileresource.h"


struct KrPaintInfo;
class  KrTileResource;
union KrRGBA;


/**	Tiles are optimized for background tiling. The have
	to be square, and can be rotated and flipped into all 8
	configurations. 
	
	They support alpha but are not optimized for it.

	The rotational properties of Tiles are very useful for 
	some applications that use the Tiles for backgrounds. Tiles
	are good for creating backgrounds and can be very efficient.
	However, if you find yourself thinking that you could use 
	a Sprite or a Tile, use the Sprite. They are much more
	flexible.

	Tiles don't collide, and only scale if cached.
*/

class KrTile : public KrImage
{
  public:
	KrTile( KrTileResource* resource );
	virtual ~KrTile();

	/** A tile has 8 rotations, as follows:
		0:	no rotation
		1:	90 CW
		2:	180 CW
		3:	270 CW
		4:  no rotation then flipped around a horizontal line
		5:  90 CW then flipped around a horizontal line
		6:  180 CW then flipped around a horizontal line
		7:  270 CW then flipped around a horizontal line
	*/
	enum eROTATION  {
		ROTATION_NONE = 0,
		ROTATION_90CW,
		ROTATION_180CW,
		ROTATION_270CW,
		ROTATION_NONE_HFLIP,
		ROTATION_90CW_HFLIP,
		ROTATION_180CW_HFLIP,
		ROTATION_270CW_HFLIP,
		ROTATION_MAX
	};

	void SetRotation( int rotation );
	int  GetRotation()						{ return rotation; }

	/// Tiles are always square. Returns the size of any edge.
	int Size() const	{ return resource->Size(); }


	virtual void Draw(	KrPaintInfo* paintInfo, 
						const grinliz::Rectangle2I& clip,
						int window );

	virtual KrImNode* Clone();

	virtual bool HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int window );

	virtual void CalcTransform( int window );
	virtual void QueryBoundingBox( grinliz::Rectangle2I* boundingBox, int window = 0 );

	virtual KrTile*  ToTile()				{ return this; }
	virtual KrResource* Resource()			{ return resource; }
	KrTileResource* TileResource()			{ return resource; }
	
  protected:
	KrTileResource* resource;

  private:
	int rotation;
};


#endif
