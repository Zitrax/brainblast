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

#include "stitch.h"
#include "../engine/color.h"
#include "../engine/painter.h"
#include "../../tinyxml/tinyxml.h"
#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

struct Input
{
	Input()	{	filename = 0; action = 0; }

	const char* filename;
// 	const char* sprite;
	const char* action;
};


struct TGAHeader 
{
    U8 infolen;			/* length of info field */
    U8 has_cmap;		/* 1 if image has colormap, 0 otherwise */
    U8 type;

    U8 cmap_start[2];	/* index of first colormap entry */
    U8 cmap_len[2];		/* number of entries in colormap */
    U8 cmap_bits;		/* bits per colormap entry */

    U8 yorigin[2];		/* image origin (ignored here) */
    U8 xorigin[2];
    U8 width[2];		/* image size */
    U8 height[2];
    U8 pixel_bits;		/* bits/pixel */
    U8 flags;
};


void WriteTGA32( const char* filename, SDL_Surface* surface )
{
	FILE* fp = fopen( filename, "wb" );
	if ( fp )
	{
		TGAHeader tgaHeader;
		::memset( &tgaHeader, 0, sizeof( TGAHeader ) );
		tgaHeader.type = 2;		// RGB
		tgaHeader.width[0] = surface->w % 256;
		tgaHeader.width[1] = surface->w / 256;
		tgaHeader.height[0] = surface->h % 256;
		tgaHeader.height[1] = surface->h / 256;
		tgaHeader.pixel_bits = 32;

		fwrite( &tgaHeader, sizeof( TGAHeader ), 1, fp );
		
		KrPainter painter( surface );
		for( int j=surface->h-1; j >= 0; --j )
		{
			for( int i=0; i < surface->w; ++i )
			{
				KrRGBA color;
				painter.BreakPixel( i, j, &color );
				fputc( color.c.blue, fp );
				fputc( color.c.green, fp );
				fputc( color.c.red, fp );
				fputc( color.c.alpha, fp );
			}
		}
		fclose( fp );
	}
}


SDL_Surface* Write( FrameManager* frameManager, 
					const char* filenameBase, const char* spriteName )
{
	char filename[ 512 ];
	strcpy( filename, filenameBase );
	strcat( filename, ".tga" );

	char xmlfilename[ 512 ];
	strcpy( xmlfilename, filenameBase );
	strcat( xmlfilename, ".xml" );

	int i;
	TiXmlDocument doc( xmlfilename );
	TiXmlNode* node;

	TiXmlElement def( "Definition" );
	def.SetAttribute( "filename", filename );
	node = doc.InsertEndChild( def );

	TiXmlElement sprite( "Sprite" );
	sprite.SetAttribute( "name", spriteName );
	TiXmlNode* action = node->InsertEndChild( sprite );

	GlDynArray< KrRect > rects;
	rects.SetCount( frameManager->NumFrames() );

	// Get the clipping rect for each frame.
	for( i=0; i<frameManager->NumFrames(); ++i )
	{
		SDL_Surface* s = frameManager->Processed( i );
		KrPainter painter( s );

		KrRect rect;
		rect.Set( 0, 0, s->w-1, s->h-1 );

		while (    rect.xmin <= rect.xmax 
				&& painter.CalcTransparentColumn( rect.ymin, rect.ymax, rect.xmin ) == rect.Height() )
			++rect.xmin;
		while (    rect.xmax >= rect.xmin 
				&& painter.CalcTransparentColumn( rect.ymin, rect.ymax, rect.xmax ) == rect.Height() )
			--rect.xmax;
		while (    rect.ymin <= rect.ymax 
				&& painter.CalcTransparentRun( rect.xmin, rect.xmax, rect.ymin ) == rect.Width() )
			++rect.ymin;
		while (    rect.ymax >= rect.ymin 
				&& painter.CalcTransparentRun( rect.xmin, rect.xmax, rect.ymax ) == rect.Width() )
			--rect.ymax;

		GLASSERT( rect.IsValid() );
		rects[i] = rect;
	}

	// Now the screen size
	KrRect screensize;
	screensize.Set( 0, 0, 0, 0 );
	int x = 0;
	int y = 0;
	const int xtrigger = 600;
	const int pad = 10;

	for( i=0; i<frameManager->NumFrames(); ++i )
	{
		KrRect rect;
		rect.Set(	x, 
					y, 
					x + rects[i].Width() - 1, 
					y + rects[i].Height() - 1 );
		screensize.DoUnion( rect );
		GLASSERT( rects[i].IsValid() );
		frameManager->SetRect( i, rect );

		x += rects[i].Width() + pad;
		if ( x > xtrigger )
		{
			x = 0;
// 			y += screensize.ymax + pad;
			y = screensize.ymax + pad;
		}
	}
	screensize.xmax = (( screensize.xmax + 4 ) & ~0x03 ) - 1;
	screensize.ymax = (( screensize.ymax + 4 ) & ~0x03 ) - 1;

	// Create the surface
	SDL_Surface* s = SDL_CreateRGBSurface( SDL_SWSURFACE,
										   screensize.Width(), 
										   screensize.Height(),
										   32,
										   0x00ff0000,
										   0x0000ff00,
										   0x000000ff,
										   0xff000000 );
	GLASSERT( s );

// 	x = 0;
// 	y = 0;
// 	int ymax = 0;
	std::string actionName = "";

	for( i=0; i<frameManager->NumFrames(); ++i )
	{
		if ( actionName != frameManager->ActionName( i ) )
		{
			actionName = frameManager->ActionName( i );

			TiXmlElement actionElement( "Action" );
			actionElement.SetAttribute( "name", actionName );
			node = action->InsertEndChild( actionElement );
		}

		SDL_Rect source;
		SDL_Rect target;
		source.x = rects[i].xmin;
		source.y = rects[i].ymin;
		source.w = rects[i].Width();
		source.h = rects[i].Height();
// 		ymax = GlMax( ymax, y + rects[i].Height() - 1 );
// 
// 		GLASSERT( x >= 0 );
// 		GLASSERT( y >= 0 );
// 		int xtest = x + rects[i].Width() - 1;
// 		int ytest = y + rects[i].Height() - 1;  
// 		GLASSERT( xtest < s->w );
// 		GLASSERT( ytest < s->h );
		KrRect tr = frameManager->GetRect( i );
		GLASSERT( tr.xmin >= 0 );
		GLASSERT( tr.ymin >= 0 );
		GLASSERT( tr.xmax < s->w );
		GLASSERT( tr.ymax < s->h );

		target.x = tr.xmin;
		target.y = tr.ymin;
		target.w = tr.Width();
		target.h = tr.Height();

		// ACK! SDL is killing the alpha channel.
// 		SDL_BlitSurface( frameManager->Processed( i ), &source,
// 						 s, &target );
		for ( int jj=0; jj<rects[i].Height(); ++jj )
		{
			SDL_Surface* src = frameManager->Processed( i );

			memcpy( ((U8*)s->pixels)   + ( jj + target.y ) * s->pitch   + target.x * s->format->BytesPerPixel,
					((U8*)src->pixels) + ( jj + source.y ) * src->pitch + source.x * src->format->BytesPerPixel,
					source.w * src->format->BytesPerPixel );
		}

		TiXmlElement frame( "Frame" );
		frame.SetAttribute( "x", tr.xmin );
		frame.SetAttribute( "y", tr.ymin );
		frame.SetAttribute( "width", tr.Width() );
		frame.SetAttribute( "height", tr.Height() );
		frame.SetAttribute( "hotspotx", tr.xmin );
		frame.SetAttribute( "hotspoty", tr.ymin );
		frame.SetAttribute( "deltax", 0 );
		frame.SetAttribute( "deltay", 0 );
		node->InsertEndChild( frame );

// 		x += rects[i].Width() + pad;
// 		if ( x > xtrigger )
// 		{
// 			x = 0;
// 			y += ymax + pad;
// 		}
	}

	WriteTGA32( filename, s );
	doc.SaveFile();

	//SDL_FreeSurface( s );
	return s;
}


void Process( FrameManager* frameManager, 
			  int scale, 
			  bool applyAlpha,
			  KrRGBA alphaColor, 
			  int alphaValue )
{
	int count = 0;
	for( count = 0; count < frameManager->NumFrames(); ++count )
	{
		GLOUTPUT( "Processing frame %d...", count );

		SDL_Surface* s = frameManager->Original( count );
		SDL_Surface* t = frameManager->Processed( count );
		GLASSERT( s );
		GLASSERT( t );

		KrPainter sPainter( s );
		KrPainter tPainter( t );

		KrRGBA colorKey;
		sPainter.BreakPixel( 0, 0, &colorKey );
		int pixelGroup = scale * scale;

		KrRGBA tAlphaColor = alphaColor;
		tAlphaColor.c.alpha = alphaValue;

		int tx, ty, sx, sy;

		for( tx=0; tx<(s->w/scale); ++tx )
		{
			for( ty=0; ty<(s->h/scale); ++ty )
			{
				int nBackground = 0;
				int nAlpha = 0;
				int nColor = 0;
				int red = 0;
				int green = 0;
				int blue = 0;

				for( sx = tx*scale; sx < (tx+1) * scale; ++sx )
				{
					for( sy = ty*scale; sy < (ty+1) * scale; ++sy )
					{
						KrRGBA sColor;
						sPainter.BreakPixel( sx, sy, &sColor );
						if ( sColor == colorKey )
						{
							// Background
							++nBackground;
						}
						else if (    applyAlpha 
								  && sColor.c.red   == alphaColor.c.red
								  && sColor.c.green == alphaColor.c.green
								  && sColor.c.blue  == alphaColor.c.blue )
						{
							++nAlpha;
						}
						else
						{
							++nColor;
							red += sColor.c.red;
							green += sColor.c.green;
							blue += sColor.c.blue;
						}
					}
				}
				// Source analyzed. Figure out the target.
				GLASSERT( nColor + nAlpha + nBackground == pixelGroup );
				if ( nColor >= pixelGroup / 2 )
				{
					KrRGBA tColor;
					tColor.Set( red / nColor, green / nColor, blue / nColor, 255 );
					tPainter.SetPixel( tx, ty, tColor );
					
// 					KrRGBA c;
// 					tPainter.BreakPixel( tx, ty, &c );
				}
				else if ( nAlpha > pixelGroup / 2 )
				{
					tPainter.SetPixel( tx, ty, tAlphaColor );					
				}
			}

		}
		GLOUTPUT( "done.\n" );
	}
}




int main( int argc, char* argv[] )
{
	if ( argc < 4 )
	{
		printf( "Usage: stitch sprite -sc# -sk# -acRRGGBB -av128 filename action\n" );
		printf( "  filename: ex. walking0001.bmp\n" );
		printf( "  sprite, action: the name of the sprite and action to write to the XML\n" );
		printf( "  sc scale factor\n" );
		printf( "  sk skip\n" );
		printf( "  ac color to apply alpha to\n" );
		printf( "  av value of the alpha applied\n" );
		printf( "\nfilename, etc. can be repeated.\n" );
	}
	
	SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE );
	
	const int MAX_INPUT = 32;
	Input input[ MAX_INPUT ];
	
	int i;
	int numInput = 0;
	int scale = 1;
	int skip = 1;
	bool applyAlpha = false;
	KrRGBA alphaColor;
	int alphaValue = 128;
	const char* sprite = argv[1];

	for( i=2; i<argc && *argv[i] == '-'; ++i )
	{
		if ( !strncmp( argv[i], "-sc", 3 ) )
		{	
			scale = atoi( argv[i] + 3 );
		}
		else if ( !strncmp( argv[i], "-sk", 3 ) )
		{	
			skip = atoi( argv[i] + 3 );
		}
		else if ( !strncmp( argv[i], "-ac", 3 ) )
		{	
			alphaColor.FromString( argv[i] + 3 );
			applyAlpha = true;
		}
		else if ( !strncmp( argv[i], "-av", 3 ) )
		{	
			int alpha = atoi( argv[i] + 3 );
			alphaColor.c.alpha = alpha;
		}
	}
	
	while ( i+1<argc )
	{
		input[numInput].filename = argv[i];
// 		input[numInput].sprite   = argv[i+1];
		input[numInput].action   = argv[i+1];
		i += 2;
		++numInput;
	}

	char filenameBase[256];
	strcpy( filenameBase, input[0].filename );
	char* p = strrchr( filenameBase, '_' );
	if ( p ) 
	{
		*p = 0;
	}
	else
	{
		p = strchr( filenameBase, '.' );
		if ( p ) *p = 0;
	}

	FrameManager frameManager;
	for( i=0; i<numInput; ++i )
	{
		frameManager.Add( input[i].filename, input[i].action, skip, scale );
	}

	// Okay, we have frames.
	Process( &frameManager, scale, applyAlpha, alphaColor, alphaValue );

	SDL_Surface* s = Write( &frameManager, filenameBase, sprite );

	SDL_FreeSurface( s );
	SDL_Quit();
	return 0;
}