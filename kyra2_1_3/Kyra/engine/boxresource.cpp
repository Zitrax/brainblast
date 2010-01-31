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

#include "box.h"

#include "SDL.h"
#ifdef KYRA_SUPPORT_OPENGL
	#include "SDL_opengl.h"
#endif
#include "boxresource.h"
#include "painter.h"
#include "imagetree.h"


using namespace grinliz;

const std::string KrBoxResource::boxName = "Box";
int KrBoxResource::boxId = 0;

KrBoxResource::KrBoxResource(	const std::string& _name,
								int _width, 
								int _height, 
								const KrRGBA* _colorArray,
								int numColors,
								int _flags )
{
	++boxId;
	SetNameAndId( _name, boxId );
	width  = _width;
	height = _height;
	boxtype  = _flags;
	sourceAlpha = false;

	int i, j = 0;
	for( i=0; i < COLOR_ARRAY_SIZE; i++ )
	{
		colorArray[i] = _colorArray[j];
		if ( colorArray[i].c.alpha != KrRGBA::KR_OPAQUE )
		{
			sourceAlpha = true;
		}

		++j;
		if ( j == numColors )
		{
			j = 0;
		}
	}
}


void KrBoxResource::CalculateBounds(  const KrMatrix2& matrix,
									  Rectangle2I* bounds ) const
{
	int dx = 0, dy = 0;

	if ( boxtype == CROSSHAIR )
	{
		dx = Width() / 2;
		dy = Height() / 2;
	}

	bounds->min.x = -dx;
	bounds->max.x = -dx + Width() - 1;
	bounds->min.y = -dy;
	bounds->max.y = -dy + Height() - 1;

	// Scaling factor won't change the x and y:
	// just the size.
	ScaleTranslate( bounds, matrix );
}


void KrBoxResource::DrawOpenGL(	KrPaintInfo* paintInfo,
								const KrMatrix2& matrix,
								const KrColorTransform& cForm,
								const Rectangle2I& clipping )
{
	#ifdef KYRA_SUPPORT_OPENGL
	// TODO: It would be good to have color array support. This would need
	//		  to be implemented by a texture.
	Rectangle2I bounds;
	CalculateBounds( matrix, &bounds );

	KrRGBA color = colorArray[0];
	cForm.ApplyTransform( &color );

	glBindTexture( GL_TEXTURE_2D, 0 );
	glColor4f( color.Redf(), color.Greenf(), color.Bluef(), color.Alphaf() );

	if ( boxtype == OUTLINE )
	{
		glBegin( GL_LINE_LOOP );
		{
			glVertex3f( bounds.min.x, bounds.min.y, 0.0f );
			glVertex3f( bounds.min.x + bounds.Width(), bounds.min.y, 0.0f );
			glVertex3f( bounds.min.x + bounds.Width(), bounds.min.y + bounds.Height(), 0.0f );
			glVertex3f( bounds.min.x, bounds.min.y + bounds.Height(), 0.0f );
		}
		glEnd();
		GLASSERT( glGetError() == GL_NO_ERROR );
	}
	else if ( boxtype == CROSSHAIR )
	{
		glBegin( GL_LINES );
		{
			glVertex3f( bounds.min.x + bounds.Width() / 2, bounds.min.y, 0.0f );
			glVertex3f( bounds.min.x + bounds.Width() / 2, bounds.min.y + bounds.Height(), 0.0f );
			glVertex3f( bounds.min.x,                  bounds.min.y + bounds.Height() / 2, 0.0f );
			glVertex3f( bounds.min.x + bounds.Width(), bounds.min.y + bounds.Height() / 2, 0.0f );
		}
		glEnd();
	}
	else if ( boxtype == FILL )
	{
		glBegin( GL_QUADS );
		{
			glVertex3f( bounds.min.x, bounds.min.y, 0.0f );
			glVertex3f( bounds.min.x + bounds.Width(), bounds.min.y, 0.0f );
			glVertex3f( bounds.min.x + bounds.Width(), bounds.min.y + bounds.Height(), 0.0f );
			glVertex3f( bounds.min.x, bounds.min.y + bounds.Height(), 0.0f );
		}
		glEnd();
	}
	GLASSERT( glGetError() == GL_NO_ERROR );
	#endif
}


void KrBoxResource::Draw(	KrPaintInfo* paintInfo,
							const KrMatrix2& matrix,
							const KrColorTransform& cForm,
							const Rectangle2I& clipping )
{
	if ( paintInfo->OpenGL() )
	{
		#ifdef KYRA_SUPPORT_OPENGL
			DrawOpenGL( paintInfo, matrix, cForm, clipping );
		#else
			GLASSERT( 0 );	
		#endif
		return;
	}

	KrPaintFunc blitter = paintInfo->GetBlitter( sourceAlpha, cForm );

	Rectangle2I isect;
	Rectangle2I bounds;
	CalculateBounds( matrix, &bounds );

	isect = bounds;
	if ( isect.Intersect( clipping ) )
	{
		isect.DoIntersection( clipping );

		int j, xSource, run, remainingWidth, dx;

		int yOffset		= isect.min.y - bounds.min.y;
		int xOffset		= isect.min.x - bounds.min.x;
		int width		= isect.Width();
		int height		= isect.Height();
		
		// measured by the j value, not y
		int drawHLine0	= -1;
		int drawHLine1	= -1;

		// measured by the x value
		int drawVLine0	= -1;
		int drawVLine1	= -1;

		if ( boxtype == OUTLINE )
		{
			if ( isect.min.x == bounds.min.x )
				drawVLine0 = isect.min.x;
			if ( isect.max.x == bounds.max.x )
				drawVLine1 = isect.max.x;
			if ( isect.min.y == bounds.min.y )
				drawHLine0 = 0;
			if ( isect.max.y == bounds.max.y )
				drawHLine1 = height-1;
		}
		else if ( boxtype == CROSSHAIR )
		{
			Vector2I center;
			center.x = ( bounds.min.x + bounds.max.x ) / 2;
			center.y = ( bounds.min.y + bounds.max.y ) / 2;

			if ( isect.Contains( center ) )
			{
				drawVLine0 = center.x;
				drawHLine0 = center.y - isect.min.y;
			}
		}

		if ( width > 0 && height > 0 )
		{
			for( j=0; j<height; j++ )
			{
				U8* target =   (U8*) paintInfo->pixels 
									 + ( isect.min.y + j ) * paintInfo->pitch
									 + isect.min.x * paintInfo->bytesPerPixel;
				
				// First, the case where the entire line is drawn.
				//		FILL: draw all lines.
				//		always draw top and bottom lines.
				if (    ( boxtype == FILL )
					 || ( j == drawHLine0 )
					 || ( j == drawHLine1 ) )
				{

					// As we draw, remember the source buffer is
					// only COLOR_ARRAY_SIZE pixels wide.
					remainingWidth = width;
					dx = xOffset;

					while ( remainingWidth )
					{
						// Change the source offset if we are striped:
						//	 dx is measured from bounds.
						//	 j  is measured from isect.

						xSource = ( (   ( dx )
									  + ( j + yOffset )
									) & MASK );

						run = Min( COLOR_ARRAY_SIZE - xSource, remainingWidth );
						GLASSERT( run > 0 );

						blitter( paintInfo, target,
								 &colorArray[ xSource ],
								 run,
								 cForm );

						remainingWidth -= run;
						dx += run;
						target += run * paintInfo->bytesPerPixel;
					}

					target += paintInfo->pitch;
				}
				else
				{
					// The case where we just draw the left or right edge

					if ( drawVLine0 >= 0 )
					{
						// Change the source offset if we are striped:
						//	 dx is measured from bounds.
						//	 j  is measured from isect.

						xSource = ( ( drawVLine0 + isect.min.y + j ) & MASK );

						blitter( paintInfo, 
								 target + ( drawVLine0 - isect.min.x )*paintInfo->bytesPerPixel,
								 &colorArray[ xSource ],
								 1,
								 cForm );
					}

					if ( drawVLine1 >= 0 )
					{
						// Change the source offset if we are striped:
						//	 dx is measured from bounds.
						//	 j  is measured from isect.

						xSource = ( ( drawVLine1 + isect.min.y + j ) & MASK );

						blitter( paintInfo, 
								 target + ( drawVLine1 - isect.min.x )*paintInfo->bytesPerPixel,
								 &colorArray[ xSource ],
								 1,
								 cForm );
					}
				}
			}
		}		
	}
}


bool KrBoxResource::HitTestTransformed( int x, int y, int hitFlags )
{
	// If this is a filled box, or ALWAYS_INSIDE_BOX is set,
	// then it is a simple rectangle intersection, else we
	// check to see if we are on the edge.

	if ( ( boxtype == FILL ) || ( hitFlags & KrImageTree::ALWAYS_INSIDE_BOX ) )
	{
		if ( x >= 0 && x < width && y >=0 && y < height )
		{
			return true;
		}
	}
	else
	{
		if ( x == 0 || x == ( width - 1	) || y == 0 || y == ( height - 1 ) )
		{
			return true;
		}
	}
	return false;
}

