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

#ifndef KYRA_DATASOURCE_INCLUDED
#define KYRA_DATASOURCE_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "kyraresource.h"


/** A data resource stores user data in the dat file. Kyra does
	not use or interact with these resources. 

	When text is loaded into the resource, it will be stored
	as one long newline seperated null terminated string.
	(The newline termination is not effected by 
	encoding or reading OS).

	IMPORTANT: This is unrelated to the KrFontResource, which
	is used to display KrTextBox's. This is a collection of
	user defined text data.

	See also class KrBinaryDataResource.

	See the HTML docs for an example of how to encode user resources.
*/
class KrTextDataResource : public KrResource
{
  public:
	// Create by reading from a .dat file
	KrTextDataResource( U32 size, SDL_RWops* data );
	// Create from file
	KrTextDataResource( const std::string& resourceName );

	virtual ~KrTextDataResource()						{}

	virtual	U32 Type()									{ return KYRATAG_TEXTDATA; }
	virtual const std::string&	TypeName()				{ return textName; }
	virtual KrTextDataResource* ToTextDataResource()	{ return this; }

	/** Get the text as one long null terminated, newline
		separated char string.
	*/
	const char* Text()									{ return text.c_str(); }
	const std::string& TextString()						{ return text; }

	/** Get the text as a group of null terminated strings.
	*/
	void Text( std::vector< std::string >* strings );

	// [internal]
	virtual void Save( KrEncoder* );
	bool LoadTextFile( const char* fname );

  private:
	const static std::string textName;
	std::string text;
};


/** A data resource stores user data in the dat file. Kyra does
	not use or interact with these resources. 

	The binary data resource stores an arbitrary binary block 
	of data.

	See also class KrTextDataResource

	See the HTML docs for an example of how to encode user resources.
*/
class KrBinaryDataResource : public KrResource
{
  public:
	// Create by reading from a .dat file
	KrBinaryDataResource( U32 size, SDL_RWops* data );
	KrBinaryDataResource( const std::string& resourceName );
	virtual ~KrBinaryDataResource()						{ delete [] data; }

	virtual	U32 Type()									{ return KYRATAG_BINARYDATA; }
	virtual const std::string&	TypeName()				{ return binaryName; }
	virtual KrBinaryDataResource* ToBinaryDataResource()	{ return this; }

	/// Get a pointer to the binary data.
	const U8* Data()									{ return data; }
	/// Get the data length
	int Length()									{ return length; }

	// [internal]
	virtual void Save( KrEncoder* );
	bool LoadFile( const char* fname );

  private:
	const static std::string binaryName;
	U8* data;
	int length;
};

#endif
