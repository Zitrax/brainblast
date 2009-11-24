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

#ifndef KYRA_BUTTONWIDGET_INCLUDED
#define KYRA_BUTTONWIDGET_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "../gui/widget.h"

/** A super class for more specialized buttons. It's only 
	external method is to set the text of the button.
*/
class KrButton : public KrWidget
{
  public:
	virtual ~KrButton();

	/// Set the text of the button. The font used is specified in the Scheme.
	void SetTextChar( const std::string& text );

	/** Put a sprite on the button, as an icon or "decal" image. The sprite
		is passed to the Button, which owns it and will delete it when the
		Button is deleted. The Frame and Action of the Sprite will be unchanged
		by the button.

		You can change the Frame or Action, but the button will always position
		and size the icon based on the Frame and Action at the time it was added.

		The icon can be changed at any time, and the button will clean up memory
		and create a new one.
	*/
	void SetIcon( KrSprite* giveSprite );

	// Used to initialize the widget.
	virtual void AddedtoTree();

  protected:
	KrButton(	int width, 
				int height,
				const KrScheme& scheme );
	KrButton(	KrSprite* graphic,
				const KrScheme& scheme );


	enum
	{
		OVER = 0x01,
		DOWN = 0x02,

		ICON_DEPTH = 1,
		TEXT_DEPTH = 2,
	};
	void SetMode( int mode );	// bitflags of OVER & DOWN

	virtual void ModeChange(KrColorTransform& color);
	void PlaceText();
	void PlaceIcon();

	int width, height, mode;
	KrSprite	  *icon;
		
  private:
	KrImNode	  *holder;
	KrBoxResource *plateRes;
	KrBox		  *plate;
	KrBevelElement bevel;
	bool			userDrawn;
	int			   iconX, iconY;

	KrTextBox*		  textBox;
	std::string		  text;
};


/**	A push button that is only depressed while the mouse or accelerator
	key is down.

	This type of button is not groupable or selectable.

	<b> Events Sent </b>

	- ACTIVATED when the button is pushed down.
	- DEACTIVATED when the button is released.

	There are 2 approaches to rendering a Button. The first is to let
	the system draw a square button, optionally with a text or icon. No
	graphics are required to be supplied, and this is a fast way to get started.

	The second approach is to provied a 3 frame Sprite, where the 
	frames represent the UP, OVER, and DOWN states.
*/
class KrPushButton : public KrButton
{
  public:
	/// Creates a push button, drawn by the system.
	KrPushButton(	int width, 
					int height,
					const KrScheme& scheme ) : KrButton( width, height, scheme )	{}

	/** Creates a push button, and potentially a much better one.
		A 3 frame sprite must be provided. Frame 0: UP state, Frame 1: OVER 
		state, and Frame 2: DOWN state. The scheme is only used for the text.
	*/
	KrPushButton(	KrSprite* graphic,
					const KrScheme& scheme ) : KrButton( graphic, scheme ) {}

	virtual ~KrPushButton()	{}

	virtual const char* WidgetType() { return "PushButton"; }

	virtual int  IsMouseListener()						{ return LEFT_MOUSE; }

	virtual void MouseIn( bool down, bool in );
	virtual void MouseMove( bool down, int x, int y );
	virtual bool MouseClick( int down, int x, int y );

	// Not a key listener.
	// Not groupable or selectable.

	virtual bool CanAccelerate()						{ return true; }
	virtual void Accelerate( bool down );
};


/**	A toggle type button, that clicks down and stays down until clicked again.
	It also responds to hot keys.

	The toggle button is selectable and can be accelerated.

	<b> Events Sent </b>

	- ACTIVATED when the button switches to the down state.
	- DEACTIVATED when the button switches to the up state.

	There are 2 approaches to rendering a Button. The first is to let
	the system draw a square button, optionally with a text or icon. No
	graphics are required to be supplied, and this is a fast way to get started.

	The second approach is to provied a 3 frame Sprite, where the 
	frames represent the UP, OVER, and DOWN states.
*/
class KrToggleButton : public KrButton
{
  public:
	/// Creates a toggle button.
	KrToggleButton(	int width, 
					int height,
					const KrScheme& scheme ) : KrButton( width, height, scheme )	{}

	/** Creates a toggle button, and potentially a much better one.
		A 3 frame sprite must be provided. Frame 0: UP state, Frame 1: OVER 
		state, and Frame 2: DOWN state. The scheme is only used for the text.
	*/
	KrToggleButton(	KrSprite* graphic,
					const KrScheme& scheme ) : KrButton( graphic, scheme ) {}

	~KrToggleButton()	{}

	virtual const char* WidgetType() { return "ToggleButton"; }

	virtual int IsMouseListener()						{ return LEFT_MOUSE; }
	virtual void MouseIn( bool down, bool in );
	virtual void MouseMove( bool down, int x, int y );
	virtual bool MouseClick( int down, int x, int y );

	// Not a key listener.

	virtual bool IsSelectable()							{ return true; }
	virtual void Selected( bool selected );
//	virtual void IsGroup()								{ return true; }
	virtual void Accelerate( bool down );
};


#endif

