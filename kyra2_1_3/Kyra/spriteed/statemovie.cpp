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

#include "states.h"
#include "SDL.h"
#include "statemachine.h"
#include "../engine/parser.h"
#include "../engine/textbox.h"
#include "../engine/box.h"

using namespace grinliz;

EdStateMovie::EdStateMovie( Editor* _machine ) : EdState( _machine )
{
	spriteResource = 0;
	movieNode = 0;
	scale = 1;
}


void EdStateMovie::StateOpening()
{
	movieNode = new KrImNode();
	shared->Engine()->Tree()->AddNode( shared->ImNode(), movieNode );

	// Create the action, put it in a sprite resource.
	KrAction* movieAction = shared->CreateAnimationAction();
	if ( movieAction )
	{
		spriteResource = new KrSpriteResource(	"AlignerSprite" );
		spriteResource->AddAction( movieAction );

		numFrames = spriteResource->GetActionByIndex( 0 )->NumFrames();
	
		KrSprite* sprite = new KrSprite( spriteResource );
		sprite->SetFrame( 0 );
		sprite->SetNodeId( 0 );

		KrRect rect = shared->Engine()->ScreenBounds();
		sprite->SetPos( rect.Width()  / 2,
						rect.Height() / 2 );
		shared->Engine()->Tree()->AddNode( movieNode, sprite );

		// Restore previous scale.
		scale = Clamp( scale, GlFixed( 1 ) / GlFixed( 4 ), GlFixed( 4 ) );
		GlFixed inverse = GlFixed( 1 ) / scale;
		sprite->SetPos( (inverse*rect.Width()).ToInt() / 2, 
						(inverse*rect.Height()).ToInt() / 2 );		
		movieNode->SetScale( scale, scale );
	}	
	shared->ConsoleNode()->SetVisible( false );	
	shared->SetInfoBox( false );
}


void EdStateMovie::StateClosing()
{
	shared->Engine()->Tree()->DeleteNode( movieNode );	
	delete spriteResource;
	spriteResource = 0;
}


void EdStateMovie::FrameTick()
{
	KrImNode* image = shared->Engine()->Tree()->FindNodeById( 0 );
	GLASSERT( image );
	KrSprite* sprite = image->ToSprite();
	GLASSERT( sprite );

	sprite->DoStep();

	int x = sprite->CompositeXForm(0).x.ToInt();
	int y = sprite->CompositeXForm(0).y.ToInt();
	KrRect screen = shared->Engine()->ScreenBounds();
	GlFixed scale = movieNode->XScale();
	GlFixed inverse = GlFixed( 1 ) / scale;

	if ( x < 0 )
		sprite->SetPos( (inverse * screen.Width()).ToInt(), sprite->Y() );
	if ( x > screen.Width() )
		sprite->SetPos( 0, sprite->Y() );
	if ( y < 0 )
		sprite->SetPos( sprite->X(), (inverse*screen.Height()).ToInt() );
	if ( y > screen.Height() )
		sprite->SetPos( sprite->X(), 0 );
}


void EdStateMovie::ZoomIn()
{
	if ( scale < 4 )
	{
		scale *= 2;
		movieNode->SetScale( scale, scale );


		KrImNode* image = shared->Engine()->Tree()->FindNodeById( 0 );
		GLASSERT( image );
		KrSprite* sprite = image->ToSprite();
		GLASSERT( sprite );

		shared->Engine()->Tree()->Walk();

		//int x = sprite->CompositeXForm(0).x.ToInt();
		//int y = sprite->CompositeXForm(0).y.ToInt();
		KrRect screen = shared->Engine()->ScreenBounds();
		GlFixed scale = movieNode->XScale();
		GlFixed inverse = GlFixed( 1 ) / scale;

		sprite->SetPos( (inverse * screen.Width()).ToInt() / 2, (inverse*screen.Height()).ToInt()/2 );
	}
}


void EdStateMovie::ZoomOut()
{
	if ( scale.v > GlFixed_1 / 4 )
	{
		scale /= 2;
		movieNode->SetScale( scale, scale );


		KrImNode* image = shared->Engine()->Tree()->FindNodeById( 0 );
		GLASSERT( image );
		KrSprite* sprite = image->ToSprite();
		GLASSERT( sprite );

		shared->Engine()->Tree()->Walk();

		//int x = sprite->CompositeXForm(0).x.ToInt();
		//int y = sprite->CompositeXForm(0).y.ToInt();
		KrRect screen = shared->Engine()->ScreenBounds();
		GlFixed scale = movieNode->XScale();
		GlFixed inverse = GlFixed( 1 ) / scale;

		sprite->SetPos( (inverse * screen.Width()).ToInt() / 2, (inverse*screen.Height()).ToInt()/2 );
	}
}

