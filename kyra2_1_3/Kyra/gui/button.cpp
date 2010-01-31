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
#include "button.h"
#include "eventmanager.h"
#include "../engine/boxresource.h"
#include "../engine/box.h"
#include "../engine/textbox.h"

using namespace grinliz;

KrButton::KrButton( int _width, int _height, const KrScheme& s )
	: KrWidget( s ),
	  bevel( _width, _height, s )
{
	// Like all widgets, this is essentially a KrImNode that creates children
	// in its "addedToTree" method. Little to do here.
	width = _width;
	height = _height;
	mode = 0;

	KrRGBA grey;
	//KrColorTransform xform;

	grey		= scheme.primary;
	plateRes	= new KrBoxResource( "KrButton plate", width, height,	&grey,		1, KrBoxResource::FILL );
	textBox = 0;
	icon = 0;
	userDrawn = false;
}


KrButton::KrButton(	KrSprite* graphic,
					const KrScheme& scheme ) : KrWidget( scheme ), bevel( 0, 0, scheme )
{
	Rectangle2I bounds;
	graphic->QueryBoundingBox( &bounds );

	width = bounds.Width();
	height = bounds.Height();

	mode = 0;
	textBox = 0;
	plateRes = 0;
	icon = graphic;
	userDrawn = true;
}


KrButton::~KrButton()
{
	//if we go out of scope, remove our listener
	KrEventManager::Instance()->RemoveListener( this );

	// Our children are deleted first, so this is safe.
	delete plateRes;
//	delete topLineRes;
//	delete bottomLineRes;
//	delete leftLineRes;
//	delete rightLineRes;
}


void KrButton::SetTextChar( const std::string& _text )
{
	if ( Engine() && scheme.font )
	{
		if ( textBox )
			Engine()->Tree()->DeleteNode( textBox );

		textBox = 0;
		text = _text;
		PlaceText();
	}
	else
	{
		text = _text;
	}
}


void KrButton::SetIcon( KrSprite* giveSprite )
{
	// No icon is set for a user draw button! The graphic
	// provided can't be changed.
	if ( userDrawn == true )
		return;

	if ( Engine() )
	{
		if ( icon )
			Engine()->Tree()->DeleteNode( icon );

		icon = giveSprite;
		PlaceIcon();
	}
	else
	{
		icon = giveSprite;
	}
}


void KrButton::PlaceText()
{
	GLASSERT( Engine() );
	if ( scheme.font )
	{
		textBox = new KrTextBox( scheme.font, width, scheme.font->FontHeight(), 0, KrTextBox::CENTER );
		textBox->SetPos( 0, ( height - scheme.font->FontHeight() ) / 2 );
		textBox->SetTextChar( text, 0 );
		Engine()->Tree()->AddNode( holder, textBox );
		textBox->SetZDepth( TEXT_DEPTH );
	}
}


void KrButton::PlaceIcon()
{
	GLASSERT( Engine() );

	if ( icon )
	{
		int iconWidth = width;
		int iconHeight = height;
			
		if ( !userDrawn )
		{
			iconWidth  = bevel.width - 4;	// Give some border space.
			iconHeight = bevel.height - 4;
		}

		if ( width > 0 && height > 0 )
		{
			Rectangle2I bounds;
			icon->QueryBoundingBox( &bounds );

			GlFixed scale = 1;

			if ( !userDrawn )
			{
				GlFixed wScale = GlFixed( iconWidth ) / GlFixed( bounds.Width() );
				GlFixed hScale = GlFixed( iconHeight ) / GlFixed( bounds.Height() );
				scale = Min( wScale, hScale );
				GLASSERT( scale > 0 );
				icon->SetScale( scale, scale );
			}

			iconX = ( width - ( scale * bounds.Width() ).ToInt() ) / 2;
//			iconX = iconX / 2 - ( scale * bounds.xmin ).ToInt() + 1;

			iconY = ( height - ( scale * bounds.Height() ).ToInt() ) / 2;
//			iconY = iconY / 2 - ( scale * bounds.ymin ).ToInt() + 1;

			icon->SetPos( iconX, iconY );
			Engine()->Tree()->AddNode( holder, icon );
			icon->SetZDepth( ICON_DEPTH );
		}
	}
}


void KrButton::AddedtoTree()
{
	KrWidget::AddedtoTree();

	holder = new KrImNode();
	Engine()->Tree()->AddNode( this, holder );

	if ( !userDrawn )
	{
		plate = new KrBox( plateRes );
		Engine()->Tree()->AddNode( holder, plate );

		bevel.AddToTree( Engine(), holder );
		bevel.DrawOut();
	}
	KrEventManager::Instance()->AddListener( this );

	if ( scheme.font )
		PlaceText();
	if ( icon )
		PlaceIcon();
}


void KrButton::SetMode( int m )
{
	if (    Engine() && m != mode )
	{
		if ( ( m & DOWN ) && !( mode & DOWN ) )			// down transition
		{
			KrWidgetEvent activated;
			activated.type = KrWidgetEvent::ACTIVATED;

			for(	Publisher< IKrWidgetListener >::const_iterator it = widgetPublish.begin();
					it != widgetPublish.end();
					++it )
			{
				IKrWidgetListener* listener = *it;
				listener->HandleWidgetEvent( this, activated );
			}
			//PublishEvent( ACTIVATED, 0, 0, 0, 0 );		
			//ButtonOnSignal.emit();

			if ( IsSelectable() )
			{
				mode = m;		// Prevent from recursion.
				KrEventManager::Instance()->SelectMe( this );
			}
		}
		else if ( !( m & DOWN ) && ( mode & DOWN ) )	// up transition
		{
			KrWidgetEvent released;
			released.type = KrWidgetEvent::DEACTIVATED;

			for(	Publisher< IKrWidgetListener >::const_iterator it = widgetPublish.begin();
					it != widgetPublish.end();
					++it )
			{
				(*it)->HandleWidgetEvent( this, released );
			}
			//PublishEvent( DEACTIVATED, 0, 0, 0, 0 );
			//ButtonOffSignal.emit();
		}
		mode = m;

		KrColorTransform color;
		ModeChange(color);

		if ( !userDrawn )
		{
			plate->SetColor( color );
		}

	}
}

void KrButton::ModeChange(KrColorTransform& color)
{

		if ( mode == ( OVER ) )
		{
			color = scheme.CalcHiSec();

			if ( userDrawn )
			{
				icon->SetFrame( 1 );
			}
			else
			{
				bevel.DrawOut();
				if ( icon )
					icon->SetPos( iconX, iconY );
			}
		}
		else if ( mode & DOWN )
		{		
			if ( mode & OVER )
				color = scheme.CalcDarkSec();
			else
				color = scheme.CalcDark();

			if ( userDrawn )
			{
				icon->SetFrame( 2 );
			}
			else
			{
				bevel.DrawIn();
				if ( icon )
					icon->SetPos( iconX+1, iconY+1 );
			}
		}
		else
		{
			if ( userDrawn )
			{
				icon->SetFrame( 0 );
			}
			else
			{
				bevel.DrawOut();
				if ( icon )
					icon->SetPos( iconX, iconY );
			}
		}

}


///////////////////////////////////////////////////////
// PUSH


void KrPushButton::MouseIn( bool down, bool in )
{
	if ( in )
		SetMode(   ( down ? DOWN : 0 )
				 | ( in   ? OVER : 0 ) );
	else	
		SetMode( 0 );
}


void KrPushButton::MouseMove( bool down, int x, int y )
{}


bool KrPushButton::MouseClick( int down, int x, int y )
{
	SetMode( down ? DOWN | OVER : OVER );
	return true;
}


void KrPushButton::Accelerate( bool down )
{
	int m = mode & OVER;
	if ( down )
		m |= DOWN;
	SetMode( m );
}


///////////////////////////////////////////////////////
// TOGGLE


void KrToggleButton::MouseIn( bool down, bool in )
{
	int m = mode & DOWN;

	if ( in )
		m |= OVER;

	SetMode( m );
}


void KrToggleButton::MouseMove( bool down, int x, int y )
{}


bool KrToggleButton::MouseClick( int down, int x, int y )
{
	if ( down )
	{
		int m = ( mode & DOWN ) ? 0 : DOWN;
		m |= OVER;

		SetMode( m );
		return true;
	}

	return false;
}


void KrToggleButton::Accelerate( bool down )
{
	if ( down )
	{
		int m = mode & OVER;
		int d = ( mode & DOWN );

		if ( !d )
			m |= DOWN;

		SetMode( m );
	}
}


void KrToggleButton::Selected( bool selected )
{
	int m = mode & OVER;
	m |= selected ? DOWN : 0;
	SetMode( m );
}
