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

#ifndef KYRA_TEXT_INCLUDED
#define KYRA_TEXT_INCLUDED

#include "image.h"
#include "fontresource.h"

class KrSprite;

/**
	Defines an area of the screen to be used to display text.
	Created from a font resource.

	Note that this is a container class; it does not draw. It
	will create sprites -- as children -- for the letters of text.
	The sprites will be fully managed by the text box and should
	not be changed by the client.

	If hit testing, it may be necessary to check the Parent's parent
	to see if it is a text box.
*/
class KrTextBox : public KrImNode
{
  public:
	enum Alignment
	{
		LEFT,
		CENTER,
		RIGHT
	};
	/**	Create a box that contains text. 
		@param resource		A pointer to the font to use. 
		@param width		Width of the text box.
		@param height		Height of the text box.
		@param lineSpacing	Pixels of padding between each line.
		@param alignment	LEFT, RIGHT, or CENTER justification.
	*/	
	KrTextBox( KrFontResource* resource,
			   int width, int height,	   // set to 0 to be un-bound
			   int lineSpacing,
			   Alignment alignment = LEFT );

	virtual ~KrTextBox();

	/// Get the resource for this TextBox.
	KrFontResource*  FontResource()	{ return resource; }

	/// Size, untransformed, of this box:
	int Width()	const	{ return width; }
	/// Size, untransformed, of this box:
	int Height() const	{ return height; }

	/// Number of lines of text in this text box.
	int NumLines() const					{ return numLines; }
	/// Get the y offset to a given line.
	int GetLineY( int lineNum ) const		{ return line[lineNum].dy; }

	/// Set the text of a given linenumber, in 16 bit characters.
	void		SetText16(   const U16*  text, int lineNum );
	/// Get the text of a given linenumber, in 16 bit characters.
	const U16*  GetText16( int lineNum ) const				{ return &line[lineNum].str[0]; }
	/// A slightly faster way to get the 16 bit text.
	const std::vector<U16>& GetText16Array( int lineNum ) const  { return line[lineNum].str; }

	/// Set the text of a given linenumber, in 8 bit characters.
	void SetTextChar( const std::string& text, int lineNum );
	/// Get the text of a given linenumber, in 8 bit characters.
	void GetTextChar( std::string* buffer, int lineNum );

	/// Return the length of the line, without counting the terminator.
	int GetLineLength( int lineNum = 0 )	{	GLASSERT( line[lineNum].str.size() > 0 ); 
												return line[lineNum].str.size() - 1; }

	virtual KrImNode* Clone();

	// ---- Internal ----- //
	virtual KrTextBox* ToTextBox()	{ return this; }	
	virtual void	AddedtoTree();
	void			CreateLetters( int index );
	virtual void FlushInvalid( int win, bool cache );


  private:
	enum {
		CHILD_DEPTH = 65536	// Depth where the letters will start.
	};
	
	// The collision routines depend on letter->parent->textbox relationship.
	// Be careful changing the tree structure.
	struct TextLine
	{
		int						dy;
		std::vector<U16>			str;
		KrImNode*				parent;		
		std::vector<KrSprite*> 	letter;		
		int						width;		// The width of this line. Used for positioning of LEFT, RIGHT, CENTER.
	};

	int				width, height;	// Untransformed.
	int				numLines;		// Number of text lines in the box.
	TextLine*		line;
	KrFontResource*	resource;
	Alignment		align;
	int				lineSpacing;
};

#endif
