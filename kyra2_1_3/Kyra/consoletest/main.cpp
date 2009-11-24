/*--License:
	Kyra Sprite Engine
	Copyright Lee Thomason (Grinning Lizard Software) 2001-2002
	www.grinninglizard.com/kyra
	www.sourceforge.net/projects/kyra

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	The full text of the license can be found in license.txt
*/

#include <stdlib.h>

#include "SDL.h"
#include "../../grinliz/gldebug.h"
#include "../engine/engine.h"
#include "../engine/textbox.h"
#include "../gui/console.h"

class Handler : public IKrCommand
{
  public:
	Handler( const char* _name )		{ name = _name; }

	virtual void Execute( const char* command, const char* args )
	{
		GLOUTPUT( "Handler %s called. command='%s' args='%s'\n",
				  name, command, args );
	}

	const char* name;
};


int main(int argc, char *argv[])
{
	SDL_Surface* screen;

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE ) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		GLASSERT( 0 );
		exit(255);
	}

	// Create an output surface, any bit depth. (Except 8)
	screen = SDL_SetVideoMode( 640, 480, 0, SDL_SWSURFACE);
	if ( screen == NULL ) {
		fprintf(stderr,"Couldn't set video mode: %s\n", SDL_GetError());
		GLASSERT( 0 );
		exit(3);
	}

	SDL_EnableUNICODE( true );
	SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	KrEngine* engine = new KrEngine( screen );
	bool ret = engine->Vault()->LoadDatFile( "../tests/standardtest.dat" );
	GLASSERT( ret == true );

	KrFontResource* fontRes = engine->Vault()->GetFontResource( "CONSOLE" );
	{
		// Get the proper scope for the console
		KrConsole console(	engine->Tree(), 
							engine->Tree()->Root(),
							fontRes,
							600, 400, 0 );

		Handler hHello( "hello" );
		Handler hWorld( "world" );
		Handler hDefault( "default" );

		console.AddCommandHandler( "hello", &hHello );
		console.AddCommandHandler( "world", &hWorld );
		console.SetDefaultHandler( &hDefault );

		engine->Draw();

		SDL_Event event;

		while(SDL_WaitEvent(&event))
		{
			if ( event.type == SDL_QUIT )
				break;

			if ( event.type == SDL_KEYDOWN )
				console.HandleKey( event );

			engine->Draw();
		}
	}
	delete engine;
	SDL_Quit();
	return 0;
}

