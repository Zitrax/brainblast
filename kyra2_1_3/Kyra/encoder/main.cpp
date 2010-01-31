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



#include "../../tinyxml/tinyxml.h"
#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "SDL.h"
#include "SDL_image.h"
#include "../engine/kyra.h"
#include "../gui/console.h"
#include "../engine/encoder.h"
#include "../spriteed/CONSOLEFONT.h"
#include <string.h>
#include "../util/glstring.h"

void EncoderOutput( KrResourceVault* vault, const char* prefix );

const int SCREENX = 640;
const int SCREENY = 480;

const int fontDatSize = 11153;
extern unsigned char fontDat[];


void ProcessOneXMLFile( const char* inputXML,
						KrEncoder* encoder )
{
	TiXmlDocument doc( inputXML );
	doc.LoadFile();

	if ( doc.Error() )
		printf( "Error reading XML file: '%s' row=%d col=%d\n", doc.ErrorDesc(), doc.ErrorRow(), doc.ErrorCol() );
	else
		printf( "Processing XML file '%s'\n", inputXML );

	if ( !doc.Error() )
	{
		encoder->ProcessDoc( doc );
	}
	else
	{
		printf( "ERROR from XML: '%s'. XML file '%s'.\n", doc.ErrorDesc(), doc.Value() );
	}
}


int main( int argc, char* argv[] )
{
	int i = 0;

	const SDL_version* sdlVersion = SDL_Linked_Version();
	if ( sdlVersion->minor < 2 )
	{
		fprintf( stderr, "SDL version must be at least 1.2.0" );
		GLASSERT( 0 );
		exit( 254 );
	}

	if ( argc < 3 )
	{
		printf( "Usage: encoder [options] outputfile inputfile...\n" );
		printf( "  Options:  -pPREFIX append PREFIX before every name in the header.\n" );
		printf( "            -rFILE   read a list of xml input filenames from FILE.\n" );
		printf( "            -w       write bmp check files.\n" );
		exit( 0 );
	}

	int firstIndex = 1;
	const char* prefix = 0;
	bool writeBMP = false;
	const char* fromFile = 0;

	while( *argv[firstIndex] == '-' )
	{
		char* option = argv[firstIndex] + 1;
		if ( *( option ) == 'p' )
		{
			prefix = ( option+1 );
		}
		else if ( *( option ) == 'w' )
		{
			writeBMP = true;
		}
		else if ( *( option ) == 'r' )
		{
			fromFile = (option+1 );
		}
		firstIndex++;
	}

	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(255);
	}

	int major, minor, patch;
	KrEngine::Version( &major, &minor, &patch );
	printf( "Starting encoder version %d.%d.%d.\n", major, minor, patch );	

	// Create and set the extensions for the output files
	std::string datOutputName( argv[firstIndex] );
	std::string hOutputName( argv[firstIndex] );
	GlString::SetExtension( &datOutputName, "dat" );
	GlString::SetExtension( &hOutputName, "h" );

	SDL_RWops* stream = SDL_RWFromFile( datOutputName.c_str(), "wb" );
	if ( stream )
	{
		printf( "Write stream open. Beginning processing of output: %s\n", datOutputName.c_str() );
		KrEncoder encoder ( stream );

		if ( fromFile )
		{
			FILE* file = fopen( fromFile, "r" );
			if ( file )
			{
				char buffer[512];
				char* p;
				while( fgets( buffer, sizeof( buffer ), file ) )
				{
					p = strchr( buffer, '\n' );
					if ( p ) *p = 0;
					p = strchr( buffer, '\r' );
					if ( p ) *p = 0;

					if ( buffer[0] && buffer[0] != '#' )
					{
						printf( "Processing '%s'\n", buffer );
						ProcessOneXMLFile( buffer, &encoder );
					}
				}
				fclose( file );
			}	
			else
			{
				printf( "ERROR: could not open file list '%s'\n", fromFile );
			}
		}
		else
		{
			for ( i=firstIndex+1; i<argc; i++ )
			{
				ProcessOneXMLFile( argv[i], &encoder );
			}
		}
		printf( "Writing DAT file.\n" );
		encoder.StartDat();
		encoder.Save();
		encoder.EndDat();
		printf( "DAT complete. Writing header '%s'\n", hOutputName.c_str() );

		if ( writeBMP )
			EncoderOutput( encoder.GetVault(), argv[firstIndex] );

		FILE* fp = fopen( hOutputName.c_str(), "w" );
		if ( fp )
		{
			std::string headerName = hOutputName;
			unsigned i = headerName.find( '.' );
			if ( i != std::string::npos )
				headerName.resize( i );
			encoder.WriteHeader(	headerName.c_str(), fp,
									prefix );
			fclose( fp );
		}

		SDL_RWclose( stream );
	}
	SDL_Quit();
	return 0;
}

