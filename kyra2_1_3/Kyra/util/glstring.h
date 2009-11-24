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

#ifndef KYRA_STRING16_INCLUDED
#define KYRA_STRING16_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "../../grinliz/gltypes.h"
#include <string>
#include <vector>

bool StrEqual( const char* s1, const char* s2 );

class GlString
{
  public:

	static bool IsSpace( char p, const char* delimiter, bool useIsSpace );

	static const char* SkipWhiteSpace(	const char* p,
										const char* delimiter,
										bool useIsSpace );

	static bool IEqual( const std::string& s1, const std::string& s2 );
	static void AppendInt( std::string* s, int i );

	// Removes all white space in the given string.
	static void RemoveWhiteSpace( std::string* s );

	/* Creates an array of strings by splitting 'this' by
		the specified delimeters. 'this' will remain unchanged.
		The returned DynArray will need to be delete'd

		@param delimeter	An array of characters, any of which
							signals a split point.
		@param useIsSpace	Use the ctype 'isspace' call to determine
							if a character is a delimeter.
	*/
	static void Split(	std::vector<std::string>* output,
						const std::string& input, 
						const char* delimiter, 
						bool useIsSpace );

	/* Assuming 'this' is a filename or url, changes (or adds)
		the extension. The extension can be any number of letters,
		and should be passed in without the leading period.
	*/
	static void SetExtension( std::string*, const char* extension );

  private:
	static const char* ReadWord(	const char* p, 
									std::string* word, 
									const char* delimiter,
									bool useIsSpace );
};

#endif
