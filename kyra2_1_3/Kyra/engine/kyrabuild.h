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


/** @file
	The build file contains compile time switches
	that change features or turn debugging on and
	off in the Kyra project.
*/

#ifndef KYRA_BUILD_INCLUDED
#define KYRA_BUILD_INCLUDED

// Shows the dirty rectangles.
//#define DRAWDEBUG_BLTRECTS

// Asserts if the engine draws a scaled image that isn't
// cached. Very useful if you expect everything (expect
// canvases) to be draw cached.
// #define ASSERT_IF_NOT_CACHED

const int KR_MAX_WINDOWS = 6;


#ifdef DEBUG
	// Turns on a higher (and slower) level of debugging.
	// 	#define VALIDATE_DEBUG
#endif

// Constants to control OpenGL.
// max # of images = kKrOglDistance
//const int kKrOglDistance = 50000;

#endif

