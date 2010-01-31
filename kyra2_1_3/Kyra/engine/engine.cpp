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

#include "canvas.h"
#include "engine.h"
#ifdef KYRA_SUPPORT_OPENGL
	#include "SDL_opengl.h"
	#include "ogltexture.h"
#endif
#include "canvasresource.h"
#include "../../grinliz/glgeometry.h"

#ifdef _MSC_VER
// Yes, we use 'this' in a member initialization list. Quote from
// Microsoft help: "This is a level-1 warning when Microsoft extensions 
// are enabled (/Ze) and a level-4 warning otherwise." I'm sure that
// made sense to someone. -lee
#pragma warning ( disable : 4355 )
#endif

using namespace grinliz;


// Check some SDL stuff here:
#if !( SDL_VERSION_ATLEAST(1, 2, 0) )
	#error Kyra requires SDL 1.2.0 or greater.
#endif


/*static*/ int KrEngine::maxOglTextureSize = 0;

KrEngine::KrEngine( SDL_Surface* _screen )
	: paintInfo( _screen )
{
	Rectangle2I bounds;
	bounds.Set( 0, 0, _screen->w - 1, _screen->h - 1 );

	Init( _screen, 1, &bounds, 0 );
}


KrEngine::KrEngine( SDL_Surface* _screen, const Rectangle2I& bounds, const KrRGBA* extra )
	: paintInfo( _screen )
{
	Init( _screen, 1, &bounds, extra );
}


KrEngine::KrEngine( SDL_Surface* _screen, int _nWindows, const Rectangle2I* bounds, const KrRGBA* extra )
	  : paintInfo( _screen )
{
	Init( _screen, _nWindows, bounds, extra );
}


KrEngine::~KrEngine()
{	
	// Note that the tree must be deleted before the vault.
	delete tree;		
	delete vault;
}

void KrEngine::Restart( SDL_Surface* _screen,		
						int _nWindows, 
						const Rectangle2I* bounds, 
						const KrRGBA* extra )
{
	GLASSERT(bounds);

	KrPaintInfo paintNew(_screen);
	memcpy(&paintInfo, &paintNew, sizeof(paintInfo));

	screen = _screen;
	nWindows = _nWindows;

	windowBounds.Set( 0, 0, screen->w-1, screen->h-1 );
	extraBackground.Set( 0, 0, 0, 255 );

	// If this assert is thrown, increase KR_MAX_WINDOWS to an
	// appropriate value and re-compile.
	GLASSERT( nWindows <= KR_MAX_WINDOWS );

	// fullScreenUpdate draws *outside* of the windows.
	needFullScreenUpdate = ( extra != 0 );

	if ( extra )
	{
		extraBackground = *extra;

		if ( !paintInfo.OpenGL() )
		{
			U32 color = SDL_MapRGB( screen->format, extra->c.red, extra->c.green, extra->c.blue );
			SDL_FillRect( screen, 0, color ); 
		}	
	}

	int i;
	for( i=0; i<nWindows; ++i )
	{
		// Default to filling the background to black.
		fillBackground[i] = true;
		backgroundColor[i].Set( 0, 0, 0, 255 );

		// Set the screenbounds to a window.
		screenBounds[i] = bounds[i];

		GLASSERT( bounds[i].min.x >= 0 );
		GLASSERT( bounds[i].min.y >= 0 );
		GLASSERT( bounds[i].max.x < screen->w );
		GLASSERT( bounds[i].max.y < screen->h );

		// Really aweful bugs result if the indivual windows
		// aren't clipped to the screen.
		screenBounds[i].DoClip( windowBounds );

		// Initialize the DR to repaint everything and clip to the screen
		// IMPORTANT: Set clip before adding any rectangles.
		dirtyRectangle[i].SetClipping( screenBounds[i] );
		dirtyRectangle[i].AddRectangle( screenBounds[i] );
	}

	// Check that none overlap.
	#ifdef DEBUG
		int j;
		for( i=0; i<nWindows; ++i )
		{
			for( j=i+1; j<nWindows; ++j )
			{
				GLASSERT( !bounds[i].Intersect( bounds[j] ) );
			}
		}
	#endif

	if ( paintInfo.openGL )
		InitOpenGL();

	SDL_EnableUNICODE( true );
}


void KrEngine::Init(	SDL_Surface* _screen,
						int _nWindows, 
						const Rectangle2I* bounds, 
						const KrRGBA* extra )
{
	splashStart = 0;
	splashVault = 0;
	splash = splashText = 0;

	Restart(_screen, _nWindows, bounds, extra);

	// Initialization of stuff that has "this" usage.
	vault = new KrResourceVault;
	tree = new KrImageTree( this );

	//start out with about 64 rects, shouldn't need much more than that for most instances
//	nNumSDLRects = 16;
//	sdlRects = new SDL_Rect[16];		
	sdlRects.resize( 16 );
//	memset( sdlRects.Memory(), 0, sizeof(SDL_Rect) * sdlRects.Count() );
}


void KrEngine::InitOpenGL()
{
	#ifdef KYRA_SUPPORT_OPENGL
	GLASSERT( KrTextureManager::TextureIndex() == 0 );

	int w = windowBounds.Width();
	int h =  windowBounds.Height();
	glViewport( 0, 0, w, h );
	glClearColor( extraBackground.Redf(),
				  extraBackground.Greenf(),
				  extraBackground.Bluef(),
				  255.0f );

	//glClearDepth(1.0);

	// The depth buffer isn't actually used. This only occured to me at the end:
	// Kyra sorts everything and draws in order...so you don't need the depth testing.
	// Oddly...if you turn this on, it creates a bug in 16bit mode. I'de really
	// like to understand that.  -- lee
	//	glDepthFunc(GL_LESS);
	//	glEnable(GL_DEPTH_TEST);	// disabled by default.

	glDisable( GL_DEPTH_TEST );		// Don't depth test
	glDepthMask( GL_FALSE );		// Don't write the depth buffer

	// Done: after reading the above comment...we don't need the openGLZ parameter.
	// Implemented ignoring the z-buffer.
	
	glShadeModel(GL_FLAT);		// Don't need smooth for 2D.

	// Create and reset the projection matrix
	glMatrixMode(GL_PROJECTION);
	
	// This matrix is set up to map pixels to screen the same way the bitbuffer does.
	// Trick from the OpenGL docs.

	glLoadIdentity();
	glOrtho( 0, w,
			 h, 0, 
			 -1.0f, 1.0f );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0.0);

	// A more reasonable mode to start in.
	glMatrixMode(GL_MODELVIEW);

	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClear( GL_COLOR_BUFFER_BIT );

	// Enable the texturing and the blending mode needed.
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	GLASSERT( glGetError() == GL_NO_ERROR );
	#endif
}


void KrEngine::FillBackground( const KrRGBA* fillColor )
{
	for( int i=0; i<nWindows; ++i )
	{
		FillBackgroundWindow( i, fillColor );
	}
}


void KrEngine::FillBackgroundWindow( int i, const KrRGBA* fillColor )
{
	if ( fillColor )
	{
		fillBackground[i]   = true;
		backgroundColor[i]	= *fillColor;
		//SDL_MapRGB( screen->format, fillColor->c.red, fillColor->c.green, fillColor->c.blue );
	}
	else
	{
		fillBackground[i] = false;
		backgroundColor[i].Set( 0, 0, 0 );
	}
	dirtyRectangle[i].AddRectangle( screenBounds[i] );
}


//void KrEngine::ClearScreen( int red, int green, int blue )
//{
//	U32 color = SDL_MapRGB( screen->format, red, green, blue );
//	SDL_FillRect( screen, 0, color ); 
//
//	SDL_UpdateRect( screen, 0, 0, 0, 0 );
//	Tree()->Root()->Invalidate( KR_ALL_WINDOWS );
//}



#if defined( DRAWDEBUG_RLE ) || defined( DRAWDEBUG_BLTRECTS )
	extern int debugFrameCounter;
#endif


void KrEngine::UpdateScreen( std::vector< Rectangle2I >* rectArray )
{
	if ( paintInfo.openGL )
	{
		SDL_GL_SwapBuffers();
	}
	else
	{
		if ( rectArray->size() == 0 )
			return;

//		if(	rectArray->Count() > sdlRects.AllocatedSize() )  
//		{
//			sdlRects.ResizePower2( rectArray->Count() );
//
//			//resize the array, to next multiple of 8
//			nNumSDLRects = rectArray->Count() + (8 - (rectArray->Count() % 8));
//			sdlRects = new SDL_Rect[nNumSDLRects];		
//			memset(sdlRects, 0, sizeof(SDL_Rect) * nNumSDLRects);
//		}

		sdlRects.resize( rectArray->size() );
		for( unsigned i=0; i<rectArray->size(); i++ )
		{
			sdlRects[i].x = rectArray->at( i ).min.x;
			sdlRects[i].y = rectArray->at( i ).min.y;
			sdlRects[i].w = rectArray->at( i ).Width();
			sdlRects[i].h = rectArray->at( i ).Height();
			
			GLASSERT( sdlRects[i].x >= 0 );
			GLASSERT( sdlRects[i].y >= 0 );
			GLASSERT( sdlRects[i].w > 0 && sdlRects[i].w <= screen->w );
			GLASSERT( sdlRects[i].h > 0 && sdlRects[i].h <= screen->h );
			GLASSERT( sdlRects[i].x + sdlRects[i].w <= screen->w );
			GLASSERT( sdlRects[i].y + sdlRects[i].h <= screen->h );
		}

//		GLOUTPUT( "Updating %d rects\n", rectArray->Count() );
		SDL_UpdateRects(screen, rectArray->size(), &sdlRects[0] );
	}
}


void KrEngine::Draw( bool updateRect, std::vector< Rectangle2I >* _rectangles )
{
	std::vector< Rectangle2I > rectArrayOnStack;

	#if defined( DRAWDEBUG_RLE ) || defined( DRAWDEBUG_BLTRECTS )
		debugFrameCounter++;
	#endif

//	GLOUTPUT( "Engine::Draw Walk\n" );
	tree->Walk();

		// We either use the passed in rectangles,
		// or the one here on the stack. Set the pointer
		// rectArray to the right thing.
	std::vector< Rectangle2I >* rectArray = ( _rectangles ) ? _rectangles : &rectArrayOnStack;
	rectArray->resize(0);
	
	if ( !paintInfo.openGL )
	{
		// Queue up the rectangles that will be used to blit to screen:

		if ( needFullScreenUpdate )
		{
			needFullScreenUpdate = false;			
			Rectangle2I rect;
			rect.Set( 0, 0, screen->w-1, screen->h-1 );
			rectArray->push_back( rect );
		}
		else
		{
			for ( int i=0; i<nWindows; ++i )
			{
				for ( int j=0; j<dirtyRectangle[i].NumRect(); ++j )
				{
					rectArray->push_back( dirtyRectangle[i].Rect(j) );
				}
			}
		}	
	}
	
	if ( paintInfo.openGL )
	{
		#ifdef KYRA_SUPPORT_OPENGL

		// OpenGL drawing
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for( int j=0; j<nWindows; ++j )
		{
			if ( fillBackground[j] )
			{
				glBindTexture( GL_TEXTURE_2D, 0 );
				glColor4f( backgroundColor[j].Redf(), backgroundColor[j].Greenf(), backgroundColor[j].Bluef(), 1.0f );
				glBegin( GL_QUADS );
				{
					glVertex3i( screenBounds[j].min.x, screenBounds[j].min.y, 0 );
					glVertex3i( screenBounds[j].min.x + screenBounds[j].Width(), screenBounds[j].min.y, 0 );
					glVertex3i( screenBounds[j].min.x + screenBounds[j].Width(), screenBounds[j].min.y + screenBounds[j].Height(), 0 );
					glVertex3i( screenBounds[j].min.x, screenBounds[j].min.y + screenBounds[j].Height(), 0 );
				}
				glEnd();
			}

			bool clipping = ( screenBounds[j] != windowBounds );

			if ( clipping )
			{
				glEnable(GL_CLIP_PLANE0);
				glEnable(GL_CLIP_PLANE1);
				glEnable(GL_CLIP_PLANE2);
				glEnable(GL_CLIP_PLANE3);

				double plane0[4] = { 1.0, 0.0, 0.0, -screenBounds[j].min.x };
				double plane1[4] = { -1.0, 0.0, 0.0, (screenBounds[j].min.x + screenBounds[j].Width() ) };
				double plane2[4] = { 0.0, 1.0, 0.0, -screenBounds[j].min.y };
				double plane3[4] = { 0.0, -1.0, 0.0, (screenBounds[j].min.y + screenBounds[j].Height() ) };

				glClipPlane( GL_CLIP_PLANE0, plane0 );
				glClipPlane( GL_CLIP_PLANE1, plane1 );
				glClipPlane( GL_CLIP_PLANE2, plane2 );
				glClipPlane( GL_CLIP_PLANE3, plane3 );
			}

			tree->DrawWalk( screenBounds[j], &paintInfo, j );

			if ( clipping )
			{
				glDisable(GL_CLIP_PLANE0);
				glDisable(GL_CLIP_PLANE1);
				glDisable(GL_CLIP_PLANE2);
				glDisable(GL_CLIP_PLANE3);
			}
			dirtyRectangle[j].Clear();
		}
		UpdateScreen( 0 );
		#else
			// No openGl support, but openGl surface used
			GLASSERT( 0 );
		#endif
	}
	else
	{
		// Bitmap drawing.

		// Draw the background, if necessary. Then
		// do a draw walk for every DR.
		for( int win=0; win<nWindows; ++win  )
		{
			for( int i=0; i<dirtyRectangle[win].NumRect(); ++i )
			{
				const Rectangle2I& rect = dirtyRectangle[win].Rect( i );

				// Draw the background.
				//GLASSERT( fillBackground[j] );
				if ( fillBackground[win] )
				{
					SDL_Rect sdlrect = { rect.min.x, rect.min.y, rect.Width(), rect.Height() };
					U32 sdlColor = SDL_MapRGB( screen->format,	backgroundColor[win].c.red,
																backgroundColor[win].c.green,
																backgroundColor[win].c.blue );
					//GLASSERT( sdlColor == 0 );
					SDL_FillRect( screen, &sdlrect, sdlColor );
				}
				tree->DrawWalk( rect, &paintInfo, win );

				/*
				#ifdef DRAWDEBUG_BLTRECTS
					KrPainter painter( &paintInfo );
					painter.DrawBox( rect.xmin, rect.ymin, rect.Width(), rect.Height(), 200, 0, 0 );
				#endif
				*/
			}

			#ifdef DRAWDEBUG_BLTRECTS
				dirtyRectangle[win].DrawRects( screen );
			#endif
			dirtyRectangle[win].Clear();
		}

		// The windows and DRs have been walked. Now transfer to physical screen.
		if ( updateRect )
		{
			// Use the composite list of rectangles.
			UpdateScreen( rectArray );
		}
	}
}


void KrEngine::QueryRenderDesc( std::string* desc )
{
	QueryRenderDesc( screen, desc );
}


/* static */ void KrEngine::QueryRenderDesc( SDL_Surface* screen, std::string* desc )
{
	char buf[ 256 ];
	sprintf( buf, "v%d.%d.%d %dbbp Fullscreen=%d %s ",
					KyraVersionMajor, KyraVersionMinor, KyraVersionBuild,
					screen->format->BitsPerPixel,
					( screen->flags & SDL_FULLSCREEN ) ? 1 : 0,
					#ifdef WIN32
						"Win32"
					#elif defined ( linux )
						"Linux"
					#else
						"UnknownPlatform"
					#endif
		);

	char render[256];
	#ifdef KYRA_SUPPORT_OPENGL
	if ( screen->flags & SDL_OPENGL )
	{
		const unsigned char* vendor   = glGetString( GL_VENDOR );
		const unsigned char* renderer = glGetString( GL_RENDERER );
		const unsigned char* version  = glGetString( GL_VERSION );
		sprintf( render, "OpenGL render: Vendor: '%s'  Renderer: '%s'  Version: '%s'",
					vendor, renderer, version );
	} else
	#endif
	{
		sprintf( render, "Software render" );
	}
	*desc = buf;
	desc->append( render );
}


int KrEngine::GetWindowFromPoint( int x, int y )
{
	for( int i=0; i<nWindows; ++i )
	{
		if ( screenBounds[i].Intersect( x, y ) )
			return i;
	}
	return -1;
}

/*
void KrEngine::StartSplash( U32 msec )
{
	splashStart = msec;
	GLASSERT( splashVault == 0 );

	splashVault = new KrResourceVault();
	splashVault->LoadDatFileFromMemory( splash_DAT, splash_SIZE );

	KrSpriteResource* splashRes     = splashVault->GetSpriteResource( "splash" );
	KrSpriteResource* splashTextRes = splashVault->GetSpriteResource( "splashText" );

	splash = new KrSprite( splashRes );
	splashText = new KrSprite( splashTextRes );

	Rectangle2I bounds, boundsText;
	splash->QueryBoundingBox( &bounds, 0 );
	splashText->QueryBoundingBox( &boundsText, 0 );

	tree->AddNode( 0, splash );
	tree->AddNode( 0, splashText );

	splash->SetPos( screenBounds[0].Width() / 2  - bounds.Width() / 2,
					screenBounds[0].Height() / 4 - bounds.Height() / 2 );
	splash->SetZDepth( 5000 );

	splashText->SetPos( screenBounds[0].Width() / 2  - boundsText.Width() / 2,
						splash->Y() + bounds.Height() + 20 );

	splashText->SetZDepth( 5000 );
}


bool KrEngine::UpdateSplash( U32 msec )
{
	U32 delta = msec - splashStart;

	KrColorTransform xcolor;

	if ( delta < 1000 )
	{
		xcolor.SetAlpha( 255 * delta / 1000 );
	}
	// TODO - I got a crash here becayse the noded had already been deleted at the end of a splash screen cycle. This problem should be fixed properly in the caller, so this is a temporary hack - comment by CRC.
	if (splash != 0)
	splash->SetColor( xcolor );
	if (splashText != 0)
	splashText->SetColor( xcolor );

	return ( delta >= 2000 );
}


void KrEngine::EndSplash()
{
  if (splash != 0)
	tree->DeleteNode( splash );
  if (splashText != 0)
	tree->DeleteNode( splashText );
	delete splashVault;

	splash = 0;
	splashText = 0;
	splashVault = 0;
}
*/

