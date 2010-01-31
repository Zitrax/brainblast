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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "console.h"
#include "textwidget.h"
#include "../engine/engine.h"
#include "../engine/textbox.h"
#include "../engine/canvas.h"
#include "../engine/imagetree.h"
#include "../engine/boxresource.h"
#include "../engine/box.h"
#include "../../grinliz/glutil.h"

using namespace grinliz;

KrConsole::KrConsole(	int _width, int _height,
						int _lineSpacing,
						const KrScheme& scheme ) : KrWidget( scheme )
{
	commandBufSize = 0;
	commandBufNode = 0;
	background = 0;
	backgroundRes = 0;
	font = scheme.font;
	GLASSERT( font );
	width = _width;
	height = _height;
	lineSpacing = _lineSpacing;
	commandLine = 0;

}


KrConsole::~KrConsole()
{
	// Our children are deleted first, so this is safe.
	delete backgroundRes;
	backgroundRes = 0;
}


void KrConsole::AddedtoTree()
{
	KrWidget::AddedtoTree();

	// Create the textbox
	textBox = new KrTextBox(	font, 
								width, 
								height - lineSpacing - font->FontHeight(),	// leave space for command line
								lineSpacing );
	GLASSERT( textBox );
	if ( !textBox )
		return;

	textBox->SetZDepth( DEPTH_TEXT );
	Engine()->Tree()->AddNode( this, textBox );

	commandLine = new KrTextWidget( width, 
									font->FontHeight(),
									false,
									false,
									true,
									scheme );

	commandLine->SetPos( 0, height + lineSpacing - font->FontHeight() );
	commandLine->SetZDepth( DEPTH_TEXT );

	Engine()->Tree()->AddNode( this, commandLine );

	commandLine->widgetPublish.AddListener( this );
	//commandLine->AddListener( this );
	//commandLine->EnterSignal.connect( this, &KrConsole::ProcessEnterKey );
}


void KrConsole::SetBackgroundColor( const KrRGBA& color )
{
	if ( Engine() )
	{
		if ( background )
		{
			Engine()->Tree()->DeleteNode( background );
			background = 0;
			GLASSERT( backgroundRes );
		
			delete backgroundRes;
			backgroundRes = 0;
		}

		backgroundRes = new KrBoxResource(	"background",
											textBox->Width(), textBox->Height() + commandLine->Height(),
											&color,
											1,
											KrBoxResource::FILL );
		GLASSERT( backgroundRes );
		background = new KrBox( backgroundRes );
		background->SetZDepth( DEPTH_BACKGROUND );
		Engine()->Tree()->AddNode( this, background );
	}
}


bool KrConsole::HandleWidgetEvent( KrWidget* source, const KrWidgetEvent& event )
{
	GLASSERT( source == commandLine );

	if ( event.type == KrWidgetEvent::ACTIVATED )
	{
		ProcessEnterKey();
	}
	return true;			// A console handles all keys.
}


bool KrConsole::KeyEvent( const SDL_Event& key )
{
	if ( key.key.keysym.sym == SDLK_UP )
	{
		if ( !commandBufNode  )
		{
			commandBufNode = commandBuf.FrontNode();
		}
		else
		{
			commandBufNode = commandBufNode->next;
		}
		commandLine->SetTextChar( commandBufNode->data.c_str() );
	}
	else if ( key.key.keysym.sym == SDLK_DOWN )
	{
		if ( !commandBufNode  )
		{
			commandBufNode = commandBuf.BackNode();
		}
		else
		{
			commandBufNode = commandBufNode->prev;
		}
		commandLine->SetTextChar( commandBufNode->data.c_str() );
	}
	else if ( key.key.keysym.sym == SDLK_ESCAPE )
	{
		commandBufNode = 0;
		commandLine->SetTextChar( "" );
	}
	else if ( key.key.keysym.sym == SDLK_TAB )
	{
		TabCompletion();
	}
	return true;
}


void KrConsole::ProcessEnterKey()
{
	std::string buf;
	commandLine->GetTextChar( &buf );

	// Push the command to the command buffer. Don't add duplicates.
	if ( buf == commandBuf.Front() )
	{
		if ( commandBufSize >= COMMAND_BUF_SIZE )
		{
			commandBuf.PopBack();
		}
		else
		{
			++commandBufSize;
		}
		commandBuf.PushFront( buf );
	}

	// Scroll up!
	//textBox->SetText16( 0, textBox->NumLines() - 1 );
	PushText( buf.c_str() );
	commandBufNode = 0;
	commandLine->SetTextChar( "" );

	// Scan the command list, if we have a hit, set the 'recognized' flag.
	GlSListNode<std::string>* node = 0;

	std::string compareBuf = buf;
	std::string arg;

	int spaceAt = compareBuf.find( ' ' );
	if ( spaceAt > 0 )
	{
		arg = buf.substr( spaceAt + 1, buf.length() );
		compareBuf.resize( spaceAt );
	}

	KrWidgetEvent event;
	event.type = KrWidgetEvent::COMMAND;
	event.command.command = compareBuf.c_str();
	event.command.arg = arg.c_str();
	event.command.recognized = false;

	for( node = commandList.FrontNode(); node; node = node->next )
	{
		if ( node->data == compareBuf )
		{
			event.command.recognized = true;
			break;
		}	
	}

	//PublishTaggedEvent( ACTIVATED, 0, compareBuf.c_str(), arg.c_str(), node->data.handler );
	//CommandSignal.emit( compareBuf.c_str(), arg.c_str() );
	for(	Publisher< IKrWidgetListener >::const_iterator it = widgetPublish.begin();
			it != widgetPublish.end();
			++it )
	{
		(*it)->HandleWidgetEvent( this, event );
	}
}


//void KrConsole::AddCommandHandler( const char* command, IKrCommand* handler )
//{
//	Command c;
//	c.command = command;
//	c.handler = handler;
//
//	commandList.PushFront( c );
//}


void KrConsole::PushText( const char* text )
{
	int i;
	const U16* p;

	for( i=1; i<textBox->NumLines(); i++ )
	{
		p = textBox->GetText16( i );
		textBox->SetText16( p, i-1 );
	}
 	textBox->SetTextChar( text, textBox->NumLines()-1 );
}


void KrConsole::TabCompletion()
{
	// Go through the command list and figure out all the commands
	// this could complete to. If only one, make the completion,
	// else push the options to the console.
	std::string buf;
	GlSListNode<std::string>* node;
	GlSList<std::string> matches;

	commandLine->GetTextChar( &buf );

	for ( node = commandList.FrontNode(); node; node = node->next )
	{
		if ( buf.compare( node->data.substr( 0, buf.length() ) ) == 0 )
		{
			matches.PushFront( node->data );
		}
	}

	if ( matches.Size() > 1 )
	{
		GlSListNode<std::string>* mNode;
		for( mNode = matches.FrontNode(); mNode; mNode = mNode->next )
		{
			PushText( mNode->data.c_str() );
		}
	}
	else if ( matches.Size() == 1 )
	{
		//strcpy( buf, matches.FrontNode()->data->c_str() );
		//strcat( buf, " " );
		buf = matches.FrontNode()->data;
		buf += " ";
		commandLine->SetTextChar( buf );
	}
}


void KrConsole::Print( const char* format, ... )
{
    va_list     va;
    char		buffer[1024];

    //
    //  format and output the message..
    //
    va_start( va, format );
    vsprintf( buffer, format, va );
    va_end( va );

	char* start;
	char* end;
	char* next;

	start = buffer;

	while ( start && *start )
	{
		end = strchr( start, '\n' );
		if ( end )
		{
			next  = end + 1;
			*end = 0;
			PushText( start );
			start = next;
		}
		else
		{
			PushText( start );
			start = 0;
		}			
	}
}


void KrConsole::AddCommand( const std::string& command )	//, IKrWidgetListener* handler )
{
	//AddListener( handler );

//	Command c;
//	c.command = command;
	//c.handler = handler;

	commandList.PushFront( command );
}

void KrConsole::GetEntryTextChar( std::string* buffer )
{
	*buffer = "";
	if ( commandLine )
		commandLine->GetTextChar( buffer );
}

