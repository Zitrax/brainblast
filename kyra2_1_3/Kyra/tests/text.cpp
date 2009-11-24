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


#include <stdlib.h>
#include <stdio.h>

#include "SDL.h"
#include "demos.h"

using namespace grinliz;

/*	Test of a bunch of text falling down the screen.
	The foreground is text, and towards the background are some
	spaceships. The background surface is black.
*/

TextTest::TextTest( SDL_Surface* _screen )
{
	int i, j;
	random.SetSeed( 2 );

	/*	This demo uses 2 .dat files, and therefore 2 vaults. No
		problem -- just make sure the engine is deleted BEFORE the
		vault it uses.
	*/
	spaceVault = new KrResourceVault();
	dataVault  = new KrResourceVault();
	fontVault  = new KrResourceVault();
	engine = new KrEngine( _screen );

	frame = 0;
	showGlyphs = 0;

	spaceVault->LoadDatFile( "space.dat" );
	dataVault->LoadDatFile( "title.dat" );

	fontVault = new KrResourceVault();
	if ( !fontVault->LoadDatFile( "font.dat" ) )
	{
		GLOUTPUT(( "Error loading 'font.dat'\n" ));
		exit(100);
	}

	// Pull out the resources for the spaceships and 2 fonts.
	KrSpriteResource* spaceShipRes = spaceVault->GetSpriteResource( "MED" );
	GLASSERT( spaceShipRes );

	KrFontResource*	fontConsole = fontVault->GetFontResource( "CONSOLE" );
	GLASSERT( fontConsole );
	KrFontResource* fontArial   = fontVault->GetFontResource( "ARIAL" );
	GLASSERT( fontArial );

	// Create some sprites for the spaceships:
	for( i = 0; i < NUM_SHIPS; i++ )
	{
		ship[i] = new KrSprite( spaceShipRes );
		ship[i]->SetPos(  random.Rand( _screen->w ), i * _screen->h / NUM_SHIPS + ( _screen->h / NUM_SHIPS ) / 2 );
		ship[i]->SetAction( "BODY" );
		engine->Tree()->AddNode( 0, ship[i] );
	}

	// Create some text fields. The last textBox is handled as
	// a special case, since it is readable.

	// Test out boxes, as well.
	//const int NUM_BOX = 3;
	const int WIDTH = 150;
	const int HEIGHT = 300;
	KrRGBA gray, grayAlpha;
	gray.Set( 128, 128, 128 );
	grayAlpha.Set( 128, 128, 128, 128 );

	KrBoxResource* boxRes, *boxResSolid;
	boxRes		= new KrBoxResource( "", WIDTH, HEIGHT, &gray,		1, 0 );
	boxResSolid = new KrBoxResource( "", WIDTH, HEIGHT, &grayAlpha, 1, 1 );

	// Add it to the vault so it gets deleted with the other resources:
	engine->Vault()->AddResource( boxRes );
	engine->Vault()->AddResource( boxResSolid );

	// Get the text:
	std::vector< std::string > poemNoTitle;
	std::string title;

	KrTextDataResource* textData = dataVault->GetTextDataResource( "POEM" );
	GLASSERT( textData );
	textData->Text( &poemNoTitle );

	// A completely sythetic way to get text. Just done to test:
	KrBinaryDataResource* binData = dataVault->GetBinaryDataResource( "TITLE" );
	GLASSERT( binData );
	// The length is the first byte, followed by text.
	GLASSERT( *( binData->Data() ) + 1 == binData->Length() );
	for ( i=0; i<*( binData->Data() ); ++i )
	{
		title += (char) *( binData->Data() + i + 1 );
	}

	// Now construct the poem from the title and poem piece.
	poem.push_back( title );
	for( i=0; i<(int)poemNoTitle.size(); ++i )
		poem.push_back( poemNoTitle[i] );

	// And now the other text boxes.
	for( i = 0; i < NUM_TEXT_BOX-1; i++ )
	{
		int x =  i * ( _screen->w ) / (NUM_TEXT_BOX-1); //random.Rand( _screen->w );
		int y =  random.Rand( _screen->h ); //i * _screen->h / NUM_TEXT_BOX;

		if ( i != 1 )
		{
			box[i] = new KrBox( boxRes );
		}
		else
		{
			box[i] = new KrBox( boxResSolid );
		}

		KrTextBox::Alignment align = KrTextBox::LEFT;
		if      ( i == 1 )	align = KrTextBox::CENTER;
		else if ( i == 2 )	align = KrTextBox::RIGHT;

		textBox[i] = new KrTextBox( fontConsole, WIDTH, HEIGHT, i, align );
//		textBox[i]->SetColor( temp );
		#ifdef DEBUG
			if ( i == 0 )
				textBox[i]->SetNodeName( "debug" );
		#endif

		textBox[i]->SetPos( x, y );

		for( j=0; j<(int)poem.size(); j++ )
		{
			textBox[i]->SetTextChar( poem[j].c_str(), j );
		}
		engine->Tree()->AddNode( 0, textBox[i] );

		if ( i != 2 )
		{
			engine->Tree()->AddNode( textBox[i], box[ i ] );
		}
		else
		{
			delete box[i];
			box[i] = 0;
		}
	}
	textBox[NUM_TEXT_BOX-1] = new KrTextBox( fontArial, 1024, 1024, 0 );
	textBox[NUM_TEXT_BOX-1]->SetPos( _screen->w / 16, _screen->h / 16 );

	engine->Tree()->AddNode( 0, textBox[NUM_TEXT_BOX-1] );
}


TextTest::~TextTest()
{
	delete engine;
	delete spaceVault;		//note, again, vault must be deleted after engine
	delete dataVault;
	delete fontVault;

	#ifdef KYRA_SUPPORT_OPENGL
		if ( KrTextureManager::TextureIndex() != 0 )
			KrTextureManager* debug = KrTextureManager::Instance();
    	GLASSERT( KrTextureManager::TextureIndex() == 0 );
 	#endif
}


void TextTest::DrawFrame()
{
	int i, j, count;

	// Move the ships.
	for( i=0; i<NUM_SHIPS; i++ )
	{
		ship[i]->SetPos( ship[i]->X() - 1, ship[i]->Y() );
		if ( ship[i]->X() < 0 )
		{
			ship[i]->SetPos( engine->ScreenBounds().max.x, ship[i]->Y() );
		}
	}

	// Draw the "typewriter text" for the main text object.
	i = 0;
	j = 0;
	count = 0;

	while ( count < showGlyphs && j < (int)poem.size() )
	{
		if ( ( count + (int)poem[j].length() ) < showGlyphs )
		{
			count += poem[j].length();
			++j;
			// Out of lines!
			if ( j == (int)poem.size() )
			{
				for ( i=0; i<(int)poem.size(); i++ )
					textBox[NUM_TEXT_BOX-1]->SetTextChar( 0, i );
				showGlyphs = 0;
			}
		}
		else
		{
			char buf[256];
			strcpy( buf, poem[j].c_str() );
			GLASSERT( showGlyphs - count >= 0 );
			GLASSERT( showGlyphs - count <= (int)poem[j].length() );
			buf[ showGlyphs - count ] = 0;
			textBox[NUM_TEXT_BOX-1]->SetTextChar( buf, j );
			break;
		}
		if ( j - 4 >= 0 )
		{
			textBox[NUM_TEXT_BOX-1]->SetText16( 0, j-4 );
		}
	}
	
	// Animate the background text objects.
	for ( i=0; i<NUM_TEXT_BOX-1; i++ )
	{
		textBox[i]->SetPos( textBox[i]->X(), 
							textBox[i]->Y() + i/3 + 1 );
		if ( textBox[i]->Y() > engine->ScreenBounds().max.y )
		{
			textBox[i]->SetPos( textBox[i]->X(), -textBox[i]->Height() );
		}

		KrColorTransform color;
		color = textBox[i]->CTransform();

		int r = ( frame + i*30 ) % 100 + 155;
		int g = ( frame + i*10 ) % 100 + 155;
		int b = ( frame + i*50 ) % 100 + 155;

		color.SetRed(   Clamp( r, 150, 255 ), 0 );
		color.SetGreen( Clamp( g, 150, 255 ), 0 );
		color.SetBlue(  Clamp( b, 150, 255 ), 0 );

		textBox[i]->SetColor( color );
	}

	engine->Draw();
	++frame;
	++showGlyphs;
}
