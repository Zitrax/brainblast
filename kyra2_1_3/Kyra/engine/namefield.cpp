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

#include "namefield.h"
#include "../util/glstring.h"
#include "../../grinliz/glutil.h"
#include <vector>

using namespace grinliz;

GlNameField::GlNameField()
{
	calcComplete = false;
	numBucket = 0;
}


void GlNameField::Add( const std::string& sentance )
{
	GLASSERT( calcComplete == false );
	if ( calcComplete ) return;

	std::vector<std::string> split;
	GlString::Split( &split, sentance, ".", false );
	if ( split.empty() ) return;

	GLASSERT( split.size() < MAX_BUCKET );

	for ( U32 i=0; i<split.size(); i++ )
	{
		// Check for an existing entry:
		if ( !bucket[i].Find( split.at( i ) ) )
		{
			bucket[i].PushFront( split.at( i ) );
		}
	}
	numBucket = Max( numBucket, (int) split.size() );
}


void GlNameField::Calc()
{
	GLASSERT( calcComplete == false );
	if ( calcComplete ) return;

	// Calculate the bit width of each bucket. Values, shift,
	// everything comes from the width.

	int i;
	shift[0] = 0;
	for( i=0; i<numBucket; ++i )
	{
		int size = 2;							// how many names this bitWidth can hold
		int nameCount = bucket[i].Count() + 1;		// the number of names needed. The +1 is because 0 is not used.
		bitWidth[i] = 1;

		while( size < nameCount )
		{
			size *= 2;
			++bitWidth[i];
		}
		shift[ i+1 ] = shift[i] + bitWidth[i];
	}
	calcComplete = true;
}


bool GlNameField::Get( const std::string& sentance, U32* value ) const
{
	*value = 0;

	GLASSERT( calcComplete == true );
	if ( !calcComplete ) return false;

	int components[ MAX_BUCKET ] = { 0 };

	std::vector<std::string> split;
	GlString::Split( &split, sentance, ".", false );
	if ( split.empty() ) return false;
	GLASSERT( split.size() < MAX_BUCKET );

	unsigned i;
	for( i=0; i<split.size(); ++i )
	{
		GlSListIterator< std::string > it( bucket[i] );
		int id = 1;

		for( it.Begin(); !it.Done(); it.Next() )
		{
			if ( it.Current() == split.at( i ) )
			{
				break;
			}
			++id;
		}
		if ( it.Done() )
		{
			// Didn't find the entry!
			GLASSERT( 0 );
			return false;
		}
		components[i] = id;
		*value += ( id << shift[i] );
	}
	return true;
}


void GlNameField::WriteHeader( FILE* text, const char* prefix )
{
	for ( int i=0; i<numBucket; i++ )
	{
		U32 id = 1;
		fprintf( text, "// Section %d: \n", i );
		GlSListIterator<std::string> it( bucket[ i ] );

		for ( it.Begin(); !it.Done(); it.Next() )
		{
			fprintf( text, "const U32 " );

			std::string name;
			if ( prefix )
				name += prefix;	//fprintf( text, "%s", prefix );
			name += it.Current();

			fprintf( text, "%20s = (%d << %d);		// %d\n",
					 name.c_str(), id, shift[i], ( id << shift[i] ) );
			++id;
		}
		fprintf( text, "\n" );
	}
}



void KrCachedWrite::Write( const std::string& value )
{
	GLASSERT( !value.empty() );

	Data data;
	data.pos = SDL_RWtell( stream );
	data.value = value;

	nameField.Add( value );

	SDL_WriteLE32( stream, 0 );
	list.PushBack( data );
}


void KrCachedWrite::Flush()
{
	nameField.Calc();
	GlSListIterator< Data > it( list );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		Data data = it.Current();

		U32 id = 0;
        #ifdef DEBUG
        bool ret =
        #endif

        nameField.Get( data.value, &id );
		GLASSERT( ret == true );

		SDL_RWseek( stream, data.pos, SEEK_SET );
		SDL_WriteLE32( stream, id );
	}
}


void KrCachedWrite::WriteHeader( FILE* text, const char* prefix )
{
	nameField.WriteHeader( text, prefix );
}
