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


#ifndef KYRA_WIDGET_INCLUDED
#define KYRA_WIDGET_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <string>
#include "SDL.h"
#include "../util/gllist.h"
#include "../../grinliz/glpublisher.h"
#include "../engine/engine.h"
#include "../engine/color.h"


/** An "activated" event means a switch has turned on, a button has been depressed, etc.
*/
struct KrWidgetEventActivated
{
	int type;
};

/** A "Deactivated" event occurs when something that was activated is no longer. 
*/
struct KrWidgetEventDeactivated
{
	int type;
};

/** A "Command" event has a 'command' and 'argument' string. Usually generated
	by the console or a similar text based widget that recognizes a set of commands. 
	When a string is split, the first word is the "command" and all that follows is
	the "arg". If the widget recognized the command - it is in the "command list" - 
	recognized will be set to "true".
*/
struct KrWidgetEventCommand
{
	int type;
	bool recognized;
	const char* command;
	const char* arg;
};

/** A "Selection" event is a choice from a set of items. A list box selection for 
	example. It has an 'index' which is a numerical id of the selection, and a 'text'
	which is the string value. Some widgets don't have text selection, in which case
	'text' will be null. An ImageListBox will have text==0 for its SELECTION events.
*/
struct KrWidgetEventSelection
{
	int type;
	int index;
	const char* text;
};

union KrWidgetEvent
{
	enum
	{
		NO_EVENT,
		ACTIVATED,
		DEACTIVATED,
		COMMAND,
		SELECTION,
	};

	int type;
	KrWidgetEventActivated		activated;		// ACTIVATED
	KrWidgetEventDeactivated	deactivated;	// DEACTIVATED
	KrWidgetEventCommand		command;		// COMMAND
	KrWidgetEventSelection		selection;		// SELECTION
};


/** An abstract class, the HandleWidgetEvent when a widget
	listener receives an event. The events widgets publish
	are documented on a per-widget basis.

	The class is abstract, so it can be used as a mix in and
	there are no multiple inheritance issues.

	The HandleWidgetEvent is a strange method with a lot of
	paremeters. An obvious solution would be to send a WidgetEvent
	class that is abstract and polymorphic...but Kyra doesn't use/require
	dynamic cast, so implementing that is less useful than it
	seems.
*/

class IKrWidgetListener : public grinliz::Listener< IKrWidgetListener >
{
  public:


	virtual bool HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event ) = 0;
};


/**	A scheme defines a color system for widgets. Each widget
	can have it's own scheme, or an entire widget set can
	share the same one. (Or at least a copy of the same one.)

	A scheme consists of a font (some widgets allow this to
	be null), a primary color, and a secondary color that
	should visually match the primary color, and a cursor
	color. All other colors	used are calculated from the 
	primary and secondary.
*/
struct KrScheme
{
	KrScheme( KrFontResource* font );

	KrRGBA CalcBrightLine() const;
	KrRGBA CalcShadowLine() const;

	KrColorTransform CalcHiPrimary() const;
	KrColorTransform CalcHiSec() const;
	KrColorTransform CalcDark() const;
	KrColorTransform CalcDarkSec() const;

	KrRGBA				primary;
	KrRGBA				cursor;
	KrColorTransform	secondary;
	KrFontResource*		font;
	
	enum
	{
		BRIGHT = 60,
		DARK   = 60
	};
};


struct KrBevelElement
{
  public:
	KrBevelElement( int w, int h, const KrScheme& );
	~KrBevelElement();

	void AddToTree( KrEngine*, KrImNode* parent );
	void DrawIn();
	void DrawOut();

	int width;
	int height;
	KrBoxResource *vertDR, *vertLR, *horDR, *horLR;
	KrBox *vertD, *vertL, *horD, *horL;
};


/**	This is the base of any Kyra widget. A widget is much like any
	other Kyra image object. You new it, add it to the KrImageTree,
	and it is drawn as part of the Draw() pass.

	Widgets don't have resources, but do use schemes which are similar.
	
	Widgets get their events from an event manager. If you use widgets,
	you must therefore send events to the KrEventManager class. Widgets 
	generally need SDL_EnableUNICODE( true ) to function correctly.

	Widgets broadcast their events to "listeners." To register a class
	as a listener, call AddListener(). The class will then receive 
	notification of events. The event each widget broadcast is documented
	on a per-widget basis.

	A widget subclass can implement any of the following properties:

	- MouseListeners receieve and can respond to mouse events.
	- KeyListeners get keyboard focus and key messages. They grab
	  key focus with the "tab" key or if clicked on.
	- Selectable widgets have state. They can be on/off, up/down, or whatever.
	  A KrToggleButton is a selectable widget.
	- Groupable widgets are in a group, and are aware of other widgets 
	  in that group. Radio Buttons are groupable.
	- Some widgets can respond to accelerator keys. Use SetAccelerator
	  to specify a hotkey to pass to the widget.

	Widgets can be nested and you can query for parents. However, Kyra regards
	the widget model as flat and doesn't pay attention to the nesting, with
	one important exception: a key event that isn't handled by a widget
	will be passed through its parent chain. For example, a KrConsole
	uses  a KrTextWidget. If the KrTextWidget doesn't handle a 
	particular key, it will get passed up to the KrConsole.

	WARNING: You may want to use Widgets in window 0, if you are using multiple Kyra
	windows. They are not fully tested in higher window numbers. You may see placement
	or bounding errors in higher window numbers.

	Notes for implementing your own widgets:

	- All Widgets are of the type ToWidget(). To get a particular flavor,
	  the ToExtended() method can be used.
	- Widgets should initialize themselves in the AddedtoTree() method. When
	  the engine calls your AddedtoTree() method, you can add children because
	  you are already in the Tree(). Be sure to call the parent
	  KrWidget::AddedtoTree() as well!
	- If you have been added to the Tree(), your Engine() pointer will be non-null.
*/
class KrWidget : public KrImNode, public IKrWidgetListener
{
  public:
	virtual ~KrWidget();
	virtual KrWidget* ToWidget()	{ return this; }

	/** All widgets publish events through this member.
	*/
	grinliz::Publisher< IKrWidgetListener > widgetPublish;

	enum {
		LEFT_MOUSE   = 1,
		RIGHT_MOUSE  = 2,	// not currently supported
		MIDDLE_MOUSE = 4,	// not currently supported

		LEFT_UP   = 0,
		LEFT_DOWN = 1,
		RIGHT_UP   = 2,		// not currently supported
		RIGHT_DOWN = 3,		// not currently supported
		MIDDLE_UP  = 4,		// not currently supported
		MIDDLE_DOWN = 5		// not currently supported
	};

	virtual const char* WidgetType() { return "Widget"; }

	/**	IsMouseListener returns whether this is a mouse listener or not, and which buttons
		are listened to. A return value of 0 is no listening. Else
		it can return an OR mask of the buttons ( LEFT_MOUSE, RIGHT_MOUSE,
		MIDDLE_MOUSE ) it wants to listen for mouse clicks. 

		The simple case is to only listen to the left mouse, in which case
		return LEFT_MOUSE (1). The click messages can then be treated like
		a boolean for the left mouse (1 is down, 0 is up.)

		Currently, with version 2.0, only the LEFT_MOUSE is supported.

	
		MouseIn is called when a mouse moves in to the widget. The 'down' parameter
		reflects the state of the left mouse button. The 'in' reflects whether
		it is moving to the widget (true) or away from the widget (false).


		MouseMove reports when the mouse moves over this widget.


		MouseClick is called when the mouse is clicked on this widget.
		The 'click' param will have a single value (not OR mask) of
		LEFT_UP, LEFT_DOWN, RIGHT_UP, etc. with x and y coordinates
		of the action. 
		
		In the simple case that you are only listening to the left mouse,
		the parameter will be essentially a boolean: 1 for the left mouse
		down, 0 for the left mouse up.
	*/
	virtual int  IsMouseListener()						{ return 0; }
	virtual void MouseIn( bool down, bool in )			{}					///< @sa IsMouseListener
	virtual void MouseMove( bool down, int x, int y )	{}					///< @sa IsMouseListener
	virtual bool MouseClick( int click, int x, int y )	{ return false; }	///< @sa IsMouseListener

	virtual bool IsKeyListener()						{ return false; }
	virtual void KeyFocus( bool focus )					{}
	virtual bool KeyEvent( const SDL_Event& key )		{ return false; }

	virtual bool IsSelectable()							{ return false; }
	virtual void Selected( bool selected )				{}

//	virtual bool IsGroup()								{ return false; }

	virtual void Accelerate( bool down )				{}
	void SetAccelerator( int keymod, int keysym );

	/// Handle widget events, return true if handled, false if not ours
	virtual bool HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event )	{ return false; }

	/// Find the parent of the widget that is also a widget.
	KrWidget* ParentWidget();

  protected:
	KrWidget( const KrScheme& scheme );

	int groupId;
	KrScheme scheme;
};

#endif
