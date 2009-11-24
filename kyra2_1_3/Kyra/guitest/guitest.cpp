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

#include "../engine/kyra.h"
#include "SDL.h"
#include "../spriteed/CONSOLEFONT.h"
#include "../guiExtended/KrImageListBox.h" 
#include <stdlib.h>

/*	This simple program demonstrates the use of all the default widgets.
	It should be build as part of your Kyra build.

	Start with "main()" below, reading through how it works. Then refer up to
	the class files that main uses as they occur.
*/


/*	This creates a subclass of the textwidget. If it receives an activation
	message, it will change the text to "activated", or "de-activated".
	When made the listener of a button, it will print "activated" or "de-activated"
	as the button is pressed or released with the mouse, or when the button
	hot key is pressed or released.
*/

/*
class MyTextWidget : public KrTextWidget
{
  public:
	MyTextWidget(	const char* _message,
					int width, int height, 
					bool drawBorder,
					bool drawBackground,
					bool editable,
					const KrScheme& scheme
				 )
		: KrTextWidget( width, height, drawBorder, drawBackground, editable, scheme ), message( _message )	{}

	
	virtual bool HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event )
	{
		SetTextChar( message );
		return true;
	}

	private:
		const char* message;
};
*/

/*	A console window. It listens to all the other widgets, and prints out the
	event messages that travel through the system.
*/

class MyConsole : public KrConsole
{
  public:
  	MyConsole(	int width, int height,
				int lineSpacing,
				const KrScheme& scheme ) : KrConsole( width, height, lineSpacing, scheme )
	{}


	virtual bool HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event ) 
	{
		char buf[ 256 ];
		//const char* eventDesc[] = { "NONE", "ACTIVATED", "DEACTIVATED", "COMMAND", "SELECTION" };

		switch ( event.type )
		{
			case KrWidgetEvent::ACTIVATED:
				sprintf( buf, "ACTIVATED %s source=0x%x\n", source->WidgetType(), (unsigned)source );
				break;

			case KrWidgetEvent::DEACTIVATED:
				sprintf( buf, "DEACTIVATED %s source=0x%x\n", source->WidgetType(), (unsigned)source );
				break;

			case KrWidgetEvent::COMMAND:
				sprintf( buf, "COMMAND %s source=0x%x command=%s arg=%s\n", source->WidgetType(), (unsigned)source, event.command.command, event.command.arg );
				break;

			case KrWidgetEvent::SELECTION:
				sprintf( buf, "SELECTION %s source=0x%x id=%d text=%s\n", source->WidgetType(), (unsigned)source, event.selection.index, event.selection.text ? event.selection.text : "(null)" );
				break;
				
			default:
				sprintf( buf, "ERROR" );
				break;
		}	
		PushText( buf );
		return true;	
	}
};


#define SDL_TIMER_EVENT ( SDL_USEREVENT + 0 )
const int TIMER_INTERVAL = 60;

/*	A timer callback, used to test widgets on a timer.
*/
Uint32 TimerCallback(Uint32 interval)
{
	SDL_Event event;
	event.type = SDL_TIMER_EVENT;

	SDL_PeepEvents( &event, 1, SDL_ADDEVENT, 0 );
	return TIMER_INTERVAL;
}


class GUI : public IKrWidgetListener
{
  private:
	KrTextWidget *text0, *text1;
	KrPushButton *push0, *push1;
	KrListBox *listBox0, *listBox1;
	KrProgress* progressBar;

  public:
	GUI( KrEngine* engine, KrFontResource* consoleFontRes )
	{
		/*	In order to test the icon / decal functionality of the buttons,
			grab some sprites from the test directory. In order to not
			go crazy with compile & make ordering, make the icons optional.
		*/
		bool useIcons = false;
		if ( engine->Vault()->LoadDatFile( "../tests/space.dat" ) )
		{
			useIcons = true;
		}
			
		/*	Generally, you'll want all your widgets to share a color scheme.
			This uses the default. However, you can reach in and set a primary and
			matching secondary color to customize the look of the widgets.
		*/
		KrScheme scheme( consoleFontRes );		

		/*	--- Toggle Buttons ---

			2 toggle buttons in the lower left of the screen. Create them,
			with size and scheme, add to the Tree() and you're ready to go!
			Like all KrImage's, they are positioned with SetPos().

			The top button (0) responds to a control-2, and the bottom
			button (1) responds to control-3.

			Toggle buttons automatically group with their siblings on the Tree.
		*/
		KrToggleButton* toggle0 = new KrToggleButton( 30, 30, scheme );
		engine->Tree()->AddNode( 0, toggle0 );
		toggle0->SetPos( 10, 150 );

		KrToggleButton* toggle1 = new KrToggleButton( 30, 30, scheme );
		engine->Tree()->AddNode( 0, toggle1 );
		toggle1->SetPos( 10, 190 );

		toggle0->SetAccelerator( KMOD_CTRL, SDLK_2 );
		toggle1->SetAccelerator( KMOD_CTRL, SDLK_3 );

		KrImNode* group = new KrImNode();
		engine->Tree()->AddNode( 0, group );

		KrToggleButton *toggle2 = new KrToggleButton( 30, 30, scheme );
		engine->Tree()->AddNode( group, toggle2 );
		toggle2->SetPos( 60, 150 );
		
		KrToggleButton *toggle3 = new KrToggleButton( 30, 30, scheme );
		engine->Tree()->AddNode( group, toggle3 );
		toggle3->SetPos( 60, 190 );
				
		/* --- Left List Box ---

		This list box is underfilled. Each line you added is added to the bottom
		of the box. An index is returned that identifies it in the future. A SELECTION
		event will return the index, allow you to know what the user selected.

		*/
		listBox0 = new KrListBox( 150, 200, scheme, false );
		listBox0->SetPos( 330, 10 );
		engine->Tree()->AddNode( 0, listBox0 );
		listBox0->AddTextChar( "item 0" );
		listBox0->AddTextChar( "item 1" );
		listBox0->AddTextChar( "item 2" );
		listBox0->AddTextChar( "item 3" );

		/* --- Right List Box --

		This list box is overfilled. Which means you need to be able to scroll it
		to see all the contents. 
		
		How to do this?

		The push buttons, below, will scroll the selection up or down when they are
		pressed.
		*/

		listBox1 = new KrListBox( 150, 200, scheme, true );
		listBox1->SetPos( 500, 10 );
		engine->Tree()->AddNode( 0, listBox1 );
		for( int i=0; i<20; ++i )
		{
			char buf[ 256 ];
			sprintf( buf, "item %d", i );
			listBox1->AddTextChar( buf );
		}

		/* --- Push Buttons ---

		These push buttons are subclasses of the default push button. When pressed,
		they will scroll the list box up or down.

		The buttons will respond to control-0 and control-1.

		See the ScrollButtonConnector comments for how this system works.
		*/
		push0 = new KrPushButton( 50, 50, scheme );
		engine->Tree()->AddNode( 0, push0 );
		push0->SetPos( 10, 10 );

		push1 = new KrPushButton( 50, 50, scheme );
		push1->SetPos( 10, 70 );
		engine->Tree()->AddNode( 0, push1 );

		push0->SetAccelerator( KMOD_CTRL, SDLK_0 );
		push1->SetAccelerator( KMOD_CTRL, SDLK_1 );

		push0->widgetPublish.AddListener( this );
		push1->widgetPublish.AddListener( this );

		/* --- Text Widgets ---
		*/
		text0 = new KrTextWidget( 200, 50, true, true, true, scheme );
		text0->SetPos( 100, 10 );
		engine->Tree()->AddNode( 0, text0 );
		
		text1 = new KrTextWidget( 200, 50, true, true, true, scheme );
		text1->SetPos( 100, 70 );
		engine->Tree()->AddNode( 0, text1 );

		/* --- Push buttons with icons ---

		Here to test the icon functionality, with oddly sized buttons. The Medium ship
		resource is a completely arbritrary choice.

		*/

		KrPushButton* iconButton0 = 0;
		KrPushButton* iconButton1 = 0;
		KrPushButton* graphicButton = 0;

		if ( useIcons )
		{
			KrSprite* sprite = 0;
			KrSpriteResource* logoRes = engine->Vault()->GetSpriteResource( "LOGO" );
			GLASSERT( logoRes );
	
			// Button 0
			iconButton0 = new KrPushButton( 70, 50, scheme );
			sprite = new KrSprite( logoRes );

			iconButton0->SetIcon( sprite );
			engine->Tree()->AddNode( 0, iconButton0 );
			iconButton0->SetPos( 10, 240 );
			iconButton0->SetTextChar( "Button" );

			// Button 1
			iconButton1 = new KrPushButton( 30, 50, scheme );
			sprite = new KrSprite( logoRes );

			engine->Tree()->AddNode( 0, iconButton1 );
			iconButton1->SetIcon( sprite );
			iconButton1->SetPos( 90, 240 );

			// A user drawn button.
			KrSpriteResource* buttonRes = engine->Vault()->GetSpriteResource( "BUTTON" );
			KrSprite* buttonSprite = new KrSprite( buttonRes );

			graphicButton = new KrPushButton( buttonSprite, scheme );
			engine->Tree()->AddNode( 0, graphicButton );
			graphicButton->SetPos( 160, 240 );

			/*
			KrImageListBox* imageBox = new KrImageListBox( 90, 200, 60, scheme );
			imageBox->SetPos( 670, 10 );
			engine->Tree()->AddNode( 0, imageBox );		
			for( int i=0; i<5; ++i )
				imageBox->AddImage( new KrSprite( logoRes ), "test" );
			*/
		}

		/* --- Progress Bar --- */
		progressBar = new KrProgress( 200, 50, scheme );
		progressBar->SetPos( 10, 450 );
		progressBar->SetMaxValue( 122 );
		engine->Tree()->AddNode( 0, progressBar );

		/* --- Eavsdropping console ---

		This console window is set up as a listener of everyone, and will print 
		every event published to the system.
		*/
		MyConsole* console = new MyConsole( 600, 100, 1, scheme );
		console->SetPos( 10, 300 );
		engine->Tree()->AddNode( 0, console );

		push0->widgetPublish.AddListener( console );
		push1->widgetPublish.AddListener( console );
		text0->widgetPublish.AddListener( console );
		text1->widgetPublish.AddListener( console );
		toggle0->widgetPublish.AddListener( console );
		toggle1->widgetPublish.AddListener( console );
		listBox0->widgetPublish.AddListener( console );
		listBox1->widgetPublish.AddListener( console );
		if ( useIcons )
		{
			iconButton0->widgetPublish.AddListener( console );
			iconButton1->widgetPublish.AddListener( console );
			graphicButton->widgetPublish.AddListener( console );
		}

		console->Print( "1. Left toggle buttons group, Right toggle buttons group." );
		console->Print( "2. Push buttons scroll both list boxes. Left underfilled, right overfilled." );
		console->Print( "3. Tab key should focus change between text fields." );
	}

	bool HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event )
	{
		if ( source == push0 && event.type == KrWidgetEvent::ACTIVATED ) {
			listBox0->MoveUpOne();
			listBox1->MoveUpOne();
		}
		else if ( source == push1 && event.type == KrWidgetEvent::ACTIVATED ) {
			listBox0->MoveDownOne();
			listBox1->MoveDownOne();
		}
		return true;
	}

	void Tick()
	{
		unsigned int value = progressBar->GetValue();
		if ( value == progressBar->GetMaxValue() ) {
			value = 0;
		}
		else {
			value += 1;
		}
		progressBar->SetValue( value );
	}

};


/*	A Kyra main loop, that displays all the widgets on a test screen. Note that the
	setup code for Kyra and SDL is skimmed over -- check out tutorial1 for a
	full explanation there.
*/
int main( int argc, char *argv[] )
{
	const SDL_version* sdlVersion = SDL_Linked_Version();
	if ( sdlVersion->minor < 2 )
	{
		printf( "SDL version must be at least 1.2.0\n" );
		GLASSERT( 0 );
		exit( 254 );
	}

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) < 0 ) {
		GLOUTPUT(( "Couldn't initialize SDL: %s\n",SDL_GetError() ));
		exit(255);
	}

	SDL_WM_SetCaption( "Kyra GuiTest", 0 );

	int screenX = 800;
	int screenY = 600;

	SDL_Surface* screen = SDL_SetVideoMode( screenX, screenY, 32, SDL_SWSURFACE );
	SDL_EnableUNICODE( true );

	/*	The 'if (screen)' is a little strange. But some Kyra classes are created
		inside the 'if' case, and it's a way to make sure their destructors are
		called before "SDL_Quit()".
	*/
	if ( screen )
	{
		/*	The console font is compiled in. CONSOLEFONT.CPP contains a font bmp file
			in a big array. CreateFixedFontResource is a simple way to load in such
			a file. Fonts are so basic it's nice to not have to load them from a dat
			when developing and prototyping, although you certainly could.
		*/
		KrFontResource* consoleFontRes =  KrEncoder::CreateFixedFontResource( "CONSOLE", CONSOLEFONT_DAT, CONSOLEFONT_SIZE );
		KrEngine* engine = new KrEngine( screen );

		GUI* gui = new GUI( engine, consoleFontRes );

		/*	Now the main loop logic. Stay in an event loop, listening,
			and Draw() after every event. To make sure that the EventManager
			gets all the mouse and keyboard events, all events are sent. (You
			could be more selective, but it doesn't hurt to send everything.)

			The KrEventManager is a singleton. It doesn't need to be created.
		*/ 
		engine->Draw();

		SDL_Event event;
		SDL_SetTimer( TIMER_INTERVAL, TimerCallback );

		while ( SDL_WaitEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
			{
				SDL_SetTimer( 0, 0 );
				break;
			}

			if ( event.type == SDL_TIMER_EVENT )
			{
				// The progress bar is on a timer. Move it
				// every time pulse.
				gui->Tick();
			}

			KrEventManager::Instance()->HandleEvent( event, engine );
			engine->Draw();
		}	

		/*	Clean up! The only thing of interest here is how to destroy the
			event manager. Note that if you call KrEventManager::Instance()
			after the delete KrEventManager::Instance(), this will re-create
			the event manager, which would be (presumably) a memory leak.
		*/
		delete gui;
		delete engine;
		engine = 0;
		delete consoleFontRes;
		consoleFontRes = 0;
		delete KrEventManager::Instance();
	}
	SDL_Quit();

	return 0;
}

