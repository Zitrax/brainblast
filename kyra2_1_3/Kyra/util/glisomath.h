/*
Copyright (c) 2000-2003 Lee Thomason (www.grinninglizard.com)

Grinning Lizard Utilities. Note that software that uses the 
utility package (including Lilith3D and Kyra) have more restrictive
licences which applies to code outside of the utility package.


This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/

#ifndef ISOMATH_INCLUDED
#define ISOMATH_INCLUDED

#include "../../grinliz/gldebug.h"

/*
	"Front" View, screen


	     ViewY
  offset   |
	+----------------> ScreenX
	|	   |		|
	|	   |		|
	|	   |		|
	|----------------
    v      |
   SceenY  |
		   |
  ---------+------------> ViewX
          Oview

	The offset (screen offset) is measured in view coordinates
	from the view origin to the offset point.


	"Side" View, word

	WorldZ
	|
	|
	|     ViewY
	|     /
	|    /
	|   / 
	|  /
	| /
	|/
	+---------------------WorldY
	|\_
	|  \_
	|    \_ ViewZ


	"Top" View, Map (Should be right angles)

			  WorldY
			   /|\
			  /	| \
			 /	|  \
		MapY \	|  /Mapx
			  \	| /
			   \|/
	---------------------- WorldX
*/

#include "../../grinliz/gltypes.h"
#include "glfixed.h"


/** A utility class for performing math transformations in an
	Isometric view. An Iso view is a quasi-3D view of tiles and
	cubes in 3-D space. In screen view, the cube is exactly 
	twice as wide is it is high.

	The coordinate spaces referenced are:

	Tile: Cubic tile space, or map space. So a "10x10 map"
	      would be equivalent to 10 by 10 "tiles". Tile space
		  is true 3D and has fixed point coordinates.

	World: An intermediate coordinate system.
		   World coordinates are rotated 45 degrees from tile
		   coordinates, but have the same dimensions.
		   
	View: An intermediate coordinate system.
		  View coordinates are rotated to world, but
		  aligned with the screen.
		  
	Screen: A 2-D coordinate system. Pixel coordinates.

	The class can be used via its static functions, which
	provide general transforms. It is usually easier, however,
	to construct an isoMath object and use the "normal" methods.
*/
class GlIsoMath
{
  public:
	/**	Transform tile to screen.
	*/
	static void TileToScreen(	GlFixed tileX, GlFixed tileY, GlFixed tileZ,
								int tileWidth, int tileHeight,
								int screenOffsetX, int screenOffsetY,
								GlFixed* screenX, GlFixed* screenY );

	/** Transform from screen coordinates to tile coordinates,
		if you know the tileZ of the transform, as input.
	*/
	static void ScreenToFlatTile(	GlFixed screenX, GlFixed screenY, GlFixed tileZ,
									int tileWidth, int tileHeight,
									int screenOffsetX, int screenOffsetY,
									GlFixed* tileX, GlFixed* tileY );

	/** Tile to world. Generally an internal transformation. Note 
		that this can introduct rounding errors!
	*/
	static void TileToWorld(	GlFixed tileX, GlFixed tileY, GlFixed tileZ,
								GlFixed* worldX, GlFixed* worldY, GlFixed* worldZ );

// 	/** Tile to view.
// 	*/
// 	static void TileToView(		GlFixed tileX, GlFixed tileY, GlFixed tileZ,
// 								int tileWidth, int tileHeight,
// 								int* viewX, int* viewY, int* viewZ );
// 

	/** A more useable transformation object. Should be followed by
		SetScreen... in order to view in a useful way.
	*/
	GlIsoMath( int tileWidth, int tileHeight );

	/** Set the screen so that the bottom center of the screen
		is at tile 0,0
	*/
	void SetScreenBaseToOrigin( int screenW, int screenH );

	/**	Set the screen center so that it "looks" at the given tile. */
	void SetScreenCenterToTile( int screenW, int screenH, 
								int tileX, int tileY, int tileZ );

	/** Set an arbitrary screen point to an arbitrary tile. */
	void SetScreenToTile( int screenX, int screenY,
						  int tileX, int tileY, int tileZ );

	/// Coverts from tile coordinates to screen coordinates.
	void TileToScreen(	GlFixed tileX, GlFixed tileY, GlFixed tileZ,
						int* screenX, int* screenY ) const;

	/**	Converts from screen to map, for a known mapZ = worldZ.
		The map doesn't have to be flat, just the area of transformation.
	*/
	void ScreenToFlatTile(	int screenX, int screenY, GlFixed tileZ,
							GlFixed* tileX, GlFixed* tileY ) const;

  private:
	int tileWidth, tileHeight;
	int screenOffsetX, screenOffsetY;
};

#endif
