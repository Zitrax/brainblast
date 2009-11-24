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


#ifndef KYRA_ENGINE_INCLUDED
#define KYRA_ENGINE_INCLUDED

#include "SDL.h"
#include "vault.h"
#include "imagetree.h"
#include "dirtyrectangle.h"
#include "kyrabuild.h"

const int KyraVersionMajor = 2;
const int KyraVersionMinor = 1;
const int KyraVersionBuild = 3;

/** @mainpage
	A Sprite Engine...from a Slightly Different Point of View

    Kyra is a simple, fully featured, industrial strength Sprite engine written in C++.
	It is built on top of SDL for cross platform compatibility.
	It is Open Source and provided under the GPL.

	The API that is documented here is intended to be accessible and
	easy to use. It is created with you the user in mind.

    Note, however, that the API documented here is the details of how the engine
    works. For the big picture, you should start with the HTML documentation,
    available online at:
	
		<A HREF="http://www.grinninglizard.com/kyra">www.grinninglizard.com/kyra</A>
	
	<b>Brief API overview:</b>

	The KrEngine is a class that aggregates important components
	as well as providing some API calls as well. Kyra applications
	begin by creating a engine. The KrEngine::Draw
	methods renders the current sprites to the screen and updates
	the screen. The Tree method returns a pointer to the KrTree, a
	data structure where all the objects (drawable and otherwise)
	are stored. The Vault method returns a pointer to the engine's
	dictionary, where resources live.

	The tutorial and tests files give an overview as to how these
	peices fit together.

	<b>Memory Management</b>

	Generally speaking, objects are handed over to kyra components
	by ownership: that is, the client program 'new's an object,
	and then it is handed to the kyra object which will later delete
	it at the appropriate time. (An obvious exception is the engine
	itself, which you must create and destroy.)

	For example, you would create a canvas resource
	by calling 'new KrCanvasResource' with the appropriate parameters.
	Rather than memory manage this yourself, you should add it to
	the Vault, which can be retrieved via the KrEngine::Vault()
	method. You should not, in the future, delete the canvas resource
	since it is now owned by the vault. Likewise, when you create
	canvases by calling 'new KrCanvas', these will be added to the
	Tree (accessed via KrEngine::Tree() ) and will be deleted by the
	tree when appopriate.

	<b>Names and Namespaces</b>

	Method and parameter are not fully standardized and have errors. 
	(Regrettably.) The desired convention is as follows:

	For methods that return pointers:

		- Create		Allocates memory. Returns something to you 
						that you should delete or delete [].
		- Get			Returns something that is owned by the object.
						Do not delete. 
		- To			A conversion method. Will return an address if
						the object can be downcast to "ToThing", null if
						not. This is a cast, so nothing is created by
						calling this method.

	Other methods:

		Generally no prefix is used for queries that return a simple value.

		- Calc			Implies more work is involved. A slower operation
						that a typical accessor, and should be used accordingly.
		- Query			Used when memory is passed in and it is written to.
						No memory is new'd or deleted by the callee.

	<b>Organization</b>

	Kyra uses the following directories:

		- spriteed		Sprite Editor files.
		- encoder		Encoder files.
		- tests			Test and demo files.
		- engine		The files for the main engine and library.
		- util			Utility and container classes.
		- gui			User interface widgets.
		- guiExtended	User interface widgets by external authors.

	The following class prefixes:

		- Kr			Kyra classes.
	
	Nampspaces:

		- Kyra itself doesn't use a namespace. (It is old code.) All Kyra
		  classes start with Kr.
		- grinliz namespace contains the grinning lizard utilities. General
		  programming utilities and math classes (both 3D and 2D).
		- TinyXml also doesn't use a namespace. TinyXml class start with TiXml.

*/


enum
{
	KrQualityNone,		// Use the parent quality, if available.
	KrQualityFast,		// Nearest neighbor algorithm.
	KrQualityLinear,	// Bi-linear interpolation
	KrQualityAdaptive,	// Bi-linear or oversample
};

/** The core of the Kyra system. Instantiate an Engine
	to start using the sprite engine.
*/
class KrEngine
{
  public:
	/** Creates an engine that renders to the entire screen.
		@param screen	The SDL surface to use. If the surface
						is an OpenGL surface, Kyra will use the
						OpenGL library to draw with.
	*/
	KrEngine( SDL_Surface* screen );
	
	/**	Sets up an engine that renders to a rectangle on
		the screen, instead of the entire screen.
		@param screen		The SDL surface to use. 
		@param bounds		The area the engine will draw to.
		@param extraFill	If this is not null, the screen not covered
							by the bounds will be filled with this color.
							This gives a background color to "extra" screen.
	*/
	KrEngine( SDL_Surface* screen, const grinliz::Rectangle2I& bounds, const KrRGBA* extraFill );

	/** Sets up an engine that renders to multiple "windows"
		on the same surface. No window may overlap another.
		Each window is oriented with 0,0 as its upper left hand
		coordinate.

		@param screen		The SDL surface to use. 
		@param nWindows		The number of Kyra "windows" (viewports) to render to.
		@param bounds		An array of rectangles that define the windows.
		@param extraFill	If this is not null, the screen not covered
							by the bounds will be filled with this color.
							This gives a background color to "extra" screen.
	*/
	KrEngine( SDL_Surface* screen, int nWindows, const grinliz::Rectangle2I* bounds, const KrRGBA* extraFill );
	
	~KrEngine();

	/** Fetch the ImageTree. Even though it lives in the
		Engine, the Tree will be managed by the user.

		The KrImageTree returned is owned by the engine. It should not be deleted
		by the client code.
	*/
	KrImageTree*		Tree()			{ return tree; }

	/** Fetch a vault. Every engine needs a vault and there
		is nothing special about this one, except that it 
		is managed with the engine and will be created and 
		deleted at the correct time.

		The KrResourceVault is owned by the engine. It should not be deleted
		by the client code.
	*/
	KrResourceVault*	Vault()			{ return vault; }

	/// How many windows does this engine have?
	int NumWindows()					{	GLASSERT( nWindows <= KR_MAX_WINDOWS );
											return nWindows; }

	/// Given a point (in screen coordinates), what window is it in? -1 if none.
	int GetWindowFromPoint( int x, int y );
	
	/**	Once everything is ready to go, sprites and such
		are updated, call this method to put it all on
		the screen.

		@param updateRect	If true, SDL_UpdateRects will automatically
							be called in the Draw() method. Normally,
							this is the desired behavior. Only set to
							false if you will call SDL_UpdateRects,
							SDL_Flip, or something equivalent after
							you post process the buffer.
		@param rectangles	If wish the update rectangles to be returned
							to the calling application, pass in a
							DynArray.
	*/
	void Draw( bool updateRect = true, std::vector< grinliz::Rectangle2I >* rectangles = 0 );
	
	/** Return the bounds we are drawing to. (Not necessarily 
		the entire screen.) Set by the constructor.
	*/
	const grinliz::Rectangle2I& ScreenBounds( int window=0 )	{ return screenBounds[window]; }

	/** Retrun the bounds of the entire screen surface.
	*/
	const grinliz::Rectangle2I& FullScreenBounds()	{ return windowBounds; }

	/** Pass in a rectangle that should be invalidated. Only used 
		for unusual circumstances - primarily to clear a user-draw.
	*/
	void InvalidateRectangle( const grinliz::Rectangle2I& rect, int window=0 )	
		{ dirtyRectangle[window].AddRectangle( rect ); }


	/// Query the surface pointer used by this engine.
	SDL_Surface*	Surface()	{ return screen; }

	/// Invalidate everything and mark for repaint. Follow with call to Draw()
	void InvalidateScreen()	{ Tree()->Root()->Invalidate( KR_ALL_WINDOWS ); }

	/// Get a text description of the current rendering mode.
	void QueryRenderDesc( std::string* desc );

	static void QueryRenderDesc( SDL_Surface* surface, std::string* desc );

	// ------------- Engine Options ----------- //
	/**	Will fill the background with the specified color before
		drawing Images. If the Images let no background screen show
		through, NULL should be passed in to turn this feature off
		for a performance improvement.
	*/
	void FillBackground( const KrRGBA* fillColor );

	/// Set the fill of a particular backrgound window.
	void FillBackgroundWindow( int window, const KrRGBA* fillColor );

	/// Get the major, minor, and patch version of the engine.
	void static Version( int* major, int* minor, int* patch )	
	{ 
		*major = KyraVersionMajor;
		*minor = KyraVersionMinor;
		*patch = KyraVersionBuild;
	}

	/**	Set the maximize size of an OpenGL texture. This is the max size of either
		the x or y axis, so this limits memory usage to effectively: 

			max texture memory used = size * size * 4

		Useful to contain memory usage of large sprites. Quality, of course, will
		suffer if the size is constrained. Set to 0 (the default) to limit the largest
		size to that allowed by the driver.

		Note: This is static. The effect is global.
	*/
	static void SetMaxOglTextureSize( int size )	{ maxOglTextureSize = size; }

	/// Query the current max texture size. (0 is no Kyra imposed limit.)
	static int MaxOglTextureSize()					{ return maxOglTextureSize; }

	//----------------------------------
	// For use by the engine components:
 	KrDirtyRectangle*	DirtyRectangle( int window )	{ return &dirtyRectangle[window]; }

	// Call SDL_Rect for each rectangle.
	void UpdateScreen( std::vector< grinliz::Rectangle2I >* rects );

	// Debugging: try to see if this is corrupted
	void Validate()	{   GLASSERT( nWindows >= 0 );
						GLASSERT( nWindows <= KR_MAX_WINDOWS );
						GLASSERT( windowBounds.IsValid() );
						//GLOUTPUT( "Engine Validated (nWindows=%d)\n", nWindows );
      				}

	//restarts or reinits the video rendering system
	void Restart( SDL_Surface* _screen,		// The SDL surface
				int _nWindows,				// number of windows
				const grinliz::Rectangle2I* bounds,		// rect for each window
				const KrRGBA* extra );		// The color for non-window areas, if specified.


  private:
	void Init(	SDL_Surface* _screen,		// The SDL surface
				int _nWindows,				// number of windows
				const grinliz::Rectangle2I* bounds,		// rect for each window
				const KrRGBA* extra);		// The color for non-window areas, if specified.

	void InitOpenGL();

	static int maxOglTextureSize;

	SDL_Surface* screen;
	int nWindows;

	std::vector< SDL_Rect > sdlRects;
	KrDirtyRectangle	dirtyRectangle[ KR_MAX_WINDOWS ];
	grinliz::Rectangle2I				screenBounds[ KR_MAX_WINDOWS ];
	grinliz::Rectangle2I				windowBounds;

	KrImageTree*		tree;
	KrResourceVault*	vault;

	KrPaintInfo			paintInfo;
	bool				fillBackground[ KR_MAX_WINDOWS ];
	KrRGBA				backgroundColor[ KR_MAX_WINDOWS ];	// The background color of each window
	KrRGBA				extraBackground;					// The background color outside of the windows
	bool				needFullScreenUpdate;

	U32					splashStart;
	KrResourceVault		*splashVault;
	KrSprite			*splash, *splashText;
};

#endif
