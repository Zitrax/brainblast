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

#ifndef KYRA_BOXRESOURCE_INCLUDED
#define KYRA_BOXRESOURCE_INCLUDED

#include "kyraresource.h"
#include "../engine/krmath.h"
#include "../util/glcirclelist.h"
#include "color.h"

class KrBox;
struct KrPaintInfo;


/** A Box is a box of color, a horizontal line, a vertical line,
	a crosshair, or a rectangle. Which seems	
	a bit odd, granted, but from a
	certain point of view they are all the same thing.
*/
class KrBoxResource : public KrResource
{
  public:
	/** A box resource is NOT created by a dat file, but is
		created by the client program. Once a box resource is
		created (and normally placed in the vault) it is used
		like any other resource.

		@param width	The width of the box. A width of 1 is 
						a vertical line.
		@param height	The height of the box. A height of 1 is
						a horizontal line.
		@param colorArray	A box can be made of one or multiple colors.
							OpenGL mode does not support multiple colors.
							Only the first color is used in OpenGL rendering.
		@param numColors	The number of colors in the color array.
							To be drawn correctly for large objects, 
							this needs to be a power of 2 that is 64 or less.
		@param type  		OUTLINE, FILL, or CROSSHAIR
	*/ 
	KrBoxResource(	const std::string& name,
					int width, 
					int height, 
					const KrRGBA* colorArray,
					int numColors,
					int boxtype );

	virtual ~KrBoxResource()				{}

	enum
	{
		OUTLINE,
		FILL,
		CROSSHAIR
	};

	enum {
		COLOR_ARRAY_SIZE = 4,
		MASK = 3,
	};

	virtual	U32 Type()						{ return KYRATAG_BOX; }
	virtual const std::string&	TypeName()	{ return boxName; }
	virtual KrBoxResource* ToBoxResource()	{ return this; }

	int     Width()	const				{ return width; }	///< Width
	int		Height() const				{ return height; }	///< Height
	const KrRGBA* ColorArray() const	{ return colorArray; }
	int		BoxType() const				{ return boxtype; }

	/*  Draw a box resource.
		@param  paintInfo	Information about the target surface for drawing (optimizing).
		@param	x			X location in pixels.
		@param	y			Y location in pixels.
		@param  cForm		Color transformation applied to the drawing.
		@param	clip		A clipping rectangle, which can be null.
	*/
	void Draw( KrPaintInfo* paintInfo,
			   const KrMatrix2& matrix,
			   const KrColorTransform& cForm,
			   const grinliz::Rectangle2I& clipping );

	/*  Do a HitTest (see KrImageTree::HitTest) in transformed 
		coordinates. So the tree object that made this call
		has already transformed the x and y into local pixel coords.
	*/
	bool HitTestTransformed( int x, int y, int hitFlags );

	/*	Given a hotspot x and y, and scale factors,
		return the bounding box	for the box resource.
	*/
	void CalculateBounds( const KrMatrix2& matrix,
						  grinliz::Rectangle2I* bounds ) const;

  private:
	void DrawOpenGL(KrPaintInfo* paintInfo,
					const KrMatrix2& matrix,
					const KrColorTransform& cForm,
					const grinliz::Rectangle2I& clipping );

	static const std::string boxName;
	static int boxId;

	KrRGBA	colorArray[COLOR_ARRAY_SIZE];
	int		width;
	int		height;
	int		boxtype;
	bool	sourceAlpha;	// is there alpha in our colorArray?
};


#endif
