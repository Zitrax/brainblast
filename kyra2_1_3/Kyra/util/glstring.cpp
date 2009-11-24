/*
Copyright (c) 2000-2003 Lee Thomason (www.grinninglizard.com)

Grinning Lizard Utilities. Note that software that uses the 
utility package (including Lilith3D and Kyra) have more restrictive
licences which applies to code outside of the utility package.


This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "string.h"
#include "glstring.h"


bool StrEqual( const char* s1, const char* s2 )
{
	return ( s1 && s2 && strcmp( s1, s2 ) == 0 );
}

void GlString::AppendInt( std::string* s, int i )
{
	char buf[64];

	sprintf( buf, "%d", i );
	(*s) += buf;
}


bool GlString::IEqual( const std::string& s1, const std::string& s2 )
{	
	unsigned i;

	if ( s1.size() == s2.size() )
	{
		for( i=0; i<s1.size(); i++ )
		{
			if ( tolower( s1[i] ) != tolower( s2[i] ) )
				return false;
		}
		return true;
	}
	return false;
}


bool GlString::IsSpace( char p, const char* delimiter, bool useIsSpace )
{
	if ( useIsSpace && isspace( p ) )
		return true;

	const char* white;
	for( white = delimiter; white && *white; white++ )
	{
		if ( p == *white )
			return true;
	}
	return false;
}


const char* GlString::SkipWhiteSpace( const char* p,
									  const char* delimiter,
									  bool useIsSpace )
{
	for ( ;
		  p && *p;
		  p++
		 )
	{
		if ( !IsSpace( *p, delimiter, useIsSpace ) )
			break;
	}

	return p;
}


const char* GlString::ReadWord(	const char* p,
								std::string* word, 
								const char* delimeter,
								bool useIsSpace )
{
	while ( p
			&& *p
			&& !IsSpace( *p, delimeter, useIsSpace )
		  )
	{
		(*word) += *p;
		p++;
	}

	return p;
}

void GlString::Split(	std::vector<std::string>* output,
						const std::string& input, 
						const char* delimiter,
						bool useIsSpace )
{
	if ( output )
	{
		if ( input.empty() )
			return;

		//GlDynArray<std::string>* retArray = new GlDynArray<std::string>;
		output->resize(0);

//		if ( !retArray )
//			return 0;

		char* array = new char[ input.size()+1 ];
		memcpy( array, input.c_str(), input.size() );
		array[ input.size() ] = 0;

		const char* p = &array[0]; 
		
		// parse out the array, rememebering in Linux the strings are
		// NOT null-terminated.
		//
		while ( p && *p )	
		{
			p = SkipWhiteSpace( p, delimiter, useIsSpace );
			if ( p && *p )
			{
				int index = output->size();
				output->resize( index+1 );
				p = ReadWord( p, &((*output)[index] ),
							  delimiter, useIsSpace );
			}
		}
		delete [] array;
	}
}


void GlString::SetExtension( std::string* s, const char* extension )
{
	if ( !s->empty() )
	{
		unsigned pos = s->rfind( '.' );

		if ( pos != std::string::npos )
		{	
			s->resize( pos+1 );
		}
		else
		{
			*s += '.';
		}
		*s += extension;
	}
}


void GlString::RemoveWhiteSpace( std::string* s )
{
	std::string str;

	const char* p = s->c_str();

	for( ; p && *p; ++p )
	{
		if ( !isspace( *p ) )
			str += *p;
	}

	*s = str;
}
