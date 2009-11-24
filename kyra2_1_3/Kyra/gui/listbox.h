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

#ifndef KYRA_SCROLLBOX_INCLUDED
#define KYRA_SCROLLBOX_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "widget.h"
class KrTextWidget;


/**	A very simple listbox. Items can be added, but not removed. It can
	be scrolled, but through the method MoveSelection. Normally, you set
	up button to accomplish this. (The guitest demonstrates this.)

	<b> Events Sent </b>
	- SELECTION when an item in the listbox is selected.
*/
class KrListBox : public KrWidget
{
  public:
	/** Constructs a list box.

		@param width		Width in pixels of the list box.
		@param height		Height in pixels of the list box.
		@param scheme		The color palette and font to use to render the list box.
		@param drawBorder	Whether or not a bevel should be drawn around the list box.
	*/
	KrListBox(	int width, int height, 
				const KrScheme& scheme, 
				bool drawBorder );

	virtual ~KrListBox();

	virtual const char* WidgetType() { return "ListBox"; }

	/// Return the number of items in the list box.
	int NumItems()									{ return textStrings.size(); }
	/// Return the index of the selected item.
	int SelectedItem()								{ return selectedItem; }
	/// Return the text of the selected item.
	std::string SelectedItemText()					{ if ( selectedItem >= 0 && selectedItem < (int)textStrings.size() )
														return textStrings[selectedItem];
													  else
														return "";
													}

	/// Add an item. Returns the index of the item.
	int AddTextChar( const std::string& text );
	/// Query an item and write it to the "text" string.
	void GetTextChar( int index, std::string* text );
	/// Move the selection, positive or negative, scrolling if necessary.
	void MoveSelection( int delta );
	/// Move the selection up one.
	void MoveUpOne()	{ MoveSelection( -1 ); }
	/// Move the selection down one.
	void MoveDownOne()	{ MoveSelection( 1 ); }

	virtual bool HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event );

	// Used to initialize the widget.
	virtual void AddedtoTree();

  private:
	void DrawText();
	void ItemSelected( KrWidget* source );

	int width, height;
//	KrFontResource* font;
	KrBevelElement  *outerBevel;

	std::vector< KrTextWidget* > textWidgets;
	std::vector< std::string >   textStrings;

	int firstItem;
	int selectedItem;
};



#endif

