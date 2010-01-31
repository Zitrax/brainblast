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

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "SDL.h"
#include "SDL_endian.h"
#include "encoder.h"
#include "dataresource.h"
#include "sdlutil.h"

const std::string KrTextDataResource::textName = "TextData";
const std::string KrBinaryDataResource::binaryName = "BinaryData";


KrTextDataResource::KrTextDataResource( U32 size, SDL_RWops* data )
{
	std::string name;
	ReadString( data, &name );
	U32 id = SDL_ReadLE32( data );
	SetNameAndId( name, id );
	
	ReadString( data, &text );	
}


KrTextDataResource::KrTextDataResource( const std::string& resourceName )
{
	SetNameAndId( resourceName, 0 );
}


bool KrTextDataResource::LoadTextFile( const char* fname )
{
	FILE* fp = fopen( fname, "r" );
	text = "";

	if ( fp )
	{
		char buf[ 1024 ];
		while( fgets( buf, 1024, fp ) )
		{
			text += buf;		
		}
		fclose( fp );
		return true;
	}
	return false;
}


void KrTextDataResource::Text( std::vector< std::string >* strings )
{
	std::string buffer;

	for( const char* p = text.c_str(); *p; ++p )
	{
		if ( *p == '\n' )
		{
			strings->push_back( buffer );
			buffer = "";
		}
		else
		{
			buffer += *p;
		}
	}
	if ( buffer.length() > 0 )
		strings->push_back( buffer );
}


void KrTextDataResource::Save( KrEncoder* encoder )
{
	encoder->StartTag( KYRATAG_TEXTDATA );

	WriteString( encoder->Stream(), ResourceName() );
	encoder->WriteCached( ResourceName() );
	
	WriteString( encoder->Stream(), text );

	encoder->EndTag();
}

/////////////////////////////////////////////////////////////////

KrBinaryDataResource::KrBinaryDataResource( U32 size, SDL_RWops* stream )
{
	std::string name;
	ReadString( stream, &name );
	U32 id = SDL_ReadLE32( stream );
	SetNameAndId( name, id );
	
	length = SDL_ReadLE32( stream );
	data = new U8[ length ];

	SDL_RWread( stream, data, length, 1 );
}


KrBinaryDataResource::KrBinaryDataResource( const std::string& resourceName )
{
	SetNameAndId( resourceName, 0 );
	length = 0;
	data = 0;
}


void KrBinaryDataResource::Save( KrEncoder* encoder )
{
	encoder->StartTag( KYRATAG_BINARYDATA );

	WriteString( encoder->Stream(), ResourceName() );
	encoder->WriteCached( ResourceName() );
	
	SDL_WriteLE32( encoder->Stream(), length );
	SDL_RWwrite( encoder->Stream(), data, length, 1 );

	encoder->EndTag();
}


bool KrBinaryDataResource::LoadFile( const char* fname )
{
	if ( !data )
	{
		SDL_RWops* stream = SDL_RWFromFile( fname, "rb" );
		if ( stream )
		{
			SDL_RWseek( stream, 0, SEEK_END );
			length = SDL_RWtell( stream );
			SDL_RWseek( stream, 0, SEEK_SET );

			data = new U8[ length ];
			SDL_RWread( stream, data, length, 1 );
			SDL_RWclose( stream );
			return true;
		}
	}
	return false;
}
