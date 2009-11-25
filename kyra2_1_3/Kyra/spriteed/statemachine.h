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

#ifndef KYRA_ED_STATEMACINE_INCLUDED
#define KYRA_ED_STATEMACINE_INCLUDED

#include "../engine/krmath.h"
#include "../util/gllist.h"
#include "../engine/rle.h"
#include "../gui/console.h"
#include "states.h"
#include "sharedstate.h"


class Editor;
class KrAction;

enum {
	COMMAND_HELP = 1,	// Processed by the editor, not the states.
	COMMAND_LOAD,
	COMMAND_IMAGE,
	COMMAND_SAVE,
	COMMAND_NAME,
	COMMAND_ACTION,
	COMMAND_TILE,
	COMMAND_DELETE_CURRENT_FRAME,
	COMMAND_EXIT,
	COMMAND_FPS,
	COMMAND_DISPERSE,
	COMMAND_SET_UPPERLEFT,
	COMMAND_SET_LOWERRIGHT
};


class CommandArgument
{
  public:
	enum
	{
		UNKNOWN,
		STRING,
		INTEGER
	};

	CommandArgument()	: type( UNKNOWN ), integer( 0 )		{}

	int					Type() const		{ return type; }
	const std::string&	String() const		{ return str; }
	const char*			CharString() const  { return str.c_str(); }
	int					Integer() const		{ return integer; }

	void SetString( const std::string& _str )	{ str = _str; type = STRING; }
	void SetInteger( int i )					{ integer = i; type = INTEGER; }

  private:
	int type;
	std::string str;
	int integer;
};


class Editor : public IKrWidgetListener
{
  public:
	// The current state will construct itself as a child of the node.
	Editor( SDL_Surface* surface );
	virtual ~Editor();

	// For use by the command processor and event loop:
	// Inputs (mouse and command key)
	void MouseDown( int x, int y, int button, int keymod );
	void MouseUp(   int x, int y, int button, int keymod );
	void MouseMove( int x, int y );

	virtual void ZoomIn()		{ state[currentState]->ZoomIn(); }
	virtual void ZoomOut()		{ state[currentState]->ZoomOut(); }
	virtual void RightClick( int x, int y )	{ state[currentState]->RightClick(x,y); }

	void CommandDown()		{ commandDown = true;  MouseMove( mouseX, mouseY ); }
	void CommandUp()		{ commandDown = false; MouseMove( mouseX, mouseY ); }

	void MotionKey( int key )								{ state[ currentState ]->MotionKey( key ); }

	// Commands:
	void Command( int index, CommandArgument* arg, int nArg );

	void FrameTick()										{ state[ currentState ]->FrameTick(); }
	
	// General control:
	void Draw();

	// For use by the states:
	int	 State()					{ return currentState; }
	void ChangeState( int state );

	SharedStateData* SharedState()	{ return shared; }
	bool IsDragging()				{ return mouseDown && !commandDown; }
	bool IsFreeMove()				{ return !mouseDown; }
	bool IsCommand()				{ return commandDown; }
	int MouseX()					{ return mouseX; }
	int MouseY()					{ return mouseY; }
	int MouseDownX()				{ return mouseDownX; }
	int MouseDownY()				{ return mouseDownY; }

//	void HandleCommand( const std::string& commang, const std::string& arg );

	virtual bool HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event );

  private:
	enum { MOUSE_CLICK = 2 };

 	void LoadDef( const char* filename );
 	void LoadImage( const char* filename, int nTrans, KrRGBA* trans );

	EdState*	state[ EdState::NUMSTATES ];

	// Data about the inputs and statemachine:
// 	bool refresh;
	int  mouseX, mouseY;
	int  mouseDownX, mouseDownY;
	int  rightMouseDownX, rightMouseDownY;
	bool mouseDown;
	int	 rightMouseLastX, rightMouseLastY;
	bool rightButtonDown;

	bool commandDown;		// Does the mouseclick have the command flag set?
	bool commandClick;

	int  currentState;

	// Data for the states to use:
	SharedStateData* shared;

	int nTrans;
	KrRGBA trans[16];
};


// inline SharedStateData* EdState::SharedState()
// { 
// 	GLASSERT( machine );
// 	return machine->SharedState(); 
// }
// 

#endif
