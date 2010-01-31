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

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "encoder.h"
#include "SDL.h"
#include "SDL_endian.h"
#include "fontresource.h"
#include "engine.h"
#include "sdlutil.h"

using namespace grinliz;

const std::string KrFontResource::fontName = "Font";

KrFontResource::KrFontResource( U32 size, 
								SDL_RWops* data )

	: KrSpriteResource( size, data )
{
	// After the sprite has initialized, read our startIndex and type.
	startIndex = SDL_ReadLE32( data );
	fontType   = SDL_ReadLE16( data );
	GLASSERT( fontType == FIXED || fontType == SFONT );
	space      = SDL_ReadLE32( data );

	GLOUTPUT(( "startindex=%d type=%d\n", startIndex, fontType ));
	
//	actionArr = GetActionByIndex( 0 );
//	GLASSERT( action );

	CalcSpaceWidth();
}


KrFontResource::KrFontResource( const std::string& name,
								KrPaintInfo* info,
								int startingGlyph,
								int addSpaceGlyph,
								int type,
								int length )
	: KrSpriteResource( name )
{
	startIndex = startingGlyph;
	fontType   = type;
	space      = addSpaceGlyph;

	KrAction* action = new KrAction( "NONE" );

	if ( fontType == FIXED )
	{
		int width  = info->width / length;
		int height = info->height;

		for( int i = 0; i < length; i++ )
		{
			action->AddFrame();
			KrRle* rle = action->GetFrame( i );

			rle->Create(	info,
							i * width, 0, width, height,
							i * width, 0,	// the hotspots are absolute coordinates!
							width, height );
		}
	}
	else
	{
		KrPainter painter( info );
		int height = info->height - 1;
		int x = 0;
		int transparent = 0;

		while ( x < info->width )
		{
			x += painter.CalcNotTransparentRun( x, info->width - 1, 0 );
			if ( x < info->width )
				transparent = painter.CalcTransparentRun( x, info->width - 1, 0 );
			else
				transparent = 0;

			if ( x < info->width && transparent > 0 )
			{
				action->AddFrame();
				KrRle* rle = action->GetFrame( action->NumFrames() - 1 );

 				rle->Create(	info,
								x, 1, transparent, height,
								x, 1,	// the hotspots are absolute coordinates!
								transparent, height );
			}
			x += transparent;
		}
	}

	AddAction( action );

	CalcSpaceWidth();
}


KrFontResource::~KrFontResource()
{
}


void KrFontResource::CalcSpaceWidth()
{
	int total = 0;
	for( int i=0; i<actionArr[0]->NumFrames(); ++i )
		total += actionArr[0]->Frame( i ).Delta().x;
	
	spaceWidth = total / actionArr[0]->NumFrames();
	spaceWidth = Max( 1, spaceWidth );
}


int KrFontResource::FontWidth( const U16* str )
{
	// Walk the string, check to make sure each character is in
	// the font, and add up the widths.
	const U16* p;
	int width = 0;
	int glyph;

	for( p=str; p && *p; ++p )
	{
		if ( *p == space )
		{
			width += spaceWidth;
		}
		else
		{
			glyph = (*p) - startIndex;
			if ( glyph >= 0 && glyph < actionArr[0]->NumFrames() )
			{
				width += actionArr[0]->Frame( glyph ).Delta().x;
			}
		}
	}
	return width;
}


int KrFontResource::FontWidthN( const U16* str, int n )
{
	// Walk the string, check to make sure each character is in
	// the font, and add up the widths.
	const U16* p;
	int width = 0;
	int glyph;
	int i;

	for( p=str, i=0; p && *p && i < n; ++p, ++i )
	{
		if ( *p == space )
		{
			width += spaceWidth;
		}
		else
		{
			glyph = (*p) - startIndex;
			if ( glyph >= 0 && glyph < actionArr[0]->NumFrames() )
			{
				width += actionArr[0]->Frame( glyph ).Delta().x;
			}
		}
	}
	return width;
}



int KrFontResource::FontWidth1( U16 glyphCode )
{
	if ( glyphCode == space )
	{
		return spaceWidth;
	}

	int glyph = glyphCode - startIndex;
	if ( glyph >= 0 && glyph < actionArr[0]->NumFrames() )
	{
		return actionArr[0]->Frame( glyph ).Delta().x;
	}
	return 0;
}


bool KrFontResource::GlyphInFont( U16 glyphCode )
{
	int glyph = glyphCode - startIndex;
	if ( glyph >= 0 && glyph < actionArr[0]->NumFrames() )
	{
		return true;
	}
	return false;	
}


int  KrFontResource::GlyphToFrame(  U16 glyphCode )
{
	int glyph = glyphCode - startIndex;
	GLASSERT( glyph >= 0 && glyph < actionArr[0]->NumFrames() );
	return glyph;
}


void KrFontResource::Draw( KrPaintInfo* paintInfo,
						   U16 rawGlyphCode,
						   const KrMatrix2& matrix,
						   const KrColorTransform& cForm,
						   const Rectangle2I& clipping )
{
	#ifdef ASSERT_IF_NOT_CACHED
		GLASSERT( !matrix.IsScaled() );
	#endif

	if ( rawGlyphCode == space )
		return;

	int glyph = rawGlyphCode - startIndex;

	if ( glyph >=0 && glyph < actionArr[0]->NumFrames() )
	{
		actionArr[0]->Draw(	paintInfo,
							glyph,
							matrix,
							cForm,
							clipping );
		}
}


void KrFontResource::Save( KrEncoder* encoder )
{
	encoder->StartTag( KYRATAG_FONT );
	
	WriteString( encoder->Stream(), ResourceName() );
	encoder->WriteCached( ResourceName() );

	SDL_WriteLE32( encoder->Stream(), NumActions() );

	for( int i=0; i<NumActions(); ++i )
	{
		actionArr[i]->Save( encoder );
	}
	SDL_WriteLE32( encoder->Stream(), startIndex );
	SDL_WriteLE16( encoder->Stream(), fontType );
	SDL_WriteLE32( encoder->Stream(), space );
	encoder->EndTag();
}

