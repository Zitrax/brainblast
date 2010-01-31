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

#ifndef KYRA_CANVAS_INCLUDED
#define KYRA_CANVAS_INCLUDED

#include "image.h"
#include "canvasresource.h"


struct KrPaintInfo;
class  KrCanvasResource;
union  KrRGBA;


/**	A Canvas is a pixel area that a client program can draw to.
	A Canvas has the following advantages over just drawing to
	the framebuffer after the engine:

	- automates dirty rectangle handling
	- applies color and alpha transforms like all other Kyra objects
	- can have multiple instances
	- depth
*/

class KrCanvas : public KrImage
{
  public:

	/// Construct from a resource - if the resource changes ALL the instances change.
	KrCanvas( KrCanvasResource* resource );
	virtual ~KrCanvas();

	virtual void Draw(	KrPaintInfo* paintInfo, 
						const grinliz::Rectangle2I& clip,
						int window );

	int Width()			{ return resource->Width(); }	///< Width
	int Height()		{ return resource->Height(); }	///< Height

	/** Note that this method is a pass through to a Canvas Resource; 
		if you change the pixel data, all instances of the 
		Canvas Resource will change.
		@sa KrCanvasResource
	*/
	KrRGBA* Pixels()	{ return resource->Pixels(); }

	/** Note that this method is a pass through to a Canvas Resource; 
		if you change the Refresh data, all instances of the 
		Canvas Resource will change.
		@sa KrCanvasResource
	*/
	void Refresh()		{ resource->Refresh(); }

	virtual KrImNode* Clone();

	virtual void QueryBoundingBox( grinliz::Rectangle2I* boundingBox, int window = 0 );
	virtual bool HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int window );
	virtual KrCanvas* ToCanvas()	{ return this; }
	virtual void CalcTransform( int window );
	virtual KrResource* Resource()			{ return resource; }
	
  protected:

  private:
	KrCanvasResource* resource;
};


#endif
