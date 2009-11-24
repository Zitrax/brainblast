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

#include "statemachine.h"
#include "SDL.h"
#include "SDL_image.h"
#include <math.h>
#include "../../grinliz/glutil.h"
#include "../util/glstring.h"
#include "../engine/parser.h"
#include "../engine/action.h"
#include "../engine/textbox.h"
#include "dom.h"

//Dave Leimbach [Leimy] for isdigit
#include <ctype.h>

using namespace grinliz;

extern Uint32 TimerCallback( Uint32 interval );


EdState::EdState( Editor* _machine )
{
	machine = _machine;
	shared  = _machine->SharedState();
}


struct KrCommand
{
	const char* name;
	const char* usage;
	const char* desc;
	unsigned	minArg;
	unsigned	maxArg;
	unsigned	index;
};

	
static const KrCommand editorCommand[] = {
	{	"load",		"load filename", "Filename is a .xml file that contains sprite information to be edited.",
		2, 2, COMMAND_LOAD },
	{	"image",	"image filename [trans]", "An image file will be loaded and a .xml file created. (Of the same name.)",
		2, 6, COMMAND_IMAGE },
	{	"save",		"save [filename]", "Save a .xml file. The optional filename specifies a different output name.",
		1, 2, COMMAND_SAVE },
	{	"sprite",	"sprite spriteName", "Creates and selects a sprite.",
		2, 2, COMMAND_NAME },
	{	"action",	"action actionName", "Creates and selects an action; a current sprite must already exist.",
		2, 2, COMMAND_ACTION },
	{	"tile",	"tile tilename", "Creates or selects a tile.",
		2, 2, COMMAND_TILE },
	{	"exit", "exit", "Exits the sprite editor.",
		1, 1, COMMAND_EXIT },
	{   "fps",  "fps frames", "Sets the frames per second for movie playback.",
		2, 2, COMMAND_FPS },
	{   "disperse", "disperse x y", "Sets the initial frame deltas",
		3, 3, COMMAND_DISPERSE },
//	{   "isotile", "isotile width", "Set the current action to create isometric tiles. Width must be a multiple of 4.",
//	    2, 2, COMMAND_ISOTILE },
	{   0, 0, 0, 0, 0 }
};


Editor::Editor( SDL_Surface* surface ) :
	mouseX( 0 ),
	mouseY( 0 ),
	mouseDown( false ),
	rightButtonDown( false ),
	currentState( 0 )
{
	shared = new SharedStateData( surface );

	state[ EdState::EMPTY ] = new EdStateEmpty( this );
	state[ EdState::VIEW ]  = new EdStateView( this );
	state[ EdState::MOVIE ] = new EdStateMovie( this );
	state[ EdState::ALIGN ] = new EdStateAlign( this );
	commandDown = false;
	commandClick = false;

	for ( int i=0; editorCommand[i].name; i++ )
	{
		shared->Console()->AddCommand( editorCommand[i].name );
	}
	//shared->Console()->CommandSignal.connect( this, &Editor::HandleCommand );
	shared->Console()->widgetPublish.AddListener( this );

	state[currentState]->StateOpening();
}


Editor::~Editor()
{
	state[currentState]->StateClosing();

	for ( int i=0; i<EdState::NUMSTATES; i++ )
	{
		delete state[ i ];
	}

	delete shared;
}


void Editor::ChangeState( int to )
{
	GLASSERT( to >=0 && to < EdState::NUMSTATES );
	GLASSERT( state[ currentState ] );
	GLASSERT( state[ to ] );

	state[ currentState ]->StateClosing();
	state[ to ]->StateOpening();

	currentState = to;
}


void Editor::MouseDown( int x, int y, int button, int modState )
{
	mouseX = x;
	mouseY = y;
	
	if ( button == 1 )
	{
		mouseDownX = x;
		mouseDownY = y;
		mouseDown = true;

		if ( commandDown )
		{
			commandClick = true;
			state[ currentState ]->Click( x, y, true, modState );
		}
		else
		{
			commandClick = false;
		}
	}
	else if ( button == 3  )
	{
		rightMouseDownX = x;
		rightMouseDownY = y;
		rightMouseLastX = x;
		rightMouseLastY = y;
		rightButtonDown = true;
	}
}


void Editor::MouseUp( int x, int y, int button, int keymod )
{
	if ( button == 1 )
	{
		mouseX = x;
		mouseY = y;
		mouseDown = false;

		if ( !commandClick )
		{
			if (	abs( x - mouseDownX ) <= MOUSE_CLICK
				 ||	abs( y - mouseDownY ) <= MOUSE_CLICK )
			{
				state[ currentState ]->Click( x, y, false, keymod );
			}
			else			
			{
				KrRect rect;
				rect.min.x = Min( mouseDownX, x );
				rect.min.y = Min( mouseDownY, y );
				rect.max.x = Max( mouseDownX, x );
				rect.max.y = Max( mouseDownY, y );

				state[ currentState ]->Drag( rect, true );
			}
		}

		commandClick = false;
	}
	else if ( button == 3 )
	{
		rightButtonDown = false;
		if (	abs( x - rightMouseDownX ) <= MOUSE_CLICK
			 &&	abs( y - rightMouseDownY ) <= MOUSE_CLICK )
		{
			state[ currentState ]->RightClick( x, y );
		}
	}
}


void Editor::MouseMove( int x, int y )
{
	mouseX = x;
	mouseY = y;

	if ( mouseDown )
	{
		if ( mouseDown && !commandClick )
		{
			if (	abs( x - mouseDownX ) > MOUSE_CLICK
				 ||	abs( y - mouseDownY ) > MOUSE_CLICK )
			{
				KrRect rect;
				rect.FromPair( x, y, mouseDownX, mouseDownY );
				state[ currentState ]->Drag( rect, false );
			}
		}
	}
	else if ( rightButtonDown )
	{
		state[ currentState ]->Pan( x - rightMouseLastX,
									y - rightMouseLastY  );
		rightMouseLastX = x;
		rightMouseLastY = y;
	}
	else
	{
	 	state[ currentState ]->FreeMove( x, y, commandDown );
	}
}


void Editor::Command( int index, CommandArgument* arg, int nArg )
{ 
	switch( index )
	{
		case COMMAND_EXIT:
			SDL_Event event;
			::memset( &event, 0, sizeof( event ) );
			event.type = SDL_QUIT;
			SDL_PeepEvents( &event, 1, SDL_ADDEVENT, 0 );
			break;

		case COMMAND_LOAD:
			if ( currentState == EdState::EMPTY )
				LoadDef( arg[0].CharString() ); 
			else
				shared->Console()->Print( "Only allowed in help mode.\n" );
			break;

		case COMMAND_IMAGE:
			if ( currentState == EdState::EMPTY )
			{
				nTrans = 0;
				for( int i=1; i<nArg; i++ )
				{
					trans[nTrans].FromString( arg[i].CharString() );
					++nTrans;
				}
				LoadImage( arg[0].CharString(), nTrans, trans );
			}
			else
			{
				shared->Console()->Print( "Only allowed in help mode.\n" );		
			}
			break;
		
		case COMMAND_SAVE:
			if ( nArg == 0 )
				shared->Widget()->Save( shared->DefFileName().c_str(),
										shared->Console(),
										shared->SurfaceFileName(),
										nTrans,
										trans );
			else
				shared->Widget()->Save( arg[0].String().c_str(),
										shared->Console(),
										shared->SurfaceFileName(),
										nTrans,
										trans );
			break;
				
		case COMMAND_FPS:
			{
				double msec = 1000.0 / double( arg[0].Integer() );
				SDL_SetTimer( (unsigned int) msec, TimerCallback );
				shared->Console()->Print( "FPS set to %d\n", arg[0].Integer() );
			}
			break;

		default:
			state[ currentState ]->Command( index, arg, nArg );
	}
}


void Editor::LoadDef( const char* xmlfile )
{
	shared->Clear();

	TiXmlDocument doc;

	doc.SetValue( xmlfile );
	bool success = doc.LoadFile();

	if ( !success )
	{
		shared->Console()->Print( "XML file load failed. Error=%s\n",
								 doc.ErrorDesc() );
		return;
	}

	if ( !doc.FirstChild( "Definition" ) )
	{
		shared->Console()->Print( "XML file does not have 'Definition' node.\n" );
		return;
	}

	TiXmlElement*	def;
	def = doc.FirstChildElement( "Definition" );	
	GLASSERT( def );

	if ( !def->Attribute( "filename" ) )
	{
		shared->Console()->Print( "Definition node does not have filename.\n" );
		return;
	}

	// Get the transparency info.
	int i;
	nTrans = 0;
	for ( i=0; i<SharedStateData::MAX_TRANS; i++ )
	{
		char attrib[64];
		sprintf( attrib, "Transparent%d", i );

		if ( def->Attribute( attrib ) )
		{
			const char* colorString = def->Attribute( attrib );
			trans[ nTrans ].FromString( colorString );
			nTrans++;
		}
	}

	std::string fname( def->Attribute( "filename" ) );
	shared->SetFilenames( xmlfile, fname );

	shared->LoadSurface( nTrans, trans );

	if ( !shared->CanvasResource() )
	{
		shared->Console()->Print( "Load failed. Could not load graphics file '%s'.\n", 
								 shared->SurfaceFileName().c_str() );
		shared->SetFilenames( "", "" );
		return;
	}

	// Set some defaults:
	TiXmlElement* sprite	= 0;
	TiXmlElement* action	= 0;
	TiXmlElement* frame		= 0;
	TiXmlElement* tile		= 0;

	EdWidgetSprite*	widgetSprite	= 0;
	EdWidgetAction*	widgetAction	= 0;
	EdWidgetFrame*	widgetFrame		= 0;
	EdWidgetTile*	widgetTile		= 0;

	const char*	name;

	// Read in the widget.
	for(	sprite = def->FirstChildElement( "Sprite" );
			sprite;
			sprite = sprite->NextSiblingElement( "Sprite" ) )
	{
		name = sprite->Attribute( "name" );
		if ( !name )
		{
			GLASSERT( 0 );	// bad xml...
			continue;
		}

		widgetSprite = new EdWidgetSprite(	shared->Engine()->Tree(),
											shared->WidgetNode(),
											shared->Widget(),
											std::string( name ) );
		GLASSERT( widgetSprite );
		shared->Widget()->children.PushBack( widgetSprite );

		for(	action = sprite->FirstChildElement( "Action" );
				action;
				action = action->NextSiblingElement( "Action" ) )
		{
			name = action->Attribute( "name" );
			if ( !name )
			{
				GLASSERT( 0 );	// bad xml...
				continue;
			}

			widgetAction = new EdWidgetAction(	shared->Engine()->Tree(),
												shared->WidgetNode(),
												widgetSprite,
												std::string( name ));
			GLASSERT( widgetAction );
			widgetSprite->children.PushBack( widgetAction );

			for(	frame = action->FirstChildElement( "Frame" );
					frame;
					frame = frame->NextSiblingElement( "Frame" ) )
			{
				KrDom::Frame frameData;
				KrDom::ReadFrameAttributes( frame,
											&frameData );

				widgetFrame = new EdWidgetFrame( shared->Engine()->Tree(),
												 shared->WidgetNode(),
												 widgetAction,
												 shared->FontResource(),
												 frameData );
				GLASSERT( widgetFrame );
				widgetAction->children.PushBack( widgetFrame );
			}
		}
	}

	// Read in the tiles.
	for(	tile = def->FirstChildElement( "Tile" );
			tile;
			tile = tile->NextSiblingElement( "Tile" ) )
	{
		name = tile->Attribute( "name" );
		if ( !name )
		{
			GLASSERT( 0 );	// bad xml...
			continue;
		}

		KrDom::Tile tileData;
		KrDom::ReadTileAttributes( tile, &tileData );

		widgetTile = new EdWidgetTile(	shared->Engine()->Tree(),
										shared->WidgetNode(),
										shared->Widget(),
										std::string( name ),
										tileData.x,
										tileData.y,
										tileData.size );
		GLASSERT( widgetTile );
		shared->Widget()->children.PushBack( widgetTile );
	}
	ChangeState( EdState::VIEW );
}


void Editor::LoadImage( const char* imagefile, int nTrans, KrRGBA* trans )
{
	shared->Clear();
	
	std::string name( imagefile );
	GlString::SetExtension( &name, "xml" );

	shared->SetFilenames( name, imagefile );

	if ( shared->LoadSurface( nTrans, trans ) )
	{
		shared->Console()->Print( "Image file '%s' loaded. Creating text file '%s'.\n", 
								 shared->SurfaceFileName().c_str(),
								 shared->DefFileName().c_str() );
		ChangeState( EdState::VIEW );
	}
	else
	{
		shared->Console()->Print( "Load failed. Could not load graphics file '%s'.\n", 
								shared->SurfaceFileName().c_str() );
	}
}


bool Editor::HandleWidgetEvent(	KrWidget* source, const KrWidgetEvent& event )
{
	if ( event.type != KrWidgetEvent::COMMAND )	// Is this a command executed event?
		return false;

	if ( event.command.recognized == false ) {
 		shared->Console()->Print( "Command not found.\n" );
		return false;
	}
		
	// See if the command is one the Editor handle's.
	std::string comstr = event.command.command;
	comstr += ' ';
	comstr += event.command.arg;
	GLOUTPUT(( "command str: '%s'\n", comstr.c_str() ));

	std::vector< std::string > comarray;
	GlString::Split( &comarray, comstr, "", true );

	// Find a match for this command.
	const KrCommand *command = 0;
	int i = 0;

	if ( comarray.size() > 0 )
	{
		for ( ; editorCommand[i].name; i++ )
		{
			if ( comarray[0] == editorCommand[i].name )
			{
				command = &editorCommand[i];
				break;
			}
		}
	}
	
	if ( !command || !command->name )
	{
 		shared->Console()->Print( "ERROR command not found in command array\n" );
		return false;
	}

	if ( comarray.size() < command->minArg || comarray.size() > command->maxArg )
	{
		shared->Console()->Print( "Incorrect number of parameters.\n" );
		shared->Console()->Print( "Usage: %s.\n", command->usage );
		shared->Console()->Print( "%s\n", command->desc );
		return false;
	}

	// Else we parse the arguments and send them to the state for processing.
	const int MAX_COMMARG = 16;
	CommandArgument commArg[MAX_COMMARG];
	int nCommArg = comarray.size() - 1;

	for ( i=0; i<nCommArg; i++ )
	{
		// Try to guess the type.
		const std::string& str = comarray[i+1];

		if ( isdigit( str[0] ) || str[0] == '-' )
		{
			commArg[i].SetInteger( atoi( str.c_str() ) );
		}

		else
		{
			commArg[i].SetString( str );
		}
	}

	Command( command->index, commArg, nCommArg );
	return true;
}
