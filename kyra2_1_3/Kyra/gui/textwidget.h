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

#ifndef KYRA_TEXTWIDGET_INCLUDED
#define KYRA_TEXTWIDGET_INCLUDED

#include "../gui/widget.h"

/**	A "widget" version the the textbox. The regular textbox (KrTextBox) 
	can be used to display dynamic, but not editable text. The widget
	version can be editable, and integrated with the widget
	event system.

	<b> Events Sent </b>
	- ACTIVATED when the 'return' key is pressed.
	- SELECTION when it gets or loses focus.
*/
class KrTextWidget : public KrWidget
{
  public:
	/**	A rather feature rich constructor. 
		@param drawBorder		Draw a square outline arount the text box.
		@param drawBackground	Fill in the box behind the text. If false, transparent.
		@param editable			If the textbox should support a cursor and keyboard editing.
		@param scheme			Provieds the color palette and font for this textbox.
	*/
	KrTextWidget(	int capacity, int width, int height, 
					bool drawBorder,
					bool drawBackground,
					bool editable,
					const KrScheme& scheme
				 );

	KrTextWidget(	int width, int height, 
					bool drawBorder,
					bool drawBackground,
					bool editable,
					const KrScheme& scheme
				 );

	~KrTextWidget();

	virtual const char* WidgetType() { return "TextWidget"; }

	virtual int  IsMouseListener()						{ return LEFT_MOUSE; }
	virtual void MouseIn( bool down, bool in );
	virtual void MouseMove( bool down, int x, int y )	{}
	virtual bool MouseClick( int down, int x, int y );

	virtual bool IsKeyListener()						{ return editable; }
	virtual void KeyFocus( bool focus );
	virtual bool KeyEvent( const SDL_Event& key );

	/// Set the text of the box.
	void SetTextChar( const std::string& );
	/// Get the text of the box to the given string.
	void GetTextChar( std::string* buffer );
	int Width()		{ return width; }
	int Height()	{ return height; }

	// Used to initialize the widget.
	virtual void AddedtoTree();

  private:
	enum
	{
		CURSOR_WIDTH = 2
	};

	void PositionCursor();

	int width, height;
	bool drawBorder;
	bool drawBackground;
	bool editable;
	
	int cursorPos, maxSpaces;

	KrImNode*		holder;
	KrBevelElement* bevel;
	KrBoxResource *plateRes, *cursorRes;
	KrBox		  *plate, *cursor;
	KrTextBox	  *textBox;
};


#endif
