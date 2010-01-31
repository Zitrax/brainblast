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

#include "widget.h"
#include "eventmanager.h"
#include <algorithm>
#include "../engine/textbox.h"

using namespace grinliz;

/*static*/ KrEventManager* KrEventManager::instance = 0;


KrEventManager::KrEventManager()
{
	keyFocus = 0;
	mouseFocus = 0;
	mouseDown  = false;
	SDL_EnableUNICODE( true );	// Force this! Too many bugs that it's not on.
}

KrEventManager::~KrEventManager()
{
	instance = 0;	// Very important, so we don't screw up if we're re-created.
}


void KrEventManager::AddListener( KrWidget* widget )
{
	if ( widget->IsKeyListener() )
	{
		keyListeners.push_back( widget );
		if ( keyFocus == 0 )
		{
			ChangeKeyFocus( widget );
		}
	}
	if ( widget->IsMouseListener() & KrWidget::LEFT_MOUSE )
	{
		mouseListeners.push_back( widget );
	}
}


void KrEventManager::RemoveListener( KrWidget* widget )
{
	//release keyfocus as well, before deleting, so no hanging focus to invalid index...
	keyListeners.remove( widget );
	mouseListeners.remove( widget );

	Accel r = { 0, 0, widget };
	accelListeners.remove( r );

	if ( mouseFocus == widget )
		mouseFocus = 0;
	if ( keyFocus == widget )
		keyFocus = 0;
}


void KrEventManager::ChangeKeyFocus( KrWidget* newFocus )
{
	if ( newFocus != keyFocus )
	{
		if ( keyFocus )
			keyFocus->KeyFocus( false );
		if ( newFocus )
			newFocus->KeyFocus( true );
		keyFocus = newFocus;
	}
}


void KrEventManager::GrabKeyFocus( KrWidget* w )
{
//	int i = keyListeners.Find( w );
//	if ( i >= 0 )
//		ChangeKeyFocus( i );
	
	std::list< KrWidget* >::iterator it = std::find( keyListeners.begin(), keyListeners.end(), w );
	if ( it != keyListeners.end() )
		ChangeKeyFocus( w );
}


void KrEventManager::SetAccelerator( int keymod, int keysym, KrWidget* target )
{
	std::list< Accel >::iterator it;
	for ( it=accelListeners.begin(); it != accelListeners.end(); ++it )
	{
		if ( (*it).target == target )
		{
			(*it).keymod = keymod;
			(*it).keysym = keysym;
			break;
		}
	}
	if ( it == accelListeners.end() )
	{
		Accel a = { keymod, keysym, target };
		accelListeners.push_back( a );
	}
}


void KrEventManager::SelectMe( KrWidget* w )
{
	if ( w->IsSelectable() )
	{
		KrImNode* parent = w->Parent();

		for( 	GlInsideNode< KrImNode* >* childNode = parent->Child()->next;	// after sentinel
				childNode != parent->Child();     								// sentinel
				childNode = childNode->next )
		{
			KrWidget* widget = childNode->data->ToWidget();
			if ( widget )
			{
				if ( w == widget )
					widget->Selected( true );
				else
					if ( widget->IsSelectable() )
						widget->Selected( false );
			}
		}
	}
}


bool KrEventManager::HandleEvent( const SDL_Event& event, KrEngine* engine )
{
	switch(event.type)  
	{
		case SDL_KEYDOWN :		return HandleEventKeyDown(event, engine);
		case SDL_KEYUP :		return HandleEventKeyUp(event, engine);
		case SDL_MOUSEMOTION :	return HandleEventMouseMotion(event, engine);
		case SDL_MOUSEBUTTONDOWN :
		case SDL_MOUSEBUTTONUP :return HandleEventMouseButton(event, engine);
	}

	return false;
}


bool KrEventManager::HandleEventKeyDown( const SDL_Event& event, KrEngine* engine )
{
		//	- the tab key changes key focus.
		//	- accelerators are checked
		//	- keys passed through to the handler.

//		#ifdef DEBUG
//		GLOUTPUT( "KeyDown mod=%d sym=%d\n", event.key.keysym.mod, event.key.keysym.sym );
//		#endif

	if (    event.key.keysym.sym == SDLK_TAB 
		 && keyListeners.size() > 1 )
	{
		if ( keyFocus ) {
			std::list< KrWidget* >::iterator it = std::find( keyListeners.begin(), keyListeners.end(), keyFocus );
			GLASSERT( it != keyListeners.end() );

			if ( event.key.keysym.mod & KMOD_SHIFT ) {
				if ( it == keyListeners.begin() )
					it = keyListeners.end();
				--it;
			}
			else {
				++it;
				if ( it == keyListeners.end() )
					it = keyListeners.begin();
			}
			ChangeKeyFocus( *it );
		}
		else 
		{
			ChangeKeyFocus( keyListeners.front() );
		}
		return true;
	}

	for( std::list< Accel >::iterator it = accelListeners.begin(); it != accelListeners.end(); ++it )
	{
		int sym = (*it).keysym;
		int mod = (*it).keymod;

		if (    event.key.keysym.sym == sym
			 && event.key.keysym.mod & mod )
		{
			(*it).target->Accelerate( true );
			return true;
		}
	}

	if ( !keyListeners.empty() )
	{
//		keyFocus = GlClamp( keyFocus, FOCUS_KEY_INVALID + 1, int( keyListeners.Count()-1 ) );
//		KrWidget* widget = keyListeners[ keyFocus ];

		// Go up the chain until handled.
		for( KrWidget* widget = keyFocus; widget; widget = widget->ParentWidget() )
		{
			if(	widget->KeyEvent( event ) )
				return true;
		}
	}

	return false;
}

	
bool KrEventManager::HandleEventKeyUp( const SDL_Event& event, KrEngine* engine )
{
	// - only accelerates key up
	for( std::list<Accel>::iterator it = accelListeners.begin(); it != accelListeners.end(); ++it )
	{
		if (    event.key.keysym.sym == (*it).keysym
			 && event.key.keysym.mod & (*it).keymod )
		{
			(*it).target->Accelerate( false );
			return true;
		}
	}

	return false;
}


bool KrEventManager::HandleEventMouseMotion( const SDL_Event& event, KrEngine* engine )
{
	//GlDynArray<KrImage*> hitArray;
	std::vector<KrImage*> hitArray;
	KrWidget* hit = 0;
	int window = engine->GetWindowFromPoint( event.motion.x, event.motion.y );

	engine->Tree()->HitTest( event.motion.x, event.motion.y, 0,
							 KrImageTree::ALWAYS_INSIDE_BOX, //| GET_ALL_HITS,
							 &hitArray,
							 &window );

	Vector2< GlFixed > object;
	for( unsigned i=0; i<hitArray.size(); ++i )
	{
		KrImNode* parent = hitArray[i]->Parent();
		while( parent )
		{
			if ( parent->ToWidget() )
			{
				hit = parent->ToWidget();
				hit->ScreenToObject( event.motion.x, event.motion.y, &object, window );
				break;
			}
			parent = parent->Parent();
		}
	}

	// 1) Something has the focus. Nothing had it before.
	// 2) Something has the focus, something else had it before.
	// 3) Something loses the focus.
	// 5) The thing with focus gets a move.
	if ( hit && !mouseFocus )
	{
		mouseFocus = hit;
		mouseFocus->MouseIn( mouseDown, true );
		mouseFocus->MouseMove( mouseDown, object.x.ToIntRound(), object.y.ToIntRound() );
	}
	else if ( hit && mouseFocus && mouseFocus != hit )
	{
		mouseFocus->MouseIn( mouseDown, false );
		mouseFocus = hit;
		mouseFocus->MouseIn( mouseDown, true );
		mouseFocus->MouseMove( mouseDown, object.x.ToIntRound(), object.y.ToIntRound() );
	}
	else if ( !hit && mouseFocus )
	{
		mouseFocus->MouseIn( mouseDown, false );
		mouseFocus = hit;
	}
	else if ( hit && hit == mouseFocus )
	{
		GLASSERT( hit == mouseFocus );
		mouseFocus->MouseMove( mouseDown, object.x.ToIntRound(), object.y.ToIntRound() );
	}	
	else if ( !hit && !mouseFocus )
	{
		// nothing to do
	}
	else
	{
		GLASSERT( 0 );
	}					

	return (hit != 0);
}

bool KrEventManager::HandleEventMouseButton( const SDL_Event& event, KrEngine* engine )
{
		if ( event.button.button == SDL_BUTTON_LEFT )
		{
			bool down = event.button.state != 0;	// & SDL_BUTTON_LMASK;
			if ( down != mouseDown )
			{
				//GLOUTPUT("Mouse state changed to = %d\n", down);
				mouseDown = down;
				if ( mouseFocus )
				{
					int window = engine->GetWindowFromPoint( event.motion.x, event.motion.y );
					Vector2< GlFixed > object;
					mouseFocus->ScreenToObject( event.motion.x, event.motion.y, &object, window );

					return mouseFocus->MouseClick(	mouseDown ? KrWidget::LEFT_DOWN : KrWidget::LEFT_UP, 
													object.x.ToIntRound(), 
													object.y.ToIntRound() );
				}
			}
		}

	return false;
}
