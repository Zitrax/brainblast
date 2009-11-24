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

#ifndef KYRA_CONSOLE_INCLUDED
#define KYRA_CONSOLE_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif 
#include <string>
#include <ctype.h>

#include "SDL.h"
#include "widget.h"
#include "../../grinliz/gltypes.h"
#include "../util/glcirclelist.h"
#include "../util/gllist.h"


class KrTextBox;
class KrImageTree;
class KrCanvasResource;
class KrCanvas;
class KrFontResource;
class KrImNode;
union KrRGBA;
class KrBoxResource;
class KrBox;
class KrTextWidget;


/**	A GUI Widget to display a (basic) console window.

	Emulates a simple command console, with one line text 
	editing, history buffer, and command completion.

	Needs SDL_EnableUNICODE( true ) to function correctly.

	<b> Events Sent </b>
	
	- ACTIVATED when a command is entered that is recognized 
	  by the console window. If the receiving listener as
	  been marked as the special "command handler" then
	  the 'data' parameter of HandleWidgetEvent will be
	  non zero.

*/
class KrConsole : public KrWidget
{
  public:
	/** The font resource to use is passed in, as well as the size
		of the console box. The lineSpacing can be used to give
		additional space between lines. The normal behavior is zero.

		WARNING: The font is in the scheme, and must be set.
	*/
	KrConsole(	int width, int height,
				int lineSpacing,
				const KrScheme& scheme );

	~KrConsole();

	virtual const char* WidgetType() { return "Console"; }

	/** Set a background color
		Only works after the KrConsole has been added to the Tree. 
	*/
	void SetBackgroundColor( const KrRGBA& color );

	// Not a mouse listener.

	// Not a key listener. Odd, but its child textwidget is what gets the keys.
	// Does get keys if children don't handle.
	virtual bool KeyEvent( const SDL_Event& key );

	// Not groupable.
	// Doesn't accelerate.
	// Doesn't handle events.

	/// Prints out a null termintated text string.
	void PushText( const char* text );
	/// Prints using "printf" format. Will handle newlines.
	void Print( const char* format, ... );

	/// Get the current text in the command line.
	void GetEntryTextChar( std::string* buffer );

	/** Adds a command to the list of recognized (and auto-completed) commands.
	*/
	void AddCommand( const std::string& );

	const KrTextBox* TextBox()		{ return textBox; }

	KrImNode* ToExtended( const std::string& name )		{ if ( name == "console" ) return this;
														  return 0;
														}

	virtual bool HandleWidgetEvent( KrWidget* source, const KrWidgetEvent& event );

	// Used to initialize the console.
	virtual void AddedtoTree();

  private:
	void PositionCursor();
	void ProcessEnterKey();
	void TabCompletion();

	enum
	{
		CURSOR_WIDTH = 2,
		COMMAND_BUF_SIZE = 32,
		LINE_BUF_SIZE    = 256
	};

	enum
	{
		DEPTH_BACKGROUND = -10,
		DEPTH_TEXT
	};

//	struct Command
//	{
//		std::string			command;
//		IKrWidgetListener*	handler;
//	};

	GlCircleList<std::string>	commandBuf;			// Command history
	int							commandBufSize;
	GlCircleNode<std::string>*	commandBufNode;		/// Current location in history
	GlSList<std::string>		commandList;		// List of recognized commands.

	KrTextBox*			textBox;
	int					width, height, lineSpacing;

	KrFontResource*		font;
	KrBoxResource*		backgroundRes;	
	KrBox*				background;
	KrTextWidget*		commandLine;
};


#endif
