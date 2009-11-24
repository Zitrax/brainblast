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

#ifndef IO_MATH_INCLUDED
#define IO_MATH_INCLUDED

#include "../../grinliz/glutil.h"
#include "../util/glfixed.h"
#include "../../grinliz/glgeometry.h"

typedef grinliz::Rectangle2I KrRect;


/**	A matrix class, of sorts. Describes a spacial transformation
	in Kyra: x and y positon, as well as x and y scale. All numbers
	are stored in fixed notation.
*/
class KrMatrix2
{
  public:
	GlFixed x, y;
	GlFixed xScale, yScale;

	/// Transform this matrix.
	void Composite( const KrMatrix2& other )
	{
		// The if check helps performance a little bit. The ".v" usage
		// makes a surprising difference. The opitimizer is doing less
		// than I expected.
		if (	other.xScale.v == GlFixed_1 
			 && other.yScale.v == GlFixed_1 )
		{
			x.v = other.x.v + x.v;		// bypass the compiler opt
			y.v = other.y.v + y.v;
		}
		else
		{
			x = other.x + other.xScale * x;
			y = other.y + other.yScale * y;
			xScale = other.xScale * xScale;
			yScale = other.yScale * yScale;
   		}
	}

	void Set( GlFixed _x = 0, GlFixed _y = 0, GlFixed _xScale = 1, GlFixed _yScale = 1 )
	{
		x.v = _x.v;
		y.v = _y.v;
		xScale.v = _xScale.v;
		yScale.v = _yScale.v;
	}

	// return true if there is any scaling term:
	bool IsScaled()	const { return ( xScale.v != GlFixed_1 ) || ( yScale.v != GlFixed_1 ); }

	inline friend bool operator == (const KrMatrix2& a, const KrMatrix2& b)    { return (a.x.v == b.x.v && a.y.v == b.y.v && a.xScale.v == b.xScale.v && a.yScale.v == b.yScale.v ); }
	inline friend bool operator != (const KrMatrix2& a, const KrMatrix2& b)    { return (a.x.v != b.x.v || a.y.v != b.y.v || a.xScale.v != b.xScale.v || a.yScale.v != b.yScale.v ); }
};


// Scale then Translate using the give matrix.
inline void ScaleTranslate( grinliz::Rectangle2I* rect, const KrMatrix2& matrix )
{
	GlFixed xmin = rect->min.x;
	GlFixed ymin = rect->min.y;
	GlFixed xmax = rect->max.x;
	GlFixed ymax = rect->max.y;

	//Scale( matrix.xScale, matrix.yScale );
	xmin *= matrix.xScale;
	ymin *= matrix.yScale;
	xmax *= matrix.xScale;
	ymax *= matrix.yScale;

	rect->min.x = (xmin + matrix.x).ToInt();
	rect->max.x = (xmax + matrix.x).ToInt();
	rect->min.y = (ymin + matrix.y).ToInt();
	rect->max.y = (ymax + matrix.y).ToInt();	
}


#endif
