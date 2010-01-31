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

#ifndef KYRA_BOX_INCLUDED
#define KYRA_BOX_INCLUDED

#include "image.h"
#include "boxresource.h"


struct KrPaintInfo;
class  KrBoxResource;
union  KrRGBA;


/**	A Box is sse to create filled or unfilled rectangles, horizontal 
	lines, verticle lines, and crosshairs.
	(A line is a box with a 1 pixel width or height.)

	It can be constructed from a BoxResource, or even without one,
	depending on which constructor is used. The BoxResource can
	have multiple colors, but the box is ofter easier to manage without
	a resource.

	The size, color, and type can be changed after construction. (If
	a multi-color box resource is in use, all but the first color is
	lost if the size, color, or type is set after construction.)
*/

class KrBox : public KrImage
{
  public:

	/// Construct from a resource.
	KrBox( KrBoxResource* resource );
	KrBox( int width, int height, KrRGBA color, int boxtype );

	virtual ~KrBox();

	virtual KrBox*	  ToBox()		{ return this; }

	virtual void Draw(	KrPaintInfo* paintInfo, 
						const grinliz::Rectangle2I& clip,
						int window );

	int Width()			{ return resource->Width(); }	///< Width
	int Height()		{ return resource->Height(); }	///< Height

	
	/// Set the width and height of this box.
	void SetSize( int w, int h );
	/// Set the color of this box.
	void SetBoxColor( KrRGBA color );
	/// Set box type (OUTLINE, FILL, or CROSSHAIR)
	void SetType( int boxtype );

	virtual KrImNode* Clone();

	virtual bool HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int win );
	virtual void CalcTransform( int window );
	virtual void QueryBoundingBox( grinliz::Rectangle2I* boundingBox, int window = 0 );
	virtual KrResource* Resource()			{ return resource; }
	
  protected:

  private:
	KrBoxResource* resource;
	bool owned;					// Do we own the resource?
};


#endif
