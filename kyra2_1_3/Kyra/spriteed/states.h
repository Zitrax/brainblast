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

#ifndef STATES_INCLUDED
#define STATES_INCLUDED

#include "../engine/krmath.h"
#include "../../tinyxml/tinyxml.h"
#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif
#include "../engine/engine.h"
#include "../engine/parser.h"

class Editor;
class SharedStateData;
struct SDL_Surface;
class CommandArgument;


class EdState
{
  public:
	EdState( Editor* _machine );
	virtual ~EdState() {}

	virtual void StateOpening()	{}
	virtual void StateClosing()	{}

	// Mouse click. Keymod passed for up and down.
	virtual void Click( int x, int y, bool command, int keymod )		{}

	// Called when the user has clicked and dragged.
	virtual void Drag( const grinliz::Rectangle2I& drag, bool complete )	{}

	virtual void Pan( int deltaX, int deltaY )				{}

	/// Called when the mouse is moving and no buttons are down.
	virtual void FreeMove( int x, int y, bool command )		{}

	virtual void MotionKey( int key )		{}
	virtual void FrameTick()				{}

	virtual void ZoomIn()	{}
	virtual void ZoomOut()	{}
	virtual void RightClick( int x, int y )	{}

	// Commands:
	virtual void Command( int id, CommandArgument* commArg, int nArg ) = 0;

	enum {
		EMPTY = 0,
		VIEW,
		MOVIE,
		ALIGN,
		NUMSTATES
	};

  protected:
	SharedStateData*	shared;
	Editor*				machine;
};



class EdStateEmpty : public EdState
{
  public:
	EdStateEmpty( Editor* _machine ) : EdState( _machine ) {}
	virtual ~EdStateEmpty() {}

	virtual void StateOpening();
	virtual void StateClosing();

	virtual void Command( int id, CommandArgument* commArg, int nArg )	{}
// 	void Draw();

  private:
	KrTextBox* textBox;
};


class EdStateView : public EdState
{
  public:
	EdStateView( Editor* _machine );
	virtual ~EdStateView();

	virtual void Command( int index, CommandArgument* arg, int nArg );

	virtual void StateOpening();
	virtual void StateClosing();

	virtual void ZoomIn();
	virtual void ZoomOut();
	virtual void RightClick( int x, int y );

	virtual void Click( int x, int y, bool command, int keymod );
	virtual void Drag( const grinliz::Rectangle2I& drag, bool complete );
	virtual void Pan( int deltaX, int deltaY );
	virtual void FreeMove( int x, int y, bool command );

  private:
	// Adds a frame OR tile
	void AddFrame( int x, int y, int width, int height );
	
	void SetSpriteName( const char* name );
	void SetAction( const char* name );
	void SetTileName( const std::string& tileName );

	// Delete the current frame OR tile
	void DeleteCurrent();

	// figures out the frame# of frame of action.
 	int  CalcFrameIndex( TiXmlElement* action, TiXmlElement* frame );

	// For tiles, their bounding box must be square. This function
	// consistently squares up the bounding box.
	void MakeSquare( grinliz::Rectangle2I* );
	
	void ScreenToCanvas( int x, int y, grinliz::Vector2I* object );

	// We create the engine when we open and delete it
	// when we close.
	enum
	{
		DEPTH_BACKGROUND,
		DEPTH_CANVAS,
		DEPTH_DRAGGING,
		DEPTH_ALIGN,

        ALIGNMENT_NCOLORS = 4,
	};

    KrRGBA ALIGNMENT_COLOR[ALIGNMENT_NCOLORS];

	KrBoxResource*	alignHRes;
	KrBoxResource*	alignVRes;
	KrBoxResource*	dragRes;
	KrImNode*		viewNode;
	KrCanvas*		canvas;

	GlFixed			scale;			// current zoom
	int				xPos, yPos;		// current position of the bitmap
};


class EdStateMovie : public EdState
{
  public:
	EdStateMovie( Editor* _machine );
	virtual ~EdStateMovie() {}

	virtual void Command( int id, CommandArgument* commArg, int nArg )	{}
	virtual void StateOpening();
	virtual void StateClosing();

	virtual void FrameTick();
	virtual void ZoomIn();
	virtual void ZoomOut();

  private:
	int					numFrames;
	GlFixed				scale;
	KrSpriteResource*	spriteResource;
	KrImNode*			movieNode;
};


class EdStateAlign : public EdState
{
  public:
	EdStateAlign( Editor* _machine );
	virtual ~EdStateAlign() {}

	virtual void Command( int id, CommandArgument* commArg, int nArg )	{}
	virtual void StateOpening();
	virtual void StateClosing();

	virtual void MotionKey( int key );
	virtual void ZoomIn();
	virtual void ZoomOut();

  private:
	enum
	{
		ID_OFFSET = 600,
	};
	// Called to put everyone in the correct place:
	void PositionSprites();

	int			numFrames;		// Total count of sprite frames.
	int			currentFrame;	// The current frame. goes from 0
								// to nframes - 1
	int			adjustingFrame;	// The frame thats data is changing.
								// if the current frame is 2 the adjusting
								// frame is 1.

	KrSpriteResource*	spriteResource;
	KrTextBox*			alignInfo;
	KrImNode*			alignNode;
	KrImNode*			spriteParent;

	GlFixed				scale;
};

#endif
