#include "../engine/kyra.h"
#include "layout.h"
#include "xmlutil.h"
#include "SDL.h"
#include "focusmanager.h"
#include "../spriteed/CONSOLEFONT.H"
#include "../engine/worldmap.h"
#include "logic.h"

int main( int argc, char *argv[] )
{
	const SDL_version* sdlVersion = SDL_Linked_Version();
	if ( sdlVersion->minor < 2 )
	{
		printf( "SDL version must be at least 1.2.0\n" );
		GLASSERT( 0 );
		exit( 254 );
	}

	if ( argc < 2 )
	{
		printf( "Usage: krmapmaker map.xml\n" );
		exit( 253 );
	}

	TiXmlDocument doc( argv[1] );
	doc.LoadFile();

	XmlUtil xmlUtil( &doc );
	if ( !xmlUtil.IsValidMap() )
	{
		printf( "Not a valid map file.\n" );
		exit( 252 );
	}

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) < 0 ) {
		#ifdef DEBUG
			GLOUTPUT( "Couldn't initialize SDL: %s\n",SDL_GetError());
		#endif
		exit(255);
	}

	SDL_WM_SetCaption( "Kyra MapMaker", 0 );

	int screenX = 800;
	int screenY = 600;

	SDL_Surface* screen = SDL_SetVideoMode( screenX, screenY, 32, SDL_SWSURFACE );

	if ( screen )
	{
		KrResourceVault fontVault;
		//fontVault.LoadDatFileFromMemory( fontDat, sizeof( fontDat ) );
		KrFontResource* consoleFontRes =  KrEncoder::CreateFixedFontResource( "CONSOLE", CONSOLEFONT_DAT, CONSOLEFONT_SIZE );

		Layout layout( screen, consoleFontRes );
		if ( layout.Engine()->Vault()->LoadDatFile( xmlUtil.DatFileName().c_str() ) != true )
		{
			printf( "Couldn't load dat file.\n" );
			exit( 250 );
		}

		KrSquareWorldMap* worldMap = new KrSquareWorldMap( 100, 100, 100 );	// fixme hardcoded!
		layout.Engine()->Tree()->AddNode( layout.Map(), worldMap );
		worldMap->SetPos( 0, screenY-1, Layout::MAIN_VIEW );

		xmlUtil.Init( layout.Engine()->Vault(), layout.Engine(), layout.Map(), worldMap );
//		xmlUtil.InsertLayers( layout.LayerBox() );

		UILogic logic( &layout, &xmlUtil );
		layout.SetLayer( xmlUtil.GetLayer( "null" ), 0 );		

		KrRGBA white;
		white.Set( 200, 200, 200 );
		KrBox* whereBox = new KrBox( layout.Engine()->ScreenBounds( Layout::MAIN_VIEW ).Width(),
									 layout.Engine()->ScreenBounds( Layout::MAIN_VIEW ).Height(),
									 white, 
									 KrBoxResource::OUTLINE );
		layout.Engine()->Tree()->AddNode( 0, whereBox );
		whereBox->SetVisible( false );
		whereBox->SetVisible( true, Layout::MINIMAP_VIEW );

		layout.Engine()->Draw();

		SDL_Event event;

		while ( SDL_WaitEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
				break;

			KrEventManager::Instance()->HandleEvent( event, layout.Engine() );

			bool mapChanged = false;

			if (	( event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN )
				 && layout.Engine()->GetWindowFromPoint( event.motion.x, event.motion.y ) == Layout::MAIN_VIEW )
			{
				KrVector2T< GlFixed > map;
				worldMap->ScreenToMap( event.motion.x, event.motion.y, &map, Layout::MAIN_VIEW );
				layout.DisplayMapCoords( map.x.ToFloat(), map.y.ToFloat() );

				if(			event.type == SDL_MOUSEBUTTONDOWN 
					   ||  ( event.type == SDL_MOUSEMOTION && event.motion.state == 1 ) )
				{
					if ( logic.CurrentImage() )
					{
						worldMap->SetLoc( map.x.ToInt(), map.y.ToInt(), 
										  logic.CurrentImage()->Clone()->ToImage() );
						mapChanged = true;
					}
				}
			}
			else if ( event.type == SDL_KEYDOWN )
			{
				if ( event.key.keysym.sym == SDLK_UP )
				{
					worldMap->SetPos( worldMap->X(), worldMap->Y() + worldMap->TileHeight(), Layout::MAIN_VIEW );
					mapChanged = true;
				}
				else if ( event.key.keysym.sym == SDLK_DOWN )
				{
					worldMap->SetPos( worldMap->X(), worldMap->Y() - worldMap->TileHeight(), Layout::MAIN_VIEW );
					mapChanged = true;
				}				
				else if ( event.key.keysym.sym == SDLK_RIGHT )
				{
					worldMap->SetPos( worldMap->X() - worldMap->TileWidth(), worldMap->Y(), Layout::MAIN_VIEW );
					mapChanged = true;
				}
				else if ( event.key.keysym.sym == SDLK_LEFT )
				{
					worldMap->SetPos( worldMap->X() + worldMap->TileWidth(), 
									  worldMap->Y(), 
									  Layout::MAIN_VIEW );
					mapChanged = true;
				}
				else if ( event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_7 )
				{
					layout.SetRotation( event.key.keysym.sym - SDLK_0 );
					logic.SetRotation( event.key.keysym.sym - SDLK_0 );
				}
			}

			if ( mapChanged )
			{
				layout.Engine()->Tree()->Walk();
				KrRect bounds = worldMap->CompositeBounds( Layout::MAIN_VIEW );

//				layout.Map()->SetPos(	-bounds.xmin, // - layout.Engine()->ScreenBounds( Layout::MAIN_VIEW ).xmin,
//										-bounds.ymin, // - layout.Engine()->ScreenBounds( Layout::MAIN_VIEW ).ymin,
//										Layout::MINIMAP_VIEW );

				GlFixed scale = GlMin(  GlFixed( layout.Engine()->ScreenBounds( Layout::MINIMAP_VIEW ).Width() ) / GlFixed( bounds.Width() ),
										GlFixed( layout.Engine()->ScreenBounds( Layout::MINIMAP_VIEW ).Height() ) / GlFixed( bounds.Height() ) );
				
				layout.Map()->SetPos(	0,
										( GlFixed( Layout::UI_WIDTH ) / scale ).ToInt(),
										Layout::MINIMAP_VIEW );

				layout.Engine()->Tree()->Root()->SetScale( scale, scale, Layout::MINIMAP_VIEW );
				GLOUTPUT( "Minimap scale set to %f\n", scale.ToFloat() );

				whereBox->SetPos( -worldMap->X(), bounds.Height() - 1 - worldMap->Y() );		
			}

			layout.Engine()->Draw();
		}	
	}
	SDL_Quit();

	return 0;
}

