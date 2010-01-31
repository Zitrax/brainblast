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
#include "sdlutil.h"
#include "SDL.h"
#include "SDL_endian.h"


U8 ReadByte( SDL_RWops* stream )
{
	U8 byte;
	SDL_RWread( stream, &byte, 1, 1 );
	return byte;
}


void ReadString( SDL_RWops* stream, std::string* str )
{
	char c;
	*str = "";

	for( ;; )
	{
		c = ReadByte( stream );
		if ( c )
			(*str) += c;
		else 
			break;
	}
}


void WriteString( SDL_RWops* stream, const std::string& str )
{
	const char* buf = str.c_str();
	SDL_RWwrite( stream, buf, strlen( buf ) + 1, 1 );
}


void WriteByte(  SDL_RWops* stream, U8 byte )
{
	SDL_RWwrite( stream, &byte, 1, 1 );
}


