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


#ifndef IO_PARSER
#define IO_PARSER

#include <stdio.h>
#include <string>

#include "../util/gllist.h"

class  TiXmlNode;


class KrDom
{
  public:
	struct Frame
	{
		Frame()	: x( 0 ), y( 0 ), width( 0 ), height( 0 ), hotspotX( 0 ),
				  hotspotY( 0 ), deltaX( 0 ), deltaY( 0 ),
				  isotile( 0 ), hasHotspot( false ), hasDelta( false ) 	{}

		int x;
		int y;
		int width;
		int height;
		int hotspotX;
		int hotspotY;
		int deltaX;
		int deltaY;
		int isotile;

		bool hasHotspot;
		bool hasDelta;
	};

	struct Tile
	{
		Tile() : x( 0 ), y( 0 ), size( 0 )	{}

		int x;
		int y;
		int size;
	};

	static void ReadFrameAttributes( const TiXmlNode* node,
									 KrDom::Frame* frame );
	
	static void ReadTileAttributes(  const TiXmlNode* node,
									 KrDom::Tile* tile );

	static const TiXmlNode*	ReadFrame( const TiXmlNode* action, int frameNumber );

	static int			CountChildren( const TiXmlNode* node, const std::string& value );
};


#endif
