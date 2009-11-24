/*--License:
	Kyra Sprite Engine
	Copyright Lee Thomason (Grinning Lizard Software) 2001-2002
	www.grinninglizard.com/kyra
	www.sourceforge.net/projects/kyra

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	The full text of the license can be found in license.txt
*/

#ifndef STITCH_INCLUDED
#define STITCH_INCLUDED

#include "SDL.h"

#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#include <string>

#include "../engine/krmath.h"

class FrameManager
{
  public:
	FrameManager();
	~FrameManager();

	void Add( const char* filename, const char* action, int skip, int scale );
	int NumFrames()					{ return surface.Count(); }
	SDL_Surface* Original( int i )	{ return surface[i]; }
	SDL_Surface* Processed( int i )	{ return processed[i]; }
	const std::string& ActionName( int i ){ return actionName[i]; }

	void SetRect( int i, KrRect rect )	{	if ( i >= rectangle.Count() )
												rectangle.SetCount( i + 1 );
											rectangle[i] = rect;
										}
	KrRect GetRect( int i )				{ return rectangle[i]; }

  private:
	GlDynArray< SDL_Surface* > surface;
	GlDynArray< SDL_Surface* > processed;
	GlDynArray< std::string >  actionName;
	GlDynArray< KrRect >  rectangle;
};



#endif