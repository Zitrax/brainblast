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


#include "SDL.h"
#include "../engine/kyra.h"
#include "../gui/console.h"
#include "../engine/encoder.h"


void EncoderOutput( KrResourceVault* vault, const char* prefix )
{
	U32 rmask, gmask, bmask, amask;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0;	// no alpha
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0;	// no alpha
	#endif

    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 32,
												rmask, gmask, bmask, amask);

	KrEngine* engine = new KrEngine( surface );

	// Add a background.
	int i, j;
	KrCanvasResource* backgroundRes = new KrCanvasResource( "", 20, 20, false );
	KrRGBA grey0, grey1;
	grey0.Set( 40, 40, 40 );
	grey1.Set( 0, 0, 0 );

	for( i=0; i<20; ++i )
	{
		for( j=0; j<20; ++j )
		{
			KrRGBA color = ( i/10 + j/10 ) & 0x01 ? grey0 : grey1;
			*( backgroundRes->Pixels() + j*20 + i ) = color;
		}
	}
	backgroundRes->Refresh();

	KrImNode* backNode = new KrImNode();
	engine->Tree()->AddNode( engine->Tree()->Root(), backNode );

	for ( i=0; i<engine->ScreenBounds().Width() / backgroundRes->Width() + 1; i++ )
	{
		for( j=0; j<engine->ScreenBounds().Height() / backgroundRes->Height() + 1; j++ )
		{
			KrCanvas* backCanvas = new KrCanvas( backgroundRes );
			engine->Tree()->AddNode( backNode, backCanvas );
			backCanvas->SetZDepth( -1 );
			backCanvas->SetPos( i*backgroundRes->Width(), j*backgroundRes->Height() );
		}
	}

	int index = 0;

	GlSListIterator< KrResource* > it = vault->GetResourceIterator();
	KrRGBA color;
	GlSList<KrImage*> imageList;		// Create everything, store here, then position.

	for( it.Begin(); !it.Done(); it.Next() )
	{
		KrResource* res = it.Current();
		if ( res->ToSpriteResource() )
		{
			// Also prints fonts.
			if ( res->ToFontResource() )
				color.Set( 100, 100, 255 );
			else
				color.Set( 255, 100, 100 );

			KrSpriteResource* spriteRes = res->ToSpriteResource();

			for( int actionCount = 0; actionCount < spriteRes->NumActions(); ++actionCount )
			{
				KrAction* action = spriteRes->GetActionByIndex( actionCount );

				for( int frameCount = 0; frameCount < action->NumFrames(); ++frameCount )
				{
					KrSprite* sprite = new KrSprite( spriteRes );
					sprite->SetAction( action->Name() );
					sprite->SetFrame( frameCount );

					KrRect rect;
					sprite->QueryBoundingBox( &rect, 0 );

					// don't add empties:
					if ( rect.IsValid() )
					{
						// Now add a bounding box.
						KrBoxResource* boxRes = new KrBoxResource(	"", 
																	rect.Width()+2, rect.Height()+2,
																	&color, 1,
																	KrBoxResource::OUTLINE );
						engine->Vault()->AddResource( boxRes );
						KrBox* box = new KrBox( boxRes );
						box->SetPos( -1+rect.min.x, -1+rect.min.y );	// relative to sprite

						engine->Tree()->AddNode( sprite, box );

						// And the sprite itself (on top of the bounding box)
						engine->Tree()->AddNode( engine->Tree()->Root(), sprite );

						// Now add the hotspot.
						if ( rect.min.x != 0 || rect.min.y != 0 )
						{
							KrRGBA hotColor = color;
							hotColor.c.alpha = 120;
							KrBoxResource* hotResX = new KrBoxResource( "",
																	   rect.Width(), 1,
																	   &hotColor, 1,
																	   KrBoxResource::OUTLINE );

							KrBoxResource* hotResY = new KrBoxResource( "",
																	   1, rect.Height(),
																	   &hotColor, 1,
																	   KrBoxResource::OUTLINE );

							engine->Vault()->AddResource( hotResX );
							engine->Vault()->AddResource( hotResY );
							KrBox* hotX = new KrBox( hotResX );
							KrBox* hotY = new KrBox( hotResY );

							hotX->SetPos( rect.min.x, 0 );	// relative to sprite
							hotY->SetPos( 0, rect.min.y );	// relative to sprite

							engine->Tree()->AddNode( sprite, hotX );
							engine->Tree()->AddNode( sprite, hotY );
						}
						
						imageList.PushBack( sprite );
					}
				}
			}
		}
		else if ( res->ToTileResource() )
		{
			color.Set( 100, 255, 100 );

			KrTileResource* tileRes = res->ToTileResource();
			KrTile* tile = new KrTile( tileRes );

			KrRect rect;
			tile->QueryBoundingBox( &rect, 0 );

			engine->Tree()->AddNode( engine->Tree()->Root(), tile );

			// Now add a bounding box.
			KrBoxResource* boxRes = new KrBoxResource(	"", 
														rect.Width()+2, rect.Height()+2,
														&color, 1,
														KrBoxResource::OUTLINE );
			engine->Vault()->AddResource( boxRes );
			KrBox* box = new KrBox( boxRes );
			box->SetPos( -1+rect.min.x, -1+rect.min.y );	// relative to sprite

			engine->Tree()->AddNode( tile, box );
			
			imageList.PushBack( tile );
		}
	}


	GlSListIterator< KrImage* > imageIt( imageList );
	imageIt.Begin();
	
	while( !imageIt.Done() )
	{
		if ( !imageIt.Done() )
		{
			int x = 0;
			int y = 0;
			int maxY = 0;

			// Set everything invisible. Click on visibility of what
			// is on screen.
			GlSListIterator< KrImage* > visIt( imageList );
			for( visIt.Begin(); !visIt.Done(); visIt.Next() )
				visIt.Current()->SetVisible( false );

			while ( !imageIt.Done() )
			{
				KrImage* im = imageIt.Current();
				KrRect rect;
				im->QueryBoundingBox( &rect, 0 );

				// Will we exceed the current line?
				if (    x != 0		// if at left edge...oh well.
					 && x + rect.Width() + 2 >= engine->ScreenBounds().max.x )
				{
					x = 0;
					y += maxY + 2;
					maxY = 0;
				} 

				// Will we go off the bottom?
				if (	y != 0
					 && y + rect.Height() + 2 >= engine->ScreenBounds().max.y )
				{
					y = 0;
					x = 0;
					maxY = 0;
					break;			// Don't process this image. Off to next.
				}

				im->SetPos( x-rect.min.x + 1,		// +1: leave room for bounding box.
							y-rect.min.y + 1);
				im->SetVisible( true );

				x += rect.Width() + 2;
				maxY = grinliz::Max( rect.Height(), maxY );
				imageIt.Next();
			}
		}

		engine->Draw( false, 0 );
		char buf[256];
		sprintf( buf, "%s_%02d.bmp", prefix, index );
		int result = SDL_SaveBMP( surface, buf );
		printf( "Saved BMP (result=%d) file: %s\n", result, buf );
		++index;
	}
	engine->Tree()->DeleteNode( backNode );
	delete backgroundRes;
	delete engine;
	SDL_FreeSurface( surface );
}
