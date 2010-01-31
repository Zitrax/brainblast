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

#include "../util/glstring.h"
#include "../../grinliz/glutil.h"
#include "parser.h"
#include "SDL.h"

using namespace grinliz;

void KrDom::ReadFrameAttributes( const TiXmlNode* node,
								 KrDom::Frame* frame )
{
	const TiXmlElement* element = node->ToElement();
	
	::memset( frame, 0, sizeof( Frame ) );

	if ( !element )
	{
		GLOUTPUT(( "WARNING not an element in ReadFrameAttributes\n" ));
		GLASSERT( 0 );
		return;
	}

	if ( !StrEqual( element->Value(), "Frame" ) )
	{
		GLOUTPUT(( "WARNING not an element of type 'Frame' in ReadFrameAttributes\n" ));
		GLASSERT( 0 );
		return;
	}

	element->Attribute( "x", &frame->x );
	element->Attribute( "y", &frame->y );
	element->Attribute( "width",  &frame->width );
	element->Attribute( "height", &frame->height );

	frame->hasDelta = false;
	frame->hasHotspot = false;
	if ( element->Attribute( "hotspotx" ) && element->Attribute( "hotspoty" ) )
	{
		element->Attribute( "hotspotx", &frame->hotspotX );
		element->Attribute( "hotspoty", &frame->hotspotY );
		frame->hasHotspot = true;
	}
	if ( element->Attribute( "hotspotx" ) && element->Attribute( "hotspoty" ) )
	{
		element->Attribute( "deltax", &frame->deltaX );
		element->Attribute( "deltay", &frame->deltaY );
		frame->hasDelta = true;
	}
	if ( element->Attribute( "isotile" ) )
	{
		element->Attribute( "isotile", &frame->isotile );
	}
}


void KrDom::ReadTileAttributes(	const TiXmlNode* node,
								KrDom::Tile* frame )
{
	const TiXmlElement* ele = node->ToElement();
	GLASSERT( ele );
	if ( !ele ) return;

	ele->Attribute( "x",    &frame->x );
	ele->Attribute( "y",    &frame->y );
	ele->Attribute( "size", &frame->size );
}


const TiXmlNode* KrDom::ReadFrame( const TiXmlNode* action, int frameNumber )
{
	int i;
	const TiXmlNode* frame;

	for ( i = 0, frame = action->FirstChild( "Frame" );
		  frame;
		  i++, frame = frame->NextSibling( "Frame" ) )
	{
		if ( i == frameNumber )
			return frame;
	}
	return 0;
}


int	KrDom::CountChildren( const TiXmlNode* node, const std::string& value )
{
	int i;
	const TiXmlNode* child;

	for ( i = 0, child = node->FirstChild( value.c_str() );
		  child;
		  i++, child = child->NextSibling( value.c_str() ) )
	{}

	return i;
}


