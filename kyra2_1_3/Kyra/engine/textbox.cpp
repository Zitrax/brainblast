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
#include "sprite.h"
#include "SDL.h"
#include "textbox.h"
#include "engine.h"
#include "../../grinliz/glgeometry.h"

using namespace grinliz;

KrTextBox::KrTextBox(	KrFontResource* _resource,
						int _width, int _height,
						int _lineSpacing,
						Alignment _align )
{
	resource = _resource;
	width = _width;
	height = _height;
	numLines = 0;
	line = 0;
	align = _align;
	lineSpacing = _lineSpacing;

	GLASSERT( _resource );

	// Create the array of TextLines.
	int sizeOfLine = lineSpacing + resource->FontHeight();
	numLines = height / sizeOfLine;

	if ( numLines > 0 )
	{
		line = new TextLine[ numLines ];
		for ( int i=0; i<numLines; i++ )
		{
			line[i].dy = i * sizeOfLine;
			line[i].str.push_back( 0 );		// use a null terminator
			line[i].parent = 0;
			line[i].width = 0;
		}
	}
}


KrTextBox::~KrTextBox()
{
	// Do not delete the child letters. They will be cleaned by the
	// tree.
	delete [] line;
}


void KrTextBox::SetTextChar( const std::string& text, int lineNum )
{
	U16* str = new U16[ text.length() + 1 ];

	U16* target  = str;
	const char* source = text.c_str();

	while( *source )
	{
		*target = *source;
		++target;
		++source;
	}
	*target = 0;

	SetText16( str, lineNum );
	delete [] str;
}


void KrTextBox::GetTextChar( std::string* buffer, int lineNum )
{
	const U16* p = &line[lineNum].str[0];
	*buffer = "";
	//int i = 0;

	if ( lineNum >=0 && lineNum < numLines )
	{
		while ( *p )
		{	
			*buffer += *p;
			++p;
		}
	}
}


void KrTextBox::SetText16( const U16* text, int lineNum )
{
	if ( lineNum >=0 && lineNum < numLines )
	{
		if ( !line[lineNum].str.empty() )
		{
			// Check if the line hasn't changed:
			if ( text && *text )
			{
				const U16 *p, *q;
				for( p = &line[lineNum].str[0], q = text;
					 *p && *q;
					 ++p, ++q )
				{
					if ( *p != *q )
						break;
				}
				// They are the same! Do nothing.
				if ( *p == 0 && *q == 0 )
					return;
			}
		}

		// Remove the existing letters, then add the new ones.
		if ( Engine() )
		{
			line[lineNum].letter.resize(0);
			if ( line[lineNum].parent )
			{
				Engine()->Tree()->DeleteNode( line[lineNum].parent );
				line[lineNum].parent = 0;
			}
		}

		int textLength = 0;
		for ( const U16* pt = text; pt && *pt; ++pt )
		{
			++textLength;
		}
		if ( text && *text )
		{
			int w = resource->FontWidth( text );
			if ( w > width )
			{
				// The line doesn't fit in the bounding area. Truncate.
				// It could be any length...so start from the front and
				// see how much we can fit.
				int n = 0;
				for( n=0; n < textLength; n++ )
				{
					if ( resource->FontWidthN( text, n ) > width )
					{
						break;
					}
				}
				if ( n > 0 ) 
					--n;
				textLength = n;
			}

			// We have the text length. Copy it to string.
			line[lineNum].str.resize( textLength );
			for ( int i=0; i<textLength; i++ )
			{
				line[lineNum].str[i] = text[i];
			}
		}
		else
		{
			line[lineNum].str.resize( 0 );
		}
		// Add a null terminator, since we pass out 16bit strings.
		line[lineNum].str.push_back( 0 );
		line[lineNum].letter.resize( line[lineNum].str.size() - 1 );
		line[lineNum].width = resource->FontWidth( &line[lineNum].str[0] );
		if ( Engine() )
		{
			CreateLetters( lineNum );
		}
// 		Invalidate();
	}
}


void KrTextBox::AddedtoTree()
{
	KrImNode::AddedtoTree();			// Calculate the transform, invalidate.
	for( int i=0; i<numLines; ++i )
	{
		CreateLetters( i );
	}
}

void KrTextBox::CreateLetters( int index )
{
	GLASSERT( Engine() );
	KrAction* action = resource->GetAction( "NONE" );
	GLASSERT( action );

	// Very important to set the offset as we go, else
	// get an extra set of DRs.
	int offset = 0;
	if ( align != LEFT )
	{
		if ( align == CENTER )
		{
			offset = ( width - line[index].width ) / 2;
		}
		else if ( align == RIGHT )
		{
			offset = width - line[index].width;
		}
	}

	if ( line[index].str.size() > 1 )		// the null-term gives 1 character of length
	{
		// Check for the parent node:
		if ( !line[index].parent )
		{
			line[index].parent = new KrImNode();
			line[index].parent->SetZDepth( CHILD_DEPTH + index );
			#ifdef DEBUG
				char buf[256];
				static int id = 0;
				sprintf( buf, "parent index=%d id=%d", index, ++id );
				line[index].parent->SetNodeName( buf );
				//GLOUTPUT( "About to add parent '%s'\n", line[index].parent->NodeName().c_str() );
			#endif
			line[index].parent->SetPos( offset, line[index].dy );
			Engine()->Tree()->AddNode( this, line[index].parent );
//			GLOUTPUT( "Line Added. name='%s' xf=%f,%f cxf=%f,%f\n",
//					  line[index].parent->NodeName().c_str(),
//					  line[index].parent->XTransform( 0 ).x.ToDouble(),
//					  line[index].parent->XTransform( 0 ).y.ToDouble(),
//					  line[index].parent->CompositeXForm( 0 ).x.ToDouble(),
//					  line[index].parent->CompositeXForm( 0 ).y.ToDouble() );
//			GLOUTPUT( "  ...Textbox.         xf=%f,%f cxf=%f,%f\n",
//					  NodeName().c_str(),
//					  XTransform( 0 ).x.ToDouble(),
//					  XTransform( 0 ).y.ToDouble(),
//					  CompositeXForm( 0 ).x.ToDouble(),
//					  CompositeXForm( 0 ).y.ToDouble() );
		}

		int  count = line[index].str.size() - 1;	// The string is null terminated. Don't need the last character.
		U16* glyph = &line[index].str[0];
		int  x = 0;

		for( int i=0; i < count; ++i )
		{
			if ( resource->GlyphInFont( glyph[i] ) )
			{
				// It's rather important to not add an empty sprite:
				// happens when a "space" is used.
				if ( action->Frame( resource->GlyphToFrame( glyph[i] ) ).Width() > 0 )
				{
					line[index].letter[i] = new KrSprite( resource );
					line[index].letter[i]->SetPos( x, 0 );
					line[index].letter[i]->SetFrame( resource->GlyphToFrame( glyph[i] ) );
					line[index].letter[i]->SetZDepth( i );
					line[index].letter[i]->SetQuality( KrQualityFast ); // Optimization; this is the only setting.
					#ifdef DEBUG
						char buf[256];
						static int id=0;
						sprintf( buf, "letter glyph='%c' id=%d", glyph[i], ++id );
						line[index].letter[i]->SetNodeName( buf );
						//GLOUTPUT( "adding letter '%s'\n", line[index].letter[i]->NodeName().c_str() );
					#endif

					Engine()->Tree()->AddNode( line[index].parent, line[index].letter[i] );
				}
				else
				{
					line[index].letter[i] = 0;
				}
				x += resource->FontWidth1( glyph[i] );
			}
			else
			{
				line[index].letter[i] = 0;
				// Need to advance x if it is a space.
				x += resource->FontWidth1( glyph[i] );
			}
		}
	}
}


void KrTextBox::FlushInvalid( int win, bool cache )
{
	// Although text is nothing but a bunch of sprites -- which could
	// manage their own bounds -- they shoot the DR processing. 
	// "pre load" a DR, which will consume the individual letters.
	//
	// Note this can be conservative -- if it misses, the letters
	// can handle themselves.
	KrImNode::FlushInvalid( win, cache );

	if ( IsVisible(win) )
	{
		for( int i=0; i<numLines; ++i )
		{
			Rectangle2I bounds;
			bool first = true;
			KrImNode* parent = line[i].parent;

			if ( parent )
			{
				for( 	GlInsideNode< KrImNode* >* childNode = parent->Child()->next;		// start after the sentinel
						childNode != parent->Child();     								// look for the sentinel
						childNode = childNode->next )
				{
					if ( first )
					{
						bounds = childNode->data->Bounds( win );
						first = false;
					}
					else
					{
						bounds.DoUnion( childNode->data->Bounds( win ) );
					}
				}
				if ( !first )
					Engine()->DirtyRectangle(win)->AddRectangle( bounds );
			}
		}		
	}
}


KrImNode* KrTextBox::Clone()
{
	KrTextBox* box = new KrTextBox( resource, width, height, lineSpacing, align );
	std::string buf;

	for( int i=0; i<numLines; ++i )
		box->SetText16( GetText16( i ), i );
	return box;
}

