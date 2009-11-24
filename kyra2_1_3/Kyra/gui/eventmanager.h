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

#ifndef KYRA_KrEventManager_INCLUDED
#define KYRA_KrEventManager_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "SDL.h"
#include <list>

class KrEngine;
class KrWidget;

/**	Manages widgets -- if you use widgets, you need one of these! All SDL keyboard
	and mouse events should be sent to the
	widget system via the KrEventManager::HandleEvent method.

	The EventManager will set SDL_EnableUNICODE to true when it is created.
*/
class KrEventManager
{
  public:
	~KrEventManager();
	static KrEventManager* Instance()		{	if ( !instance ) instance = new KrEventManager();
												return instance;
											}

	/// Dispenses events to the widgets.
	bool HandleEvent( const SDL_Event& event, KrEngine* engine );

	/// Alternate call to set a widget accelerator. Usually easier to call the method on the widget itself.
	void SetAccelerator( int keymod, int keysym, KrWidget* );
	
	void AddListener( KrWidget* widget );
	void RemoveListener( KrWidget* widget );

	void GrabKeyFocus( KrWidget* w );
	void SelectMe( KrWidget* w );
	
  private:
	enum eFOCUS
	{
		FOCUS_KEY_INVALID = -1
	};
	struct Accel
	{
		int keymod;
		int keysym;
		KrWidget* target;

		bool operator==( const Accel& rhs )	{ return rhs.target == target; }
	};

	KrEventManager();
	static KrEventManager* instance;

	void ChangeKeyFocus( KrWidget* newFocus );

	bool HandleEventKeyDown( const SDL_Event& event, KrEngine* engine );
	bool HandleEventKeyUp( const SDL_Event& event, KrEngine* engine );
	bool HandleEventMouseMotion( const SDL_Event& event, KrEngine* engine );
	bool HandleEventMouseButton( const SDL_Event& event, KrEngine* engine );


	//int keyFocus;
	KrWidget* keyFocus;
	KrWidget* mouseFocus;
	bool      mouseDown;

	std::list< KrWidget* > keyListeners;
	std::list< KrWidget* > mouseListeners;

	std::list< Accel >		accelListeners;
};

#endif

