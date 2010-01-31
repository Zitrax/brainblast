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

#ifndef KYRA_SUM_HEADER
#define KYRA_SUM_HEADER

// Grinliz utility includes:
#include "../../grinliz/gldebug.h"
#include "../../grinliz/gltypes.h"
#include "../../grinliz/glmemorypool.h"
#include "../../grinliz/glprime.h"
#include "../../grinliz/glpublisher.h"
#include "../../grinliz/glrandom.h"

// Kyra utility includes
#include "../util/glisomath.h"

// Engine includes
#include "action.h"
#include "box.h"
#include "boxresource.h"
#include "canvas.h"
#include "canvasresource.h"
#include "color.h"
#include "dirtyrectangle.h"
#include "encoder.h"
#include "engine.h"
#include "fontresource.h"
#include "image.h"
#include "imagetree.h"
#include "imnode.h"
#include "kyraresource.h"
#include "painter.h"
#include "pixelblock.h"
#include "rle.h"
#include "sdlutil.h"
#include "sprite.h"
#include "spriteresource.h"
#include "textbox.h"
#include "tile.h"
#include "tileresource.h"
#include "vault.h"
#include "kyrabuild.h"
#include "dataresource.h"

// Tinyxml includes
#include "../../tinyxml/tinyxml.h"
#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

// GUI
#include "../gui/widget.h"
#include "../gui/textwidget.h"
#include "../gui/button.h"
#include "../gui/console.h"
#include "../gui/listbox.h"
#include "../gui/eventmanager.h"

// GUI Extended
#include "../guiExtended/progress.h"

#include "SDL.h"

#endif


