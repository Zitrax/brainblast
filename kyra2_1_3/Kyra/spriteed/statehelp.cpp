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
#include "sharedstate.h"
#include "../engine/fontresource.h"
#include "../engine/textbox.h"
#include "../engine/box.h"



void EdStateEmpty::StateOpening()
{
	KrFontResource* res = shared->Engine()->Vault()->GetFontResource( "CONSOLE" );
	GLASSERT( res );

	KrRect rect;
	rect = shared->Engine()->ScreenBounds();

	textBox = new KrTextBox( res, rect.Width(), rect.Height(), 0 );
	GLASSERT( textBox );
	shared->Engine()->Tree()->AddNode( shared->ImNode(), textBox );

	const char* help[] = 
	{
		"F1        Display this help screen.               F10        make a screenshot.",
		"F2        View mode: where sprites are defined.",
		"F3        Alignment mode: arrange the frames of a sprite relative to one another.",
		"F4        Movie mode: watch the sprite animate.",
		"KeyPad+,- Zoom in and out. (All modes.)",
		"",
		"At the console: (Note the console supports tab completion and history buffer.)",
		"  save [filename.xml] Saves the xml file. The current is used as default.",
		"  exit                Exit the editor. Remember to save first!",
		"",
		"  Help mode:",
		"    load filename.xml   Load an existing XML file for editing.",
		"    image filename [transparent] [transparent] Loads an image file to start editing.",
		"               filename.xml will be created, 'transparent' is an optional transparency key, in hex rrggbb.",
		"  View mode:",
		"    sprite  spriteName    Creates a sprite.       disperse x y  Spread out the sprite's frames.",
		"    action  actionName    Creates an action.      fps time      Movie playback speed in frames/second.",
		"    tile    tileName      Creates a tile.         ",
		"",
		"View Mode: ",
		"  left-click      Selects a frame or tile.        ctr-left-click  Sets the hotspot for the current frame.",
		"  (left or right)-shift left-click                Set the upper left or lower right frame bound.",
		"  left-drag       Creates a new frame.",
		"  right-click     Zoom to click point.",
		"  right-drag      Pan the view window.",
		"  shift-delete    Delete the current frame.",
		"",
		"Alignment mode:",
		"  page-up         Next frame.",
		"  page-down       Previous frame.",
		"  arrow keys      Move the current frame.",
		0
	};

	int i;
	for( i=0; help[i]; i++ )
	{
		textBox->SetTextChar( help[i], i );
	}

	shared->ConsoleNode()->SetVisible( true );
	shared->SetInfoBox( true );
}


void EdStateEmpty::StateClosing()
{
	shared->Engine()->Tree()->DeleteNode( textBox );
}
