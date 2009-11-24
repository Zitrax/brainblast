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


#include "demos.h"
#include "SDL.h"
#ifdef KYRA_SUPPORT_OPENGL
	#include "SDL_opengl.h"
	#include "../engine/ogltexture.h"
#endif
//#include <string.h>
#include <stdio.h>

GameFactory::GameFactory()
{
	memset( gameTime, 0, sizeof( U32 ) * NUM_GAMES );
	memset( gameFrames, 0, sizeof( U32 ) * NUM_GAMES );
	current = 0;
}


GameTest* GameFactory::CreateGame( int i, SDL_Surface* screen )
{
	GameTest* game = 0;

	if ( i<0 || i>= NUM_GAMES )
		return 0;

	switch ( i )
	{
		case 0: game = new TitleScreen( screen );		break;
		case 1: game = new BemGameNormal( screen );		break;
		case 2: game = new BemGameWindowed( screen );	break;
		case 3: game = new SimpleShooter( screen );		break;
		case 4: game = new WindowShooter( screen );		break;
		case 5: game = new SpaceGameFixed( screen );	break;
		case 6: game = new SpaceGameTracking( screen );	break;
		case 7: game = new TextTest( screen );			break;
		case 8: game = new SinWaveTest( screen );		break;
		case 9: game = new ScalingTest( screen );		break;
		case 10: game = new SpriteTest( screen );		break;
		case 11: game = new TileTest( screen );			break;
	}
	current = i;
	GLOUTPUT(( "------ Game %s created -------\n", game->Name() ));

	if ( game )
		name[ current ] = game->Name();

	#ifdef KYRA_SUPPORT_OPENGL
    	GLASSERT( KrTextureManager::TextureIndex() == 0 );
 	#endif
	return game;
}


void GameFactory::DestroyGame( GameTest* game )
{
	gameFrames[ current ] = game->Frame();
	delete game;
	game = 0;
}

void GameFactory::GetHeader( SDL_Surface* screen, char* buf )
{
	std::string s;
	KrEngine::QueryRenderDesc( screen, &s );
	strcpy( buf, s.c_str() );
}


void GameFactory::PrintTimes( SDL_Surface* screen )
{
	FILE* fp = fopen( "testdata.txt", "w" );
	if ( fp )
	{
		char buf[512];
		GetHeader( screen, buf );
		fprintf( fp, "%s\n", buf );

		int i;
		for( i=0; i<NUM_GAMES; i++ )
		{
			if ( gameFrames[i] <= 1 )
				continue;	// don't print 1 frame tests.

			if ( gameTime[i] == 0 )
				gameTime[i] = 1;

			double fps;
			if ( gameTime[i] )
				fps = 1000.0 * double( gameFrames[i] ) / double( gameTime[i] );
			else
				fps = 0.0;

			fprintf( fp, "Game[%d] %18s: time=%d frames=%d fps=%4.1f\n",
					i,
					name[i].c_str(),
					gameTime[i],
					gameFrames[i],
					fps );
		}
		fclose( fp );
	}
}
