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

#include "textwidget.h"
#include "listbox.h"
#include "../engine/fontresource.h"

using namespace grinliz;

KrListBox::KrListBox(	int _width, int _height, 
						const KrScheme& s,
						bool drawBorder ) : KrWidget( s )
{
	width = _width;
	height = _height;
	firstItem = 0;
	selectedItem = 0;

	unsigned numVisibleItems = ( height - 2 ) / scheme.font->FontHeight();
	textWidgets.resize( numVisibleItems );
	
	height = numVisibleItems * scheme.font->FontHeight() + 2;

	outerBevel = 0;
	if ( drawBorder )
		outerBevel = new KrBevelElement( width, height, scheme );
}

KrListBox::~KrListBox()
{
	delete outerBevel;
}

void KrListBox::AddedtoTree()
{
	if ( outerBevel )
	{
		outerBevel->AddToTree( Engine(), this );
		outerBevel->DrawIn();
	}

	for( unsigned i=0; i<textWidgets.size(); ++i )
	{
		textWidgets[i] = new KrTextWidget( width /*- scrollWidth*/ - 2, scheme.font->FontHeight(),
										   false, true, false, scheme );
		textWidgets[i]->SetPos( 1, i * scheme.font->FontHeight() + 1 );
		Engine()->Tree()->AddNode( this, textWidgets[i] );
		//textWidgets[i]->AddListener( this );
		//textWidgets[i]->SelectedSignal1.connect( this, &KrListBox::ItemSelected );
		//textWidgets[i]->widgetPublish.AddListener( this );
		textWidgets[i]->widgetPublish.AddListener( this );
	}
	DrawText();
}


bool KrListBox::HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event )
{
	if ( event.type == KrWidgetEvent::SELECTION )
	{
		ItemSelected( source );
		return true;
	}
	return false;
}

void KrListBox::ItemSelected( KrWidget* source )
{
	unsigned i;
	KrColorTransform normal;
	KrColorTransform selected = scheme.CalcHiPrimary();

	for( i=0; i<textWidgets.size(); ++i )
	{
		if ( textWidgets[i] == source  )
		{
			int indexToString = i + firstItem;
			if ( InRange( indexToString, 0, (int) textStrings.size()-1 ) )
			{
				selectedItem = indexToString;

				KrWidgetEvent event;
				event.type = KrWidgetEvent::SELECTION;
				event.selection.index = indexToString;
				event.selection.text = textStrings[selectedItem].c_str();

				//PublishEvent( SELECTION, indexToString, 0, 0, 0 );
				//ItemSelectedSignal0.emit();
				//ItemSelectedSignal1.emit( textStrings[selectedItem] );
				for(	Publisher< IKrWidgetListener >::const_iterator it = widgetPublish.begin();
						it != widgetPublish.end();
						++it )
				{
					(*it)->HandleWidgetEvent( this, event );
				}
				break;
			}
		}
	}

	for( i=0; i<textWidgets.size(); ++i )
	{
		if ( i == (unsigned) ( selectedItem - firstItem ) )
			textWidgets[i]->SetColor( selected );
		else
			textWidgets[i]->SetColor( normal );
	}
}


int KrListBox::AddTextChar( const std::string& text )
{
	textStrings.push_back( text );
	DrawText();
	return textStrings.size() - 1;
}


void KrListBox::GetTextChar( int id, std::string* text )
{
	*text = "";
	if ( id < (int) textStrings.size() )
		*text = textStrings[ id ];
}


void KrListBox::MoveSelection( int delta )
{
	if (    delta != 0
	     && InRange( selectedItem+delta, 0, (int) textStrings.size()-1 ) )
	{
		selectedItem += delta;
		if ( selectedItem < firstItem )
			firstItem = selectedItem;
		else if ( selectedItem >= firstItem + (int) textWidgets.size() )
			firstItem = selectedItem - textWidgets.size() + 1;

		//PublishEvent( SELECTION, selectedItem, 0, 0, 0 );
		//ItemSelectedSignal0.emit();
		//ItemSelectedSignal1.emit( textStrings[selectedItem] );
		KrWidgetEvent event;
		event.type = KrWidgetEvent::SELECTION;
		event.selection.index = selectedItem;
		event.selection.text = textStrings[selectedItem].c_str();

		for(	Publisher< IKrWidgetListener >::const_iterator it = widgetPublish.begin();
				it != widgetPublish.end();
				++it )
		{
			(*it)->HandleWidgetEvent( this, event );
		}

		DrawText();
	}
}


void KrListBox::DrawText()
{
	for( int i=0; i<(int)textWidgets.size(); ++i )
	{
		KrColorTransform normal;
		KrColorTransform selected = scheme.CalcHiPrimary();

		int index = i + firstItem;
		textWidgets[i]->SetColor( normal );

		if ( InRange( index, 0, (int) textStrings.size()-1 ) )
		{
			textWidgets[i]->SetTextChar( textStrings[ index ] );

			if ( selectedItem == index )
			{
				textWidgets[i]->SetColor( selected );
			}
		}
	}
}
