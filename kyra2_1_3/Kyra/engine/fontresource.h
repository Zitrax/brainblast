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


#ifndef KYRA_FONTRESOURCE_INCLUDED
#define KYRA_FONTRESOURCE_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "spriteresource.h"

/** A font is what text is created from.

	A font resource is a child of the sprite resource, because a Font
	Resource is a multi-frame Sprite Resource with some 
	additional capability. (On the other hand, a Font
	is not a Sprite.)

	That said, the Sprite API is almost certainly confusing. When
	using KrFontResource, you should probably focus on the
	KrFontResource class rather than the parent KrSpriteResource.
*/

class KrFontResource : public KrSpriteResource
{
  public:
	// Create by reading from a .dat file
	KrFontResource( U32 size, SDL_RWops* data );
		
	KrFontResource( const std::string& name,
					KrPaintInfo* info,
					int startingGlyph,
					int addSpaceGlyph,
					int type,
					int numGlyphs );	// only need for fixed fonts

	virtual ~KrFontResource();

	virtual	U32 Type()								{ return KYRATAG_FONT; }
	virtual const std::string&	TypeName()			{ return fontName; }
	virtual KrFontResource* ToFontResource()		{ return this; }

	enum {
		FIXED,
		SFONT
	};

	/// Return the height of this font.
	int FontHeight()	{ return actionArr[0]->Frame( 0 ).Delta().y; }

	int FontWidth1( U16 glyphCode );	///< Return the width of a single glyph.
	int FontWidth( const U16* str );	///< Return the width of a string.
	int FontWidthN( const U16* str, int nChars );	///< Return the width of the string up to nChars characters.
	
	/** Return true if the given glyphcode is in this font set.
		A space is not in the font, but it will have a width.
	*/
	bool GlyphInFont( U16 glyphCode );
	int  GlyphToFrame(  U16 glyphCode );

	/*  Draw a character in the font.
		@param  paintInfo	Information about the target surface for drawing (optimizing).
		@param	glyphCode	The glyph code: ascii, latin-1, whatever
		@param	x			X location in pixels.
		@param	y			Y location in pixels.
		@param  cForm		Color transformation applied to the drawing.
		@param	clip		A clipping rectangle, which can be null.
	*/
	void Draw( KrPaintInfo* paintInfo,
			   U16 glyphCode,
			   const KrMatrix2& matrix,
			   const KrColorTransform& cForm,
			   const grinliz::Rectangle2I& clipping );

	virtual void Save( KrEncoder* encoder );

  private:
	// The font (and text) somewhat abuses the sprite system. Fonts
	// are single action sprites, with a number of frames equal to
	// the length of the font. The delta is interpreted as the 
	// bounding box of the letter.

	void CalcSpaceWidth();

	const static std::string fontName;

	U32			startIndex;
	U32			space;
	int			spaceWidth;
	int			fontType;
//	KrAction*	action;		// A font is only one action.

};


#endif
