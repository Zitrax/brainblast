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

#include "eventmanager.h"
#include "widget.h"
#include "../engine/engine.h"
#include "../engine/boxresource.h"
#include "../engine/box.h"


KrScheme::KrScheme( KrFontResource* _font )
{
	primary.Set( 170, 170, 170 );
	cursor.Set( 255, 255, 255 );
	secondary.Set(	200, 0, 
					255, 0, 
					200, 0, 
					255 );
	font = _font;
}


KrRGBA KrScheme::CalcBrightLine() const
{
	KrRGBA lightGrey = primary;

	KrColorTransform xform;
	xform.Brighten( BRIGHT );
	xform.ApplyTransform( &lightGrey );

	return lightGrey;		
}


KrRGBA KrScheme::CalcShadowLine() const
{
	KrRGBA darkGrey = primary;

	KrColorTransform xform;
	xform.Darken( DARK );
	xform.ApplyTransform( &darkGrey );

	return darkGrey;		
}


KrColorTransform KrScheme::CalcHiPrimary() const
{
	KrColorTransform light;
	light.Brighten( BRIGHT );
	return light;
}


KrColorTransform KrScheme::CalcHiSec() const
{
	return secondary;
}


KrColorTransform KrScheme::CalcDark() const
{
	KrColorTransform dark;
	dark.Darken( DARK );
	return dark;	
}


KrColorTransform KrScheme::CalcDarkSec() const
{
	KrColorTransform xform = secondary;
	KrColorTransform dark;
	dark.Darken( DARK );

	xform.Composite( dark );
	return xform;	
}


KrBevelElement::KrBevelElement( int w, int h, const KrScheme& scheme )
{
	width = w;
	height = h;
	
	horD = horL = vertD = vertL = 0;

	KrRGBA light = scheme.CalcBrightLine();
	KrRGBA dark  = scheme.CalcShadowLine();

	horDR	= new KrBoxResource( "KrBevelElement",	width, 1,	&dark, 1,  KrBoxResource::FILL );
	horLR	= new KrBoxResource( "KrBevelElement",  width, 1,	&light,	1, KrBoxResource::FILL );
	vertDR	= new KrBoxResource( "KrBevelElement",	1, height,	&dark, 1,  KrBoxResource::FILL );
	vertLR	= new KrBoxResource( "KrBevelElement",	1, height,	&light,	1, KrBoxResource::FILL );
};


KrBevelElement::~KrBevelElement()
{
	// Children should be deleted from the tree.
	delete horDR;
	delete horLR;
	delete vertDR;
	delete vertLR;
}


void KrBevelElement::AddToTree( KrEngine* e, KrImNode* p )
{
	horD = new KrBox( horDR );
	e->Tree()->AddNode( p, horD );
	horL = new KrBox( horLR );
	e->Tree()->AddNode( p, horL );
	vertD = new KrBox( vertDR );
	e->Tree()->AddNode( p, vertD );
	vertL = new KrBox( vertLR );
	e->Tree()->AddNode( p, vertL );
}


void KrBevelElement::DrawIn()
{
	horD->SetPos( 0, 0 );
	horL->SetPos( 0, height-1 );
	vertD->SetPos( 0, 0 );
	vertL->SetPos( width-1, 0 );
}

void KrBevelElement::DrawOut()
{
	horD->SetPos( 0, height-1 );
	horL->SetPos( 0, 0 );
	vertD->SetPos( width-1, 0 );
	vertL->SetPos( 0, 0 );
}


KrWidget::KrWidget( const KrScheme& _scheme ) : scheme( _scheme )
{
	groupId = 0;
}


KrWidget::~KrWidget()
{
	KrEventManager::Instance()->RemoveListener( this );
}


//void KrWidget::AddListener( IKrWidgetListener* listener )
//{
//	if ( eventList.Find( listener ) == eventList.NotFound() )
//		eventList.PushBack( listener );
//}
//
//
//void KrWidget::RemoveListener( IKrWidgetListener* listener )
//{
//	// Don't actually change the list, in case this is recursively
//	// called! The Publish can clean the list of extras later.
//
//	unsigned i = eventList.Find( listener );
//	if ( i != eventList.NotFound() )
//		eventList[ i ] = 0;
//}


//void KrWidget::PublishEvent( U32 event, U32 data, const SDL_Event* sdlEvent, const char* command, const char* arg )
//{
//	bool foundNull = false;
//
//	for( unsigned currentNode = 0; currentNode < eventList.Count(); ++currentNode )
//	{
//		if ( eventList[ currentNode ] )
//		{
//			// Note: this may decrement currentNode if we self-delete
//			eventList[currentNode]->HandleWidgetEvent( this, event, data, sdlEvent, command, arg );
//		}
//		else
//		{
//			foundNull = true;
//		}
//	}
//
//	if ( foundNull )
//	{
//		unsigned i=0;
//		while ( i < eventList.Count() )
//		{
//			if ( eventList[i] )
//				++i;
//			else
//				eventList.Remove( i );
//		}
//	}
//}


//void KrWidget::PublishTaggedEvent( U32 event, const SDL_Event* sdlEvent, const char* command, const char* arg, IKrWidgetListener* special )
//{
//
//	for( unsigned currentNode = 0; currentNode < eventList.Count(); ++currentNode )
//	{
//		if ( eventList[ currentNode ] )
//		{
//			// Note: this may decrement currentNode if we self-delete
//			eventList[currentNode]->HandleWidgetEvent(	this,
//														event, 
//														special == eventList[currentNode] ? 1 : 0,
//														sdlEvent, 
//														command, arg );
//		}
//	}
//}
//

void KrWidget::SetAccelerator( int keymod, int keysym )
{
	KrEventManager::Instance()->SetAccelerator( keymod, keysym, this );
}


KrWidget* KrWidget::ParentWidget()
{
	KrImNode* node;
	for( node = this->Parent(); node; node=node->Parent() )
	{
		if ( node->ToWidget() )
			return node->ToWidget();
	}
	return 0;
}

