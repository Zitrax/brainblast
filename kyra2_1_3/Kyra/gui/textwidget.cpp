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
#include "textwidget.h"
#include "../engine/boxresource.h"
#include "../engine/box.h"
#include "../engine/textbox.h"
#include "../engine/fontresource.h"

using namespace grinliz;

KrTextWidget::KrTextWidget(	int _capacity, int _width, int _height, 
							bool _drawBorder,
							bool _drawBackground,
							bool _editable,
							const KrScheme& _scheme ) : KrWidget( _scheme )
{
	width = _width;
	height = _height;
	drawBorder = _drawBorder;
	drawBackground = _drawBackground;
	editable = _editable;
	//KrScheme scheme = _scheme;

	plateRes = 0;
	plate = 0;
	textBox = 0;
	cursorPos = 0;
	maxSpaces = _capacity;
	cursor = 0;
	bevel = 0;
}

KrTextWidget::KrTextWidget(	int _width, int _height, 
							bool _drawBorder,
							bool _drawBackground,
							bool _editable,
							const KrScheme& _scheme ) : KrWidget( _scheme )
{
	width = _width;
	height = _height;
	drawBorder = _drawBorder;
	drawBackground = _drawBackground;
	editable = _editable;
	//KrScheme scheme = _scheme;

	plateRes = 0;
	plate = 0;
	textBox = 0;
	cursorPos = 0;
	maxSpaces = 1000;
	cursor = 0;
	bevel = 0;
}

KrTextWidget::~KrTextWidget()
{
	// Children are deleted first, else this explodes.
	KrEventManager::Instance()->RemoveListener(this);

	delete plateRes;
	delete cursorRes;
	delete bevel;
}


void KrTextWidget::AddedtoTree()
{
	KrWidget::AddedtoTree();
	
	holder = new KrImNode();
	Engine()->Tree()->AddNode( this, holder );

	// Add a background plate
	if ( drawBackground )
	{
		plateRes = new KrBoxResource( "TextWidget Plate",
									  width, height,
									  &scheme.primary, 1,
									  KrBoxResource::FILL );

		plate = new KrBox( plateRes );
		Engine()->Tree()->AddNode( holder, plate );
	}

	// Add a text box
	textBox = new KrTextBox( scheme.font, 
							 ( drawBorder ) ? width -2 : width,
							 scheme.font->FontHeight(), 
							 0 );

	textBox->SetPos(	( drawBorder ) ? 1 : 0,
						( height - scheme.font->FontHeight() ) / 2  );
	
	Engine()->Tree()->AddNode( holder, textBox );

	// The cursor
	cursorRes = new KrBoxResource(	"KrTextWidget cursor",
									CURSOR_WIDTH, scheme.font->FontHeight(),
									&scheme.cursor, 1,
									KrBoxResource::FILL );

	cursor = new KrBox( cursorRes );
	cursor->SetVisible( false );
	Engine()->Tree()->AddNode( textBox, cursor );

	// Borders
	if ( drawBorder )
	{
		bevel = new KrBevelElement( width, height, scheme );
		bevel->AddToTree( Engine(), holder );
		bevel->DrawIn();
	}
	KrEventManager::Instance()->AddListener( this );
}


bool KrTextWidget::KeyEvent( const SDL_Event &event )
{
	if (	Engine() 
		 && event.type == SDL_KEYDOWN )
	{
		int length = textBox->GetLineLength( 0 );
		std::vector<U16> text = textBox->GetText16Array( 0 );

		// Make sure the cursor position is in range.
		int cursorPosTemp = Clamp( cursorPos, 0, length );
		GLASSERT( cursorPosTemp == cursorPos );
		cursorPos = cursorPosTemp;

		// Line processing keys:
		if ( event.key.keysym.sym == SDLK_BACKSPACE )
		{
			if ( cursorPos > 0 )
			{
				text.erase( text.begin() + (cursorPos-1) );
				textBox->SetText16( &text[0], 0 );
				--cursorPos;
				--length;
			}
		} 
		else if ( event.key.keysym.sym == SDLK_DELETE )
		{
			if ( cursorPos < length )
			{
				text.erase( text.begin() + cursorPos );
				textBox->SetText16( &text[0], 0 );
				--length;
			}
		} 
		else if ( event.key.keysym.sym == SDLK_LEFT )
		{
			if ( cursorPos > 0 )
				--cursorPos;
		}
		else if ( event.key.keysym.sym == SDLK_RIGHT )
		{
			if ( cursorPos <= textBox->GetLineLength( 0 ) )
				++cursorPos;
		}
		else if ( event.key.keysym.sym == SDLK_HOME )
		{
			cursorPos = 0;
		}
		else if ( event.key.keysym.sym == SDLK_END )
		{
			cursorPos = length;
		} 
		else if ( event.key.keysym.sym == SDLK_RETURN )
		{
			//PublishEvent( ACTIVATED, 0, &event, 0, 0 );
			//EnterSignal.emit();
			KrWidgetEvent event;
			event.type = KrWidgetEvent::ACTIVATED;

			for(	Publisher< IKrWidgetListener >::const_iterator it = widgetPublish.begin();
					it != widgetPublish.end();
					++it )
			{
				(*it)->HandleWidgetEvent( this, event );
			}
		}
		else if ( event.key.keysym.unicode >= ' ' )
		{
			if(length < maxSpaces)
			{
				text.insert( text.begin()+cursorPos, event.key.keysym.unicode );
				textBox->SetText16( &text[0], 0 );
				++cursorPos;
				++length;
			}
		}
		else
		{
//			PublishEvent( KEY, 0, &event, 0, 0 );
			return false;	// we didn't handle.
		}

		PositionCursor();
		return true;			// Almost all keys are handled.
	}
	return false;
}


void KrTextWidget::PositionCursor()
{
	if ( Engine() )
	{
		// Get the position of the bottom line, and 
		// put the cursor there.
		int length = textBox->GetLineLength( 0 );
		int y      = textBox->GetLineY( 0 );

		// Make sure the cursor position is in range.
		cursorPos = Clamp( cursorPos, 
							 0, length );

		int x = textBox->FontResource()->FontWidthN( textBox->GetText16( 0 ), cursorPos );

		//GLOUTPUT( "Cursor (%d,%d)\n", x, y );
		cursor->SetPos( x, y );
	}
}


void KrTextWidget::KeyFocus( bool receivingFocus )
{
	cursor->SetVisible( receivingFocus );
}


void KrTextWidget::SetTextChar( const std::string& t )
{
	if ( textBox )
	{
		textBox->SetTextChar( t, 0 );
		cursorPos = textBox->GetLineLength( 0 );
		PositionCursor();
	}
}


void KrTextWidget::GetTextChar( std::string* buffer )
{
	if ( textBox )
		textBox->GetTextChar( buffer, 0 );
}


void KrTextWidget::MouseIn( bool down, bool in )
{
	if ( in )
	{
		holder->SetColor( scheme.CalcHiSec() );
	}
	else
	{
		KrColorTransform none;
		holder->SetColor( none );
	}
}


bool KrTextWidget::MouseClick( int down, int x, int y )
{
	bool handledEvent = false;
	bool selectionSent = false;
	if ( down && editable )
	{
		cursorPos = textBox->GetLineLength();
		for( int i=0; i<textBox->GetLineLength(); ++i )
		{
			int lineX = textBox->FontResource()->FontWidthN( textBox->GetText16( 0 ), i );
			if ( lineX >= x )
			{
				cursorPos = i;
				break;		
			}
		}
		PositionCursor();

		KrEventManager::Instance()->GrabKeyFocus( this );
		selectionSent = true;
		handledEvent = true;
	}

	if (    down
		 && !selectionSent )		// don't double send selection if we sent one for grabbing focus
	{
		//PublishEvent( SELECTION, 0, 0, 0, 0 );
		//SelectedSignal0.emit();
		//SelectedSignal1.emit( this );
		KrWidgetEvent event;
		event.type = KrWidgetEvent::SELECTION;
		event.selection.index = 0;

		std::string buffer;
		textBox->GetTextChar( &buffer, 0 );
		event.selection.text = buffer.c_str();

		for(	Publisher< IKrWidgetListener >::const_iterator it = widgetPublish.begin();
				it != widgetPublish.end();
				++it )
		{
			(*it)->HandleWidgetEvent( this, event );
		}
		handledEvent = true;
	}

	return handledEvent;
}


