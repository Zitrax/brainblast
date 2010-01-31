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
#include <math.h>

#include "glisomath.h"

/*static*/
void GlIsoMath::TileToScreen(	GlFixed tileX, GlFixed tileY, GlFixed tileZ,
								int tileWidth, int tileHeight,
								int screenOffsetX, int screenOffsetY,
								GlFixed* screenX, GlFixed* screenY )
{
	// Tile to view:
	GlFixed vX =   tileX * GlFixed( tileWidth / 2 )
				 - tileY * GlFixed( tileWidth / 2 );

	GlFixed vY =   tileX * GlFixed( tileHeight / 2 )
				 + tileY * GlFixed( tileHeight / 2 )
				 + tileZ * GlFixed( tileHeight );

	// View to screen:
// 	GLOUTPUT( "view=%d, %d\n", vX.ToInt(), vY.ToInt() );
	*screenX =  vX + screenOffsetX;
	*screenY = -vY + screenOffsetY;
}


/*static*/
void GlIsoMath::ScreenToFlatTile(	GlFixed screenX, GlFixed screenY, GlFixed tileZ,
									int tileWidth, int tileHeight,
									int screenOffsetX, int screenOffsetY,
									GlFixed* tileX, GlFixed* tileY )
{
	// screen to view:
	GlFixed vX =  screenX - screenOffsetX;
	GlFixed vY = -screenY + screenOffsetY;

	// view to tile:
	*tileX =  ( vX / tileWidth ) + ( vY / tileHeight ) - tileZ;
	*tileY = -( vX / tileWidth ) + ( vY / tileHeight ) - tileZ;

// 	#ifdef DEBUG
// 	GLOUTPUT( "screen=%d,%d offset=%d,%d view=%d,%d tile=%d,%d\n",
// 			  screenX.ToInt(), screenY.ToInt(),
// 			  screenOffsetX, screenOffsetY,
// 			  vX.ToInt(), vY.ToInt(),
// 			  tileX->ToInt(), tileY->ToInt() );
// 	#endif
}


/*static*/
void GlIsoMath::TileToWorld(	GlFixed tileX, GlFixed tileY, GlFixed tileZ,
								GlFixed* worldX, GlFixed* worldY, GlFixed* worldZ )
{
	GlFixed csAlpha( 0.7071067811865475244008443621045 );

	*worldX = csAlpha * tileX - csAlpha * tileY;
	*worldY = csAlpha * tileX + csAlpha * tileY;
	*worldZ = tileZ;
}


// /*static*/
// void GlIsoMath::TileToView(		GlFixed tileX, GlFixed tileY, GlFixed tileZ,
// 								int tileWidth, int tileHeight,
// 								int* viewX, int* viewY, int* viewZ )
// {
// 	*viewX = ( tileX * tileWidth / 2 - tileY * tileHeight / 2 ).ToInt();
// 	*viewY = ( tileX * tileWidth / 2 - tileY * tileHeight / 2 ).ToInt();
// 	*viewZ = ( 
// }


GlIsoMath::GlIsoMath( int _tileWidth, int _tileHeight )
{
	tileWidth = _tileWidth;
	tileHeight = _tileHeight;
	screenOffsetX = 0;
	screenOffsetY = 0;
	
}

void GlIsoMath::SetScreenBaseToOrigin( int screenW, int screenH )
{
	screenOffsetX = screenW / 2;
	screenOffsetY = screenH;
}


void GlIsoMath::SetScreenCenterToTile( int screenW, int screenH, int tileX, int tileY, int tileZ )
{
	GlFixed sx;
	GlFixed sy;

	TileToScreen( tileX, tileY, tileZ, 
				  tileWidth, tileHeight, 
				  0, 0, 
				  &sx, &sy );

	screenOffsetX = screenW / 2 - sx.ToInt();
	screenOffsetY = screenH / 2 - sy.ToInt(); 
}


void GlIsoMath::SetScreenToTile(	int screenX, int screenY,
									int tileX, int tileY, int tileZ )
{
	GlFixed sx;
	GlFixed sy;

	TileToScreen( tileX, tileY, tileZ, 
				  tileWidth, tileHeight, 
				  0, 0, 
				  &sx, &sy );

	screenOffsetX = screenX - sx.ToInt();
	screenOffsetY = screenY - sy.ToInt(); 
}
	

void GlIsoMath::TileToScreen(	GlFixed tileX, GlFixed tileY, GlFixed tileZ,
								int* screenX, int* screenY ) const
{
	GlFixed sx, sy;

	TileToScreen(	tileX, tileY, tileZ,
					tileWidth, tileHeight,
					screenOffsetX, screenOffsetY, 
					&sx, &sy );
	*screenX = sx.ToInt();
	*screenY = sy.ToInt();
}


void GlIsoMath::ScreenToFlatTile(	int screenX, int screenY, GlFixed tileZ,
									GlFixed* mapX, GlFixed* mapY ) const
{
	ScreenToFlatTile(	screenX, screenY, tileZ,
						tileWidth, tileHeight,
						screenOffsetX, screenOffsetY,
						mapX, mapY );
}
