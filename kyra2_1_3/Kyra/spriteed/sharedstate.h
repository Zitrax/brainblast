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


#ifndef KYRA_SHARED_STATE_INCLUDED
#define KYRA_SHARED_STATE_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <string>
#include "../../tinyxml/tinyxml.h"
#include "SDL.h"
#include "SDL_image.h"

#include "../engine/color.h"

class KrAction;
class KrEngine;
class KrCanvasResource;
class KrImageTree;
class KrImNode;
class KrConsole;
class EdWidget;
class EdWidgetFrame;
class EdWidgetTile;
class EdWidgetSprite;
class EdWidgetAction;
class KrFontResource;
class KrTextBox;
class KrBox;



enum
{
	INFO_CLEAR_START = 0,
	INFO_CLEAR_END = 4,
	
	INFO_SPRITE = 0,
	INFO_ACTION = 1,
	INFO_FRAME  = 2,
	INFO_FRAME_SIZE = 3,

	INFO_TILE = 0,
	INFO_TILE_SIZE = 1,

	INFO_CREATING = 4,
	INFO_MOUSE = 5,
};


class SharedStateData
{
  public:
	SharedStateData( SDL_Surface* surface );
	~SharedStateData();
	
	// Clear the widgets and filenames
	void Clear();

	void SetFilenames( const std::string& def, const std::string& surface )	{ defFileName = def; surfaceFileName = surface; }
	const std::string&	DefFileName();
	const std::string&	SurfaceFileName();
	
	/* The image that backs the screen is kept as a canvas
		resource so it can be used with the sprite engine. The 
		sprite engine is faster and it's a good test of the engine
		code.
	*/
	KrCanvasResource*	CanvasResource()		{ return canvasResource; }

	// The engine we are using:
	KrEngine* Engine()		{ return engine; }

	// The console to output to:
	KrConsole* Console()	{ return console; }

	// The console in contained in this node:
	KrImNode*  ConsoleNode()	{ return consoleHolder; }

	// The image node that states should be a child of:
	KrImNode* ImNode()		{ return imnode; }

	// The view state tosses its (considerable) widget set in
	// this node:
	KrImNode* WidgetNode()	{ return widgetNode; }

	// The base widget, once the XML stuff is parsed to widgets.
	EdWidget* Widget()		{ return widget; }

	KrFontResource*		FontResource()	{ return fontResource; }
	KrCanvasResource*	BackgroundRes()	{ return backgroundRes; }

	void SetInfoBox( bool on );
	void SetInfoBoxMouse( int x, int y );
	
	enum {
		SPRITE,
		TILE
	};

	int				CurrentObject()		{ return currentObject; }
	EdWidgetFrame*	CurrentFrame()		{ return currentFrame; }
	EdWidgetAction*	CurrentAction()		{ return currentAction; }
	EdWidgetSprite*	CurrentSprite()		{ return currentSprite; }
	EdWidgetTile*	CurrentTile()		{ return currentTile; }

	// Sets both the current tile/sprite and switches to that mode.
	void SetCurrentTile( EdWidget* tile );
	void SetCurrentFrame( EdWidget* frame );
	void SetCurrentAction( EdWidget* action );
	void SetCurrentSprite( EdWidget* sprite );

	// Converts the current action into a KrAction, and returns
	// the object. Used to feed to the engine for rendering.
	KrAction* CreateAnimationAction();

	// SetFilenames must be set before calling LoadSurface.
	bool LoadSurface( int nTransparency, const KrRGBA* rgba );

	// Transparency values for non-32 bit surfaces.
	enum { 
		MAX_TRANS = 10
	};
  private:
	enum {
		MAIN_IMNODE_DEPTH,
		WIDGET_DEPTH,
		CONSOLE_DEPTH,
		INFO_BACKGROUND_DEPTH,
		INFO_DEPTH,
	};

	KrEngine*		engine;			
	KrImNode*		imnode;		// The node where states hang there data from
	KrImNode*		widgetNode;	// Special container for the view state.
	KrImNode*		consoleHolder;
	KrConsole*		console;
	KrTextBox*		infoBox;
	KrBox*			infoBoxBack;
	EdWidget*		widget;
	KrFontResource*		fontResource;
	KrCanvasResource*	backgroundRes;

	std::string		surfaceFileName;
	std::string		defFileName;

	KrAction*		animAction;
	int				currentObject;
	EdWidgetTile*	currentTile;
	EdWidgetFrame*	currentFrame;
	EdWidgetAction*	currentAction;
	EdWidgetSprite*	currentSprite;
	KrCanvasResource* canvasResource;

 	KrRGBA			trans[MAX_TRANS];
	int				nTrans;
};


#endif
