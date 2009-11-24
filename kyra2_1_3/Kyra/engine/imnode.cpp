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

#include "imnode.h"
#include "SDL.h"
#include "engine.h"
#include "../../grinliz/glgeometry.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4355 )
#endif

using namespace grinliz;


KrImNode::KrImNode() : treeNode( this )
{
	parent = 0;
	engine = 0;
	depth = 0;
	nodeId = -1;
	userData = 0;

	for( int i=0; i<KR_MAX_WINDOWS; ++i )
	{
		invalid[i] = true;
		visible[i] = true;
		xTransform[i].Set();
		compositeXForm[i].Set();
		quality[i] = KrQualityNone;
		compositeQuality[i] = KrQualityFast;

		compositeBounds[i].SetInvalid();
		bounds[i].SetInvalid();
	}
}


void KrImNode::SetNodeName( const std::string& name )
{
	if ( engine )
	{
		if ( !nodeName.empty() )
			engine->Tree()->RemoveNodeNameHash( nodeName );
		engine->Tree()->AddNodeNameHash( name, this );
	}
	nodeName = name;
}


void KrImNode::SetNodeId( int id )
{
	if ( engine )
	{
		engine->Tree()->RemoveNodeIdHash( nodeId );
		engine->Tree()->AddNodeIdHash( id, this );
	}
	nodeId = id;
}


void KrImNode::SetPos( int x, int y, int win )
{
	#ifdef DEBUG
		// There are no reason for these limits rather that
		// a way to check for bad input -- the engine can handle
		// inputs much larger than this.
		GLASSERT( x > -40000 && x < 40000 );
		GLASSERT( y > -40000 && y < 40000 );
	#endif
	
	GLASSERT( win >= KR_ALL_WINDOWS && win < KR_MAX_WINDOWS );
	int start = 0;
	int end = KR_MAX_WINDOWS;

	if ( win != KR_ALL_WINDOWS )
	{
		start = win;
		end   = win + 1;
	}
	else if(Engine()) 
	{
		end = Engine()->NumWindows();
	}

	for( int i=start; i<end; ++i )
	{
		if ( x != xTransform[i].x || y != xTransform[i].y )
		{
			xTransform[i].x = x;
			xTransform[i].y = y;
			invalid[i]      = true;
		}
	}
}


void KrImNode::SetScale( GlFixed xScale, GlFixed yScale, int win )
{
	#ifdef DEBUG
		GLASSERT( xScale > 0 );
		GLASSERT( yScale > 0 );
	#endif

	GLASSERT( win >= KR_ALL_WINDOWS && win < KR_MAX_WINDOWS );
	int start = 0;
	int end   = Engine() ? Engine()->NumWindows() : KR_MAX_WINDOWS;

	if ( win != KR_ALL_WINDOWS )
	{
		start = win;
		end   = win + 1;
	}

	for( int i=start; i<end; ++i )
	{
		if ( xScale != xTransform[i].xScale || yScale != xTransform[i].yScale )
		{
			xTransform[i].xScale = xScale;
			xTransform[i].yScale = yScale;
			invalid[i] = true;
		}
	}
}


void KrImNode::SetTransform( KrMatrix2& xForm, int win )
{
	#ifdef DEBUG
		GLASSERT( xForm.xScale > 0 );
		GLASSERT( xForm.yScale > 0 );
	#endif

	GLASSERT( win >= KR_ALL_WINDOWS && win < KR_MAX_WINDOWS );
	int start = 0;
	int end   = Engine() ? Engine()->NumWindows() : KR_MAX_WINDOWS;

	if ( win != KR_ALL_WINDOWS )
	{
		start = win;
		end   = win + 1;
	}

	for( int i=start; i<end; ++i )
	{	
		if ( xForm != xTransform[i] )
		{
			xTransform[i] = xForm;
			invalid[i] = true;
		}
	}
}


void KrImNode::SetColor( const KrColorTransform& color, int win )
{
	GLASSERT( win >= KR_ALL_WINDOWS && win < KR_MAX_WINDOWS );
	int start = 0;
	int end   = Engine() ? Engine()->NumWindows() : KR_MAX_WINDOWS;

	if ( win != KR_ALL_WINDOWS )
	{
		start = win;
		end   = win + 1;
	}

	for( int i=start; i<end; ++i )
	{
		if ( color != cTransform[i] )
		{
			cTransform[i] = color;
			invalid[i] = true;
		}
	}
}


void KrImNode::Invalidate( int win )
{
	GLASSERT( win >= KR_ALL_WINDOWS && win < KR_MAX_WINDOWS );
	int start = 0;
	int end   = Engine() ? Engine()->NumWindows() : KR_MAX_WINDOWS;

	if ( win != KR_ALL_WINDOWS )
	{
		start = win;
		end   = win + 1;
	}
	
	for( int i=start; i<end; ++i )
	{
		invalid[i] = true;
	}
}


void KrImNode::SetQuality( int _quality, int win )
{
	GLASSERT( win >= KR_ALL_WINDOWS && win < KR_MAX_WINDOWS );
	int start = 0;
	int end   = Engine() ? Engine()->NumWindows() : KR_MAX_WINDOWS;

	if ( win != KR_ALL_WINDOWS )
	{
		start = win;
		end   = win + 1;
	}
	
	for( int i=start; i<end; ++i )
	{
		if ( _quality != quality[i] )
		{
			quality[i] = _quality;
			invalid[i] = true;
		}
	}
}


void KrImNode::Resort( KrImNode* resortMe )
{
	// See if we need to move at all. If so, take ourselves out
	// of the list and scoot up or down.
	int depth = resortMe->ZDepth();
	GlInsideNodeIt< KrImNode* > it( child );

	// resortMe should always be a child of this.
	it.SetCurrent( resortMe->treeNode.next );
	#ifdef DEBUG
		if ( !it.Done() ) GLASSERT( it.CurrentData()->Parent() == this );
	#endif

	if (    !it.Done()
	     && depth > it.CurrentData()->ZDepth() )
	{
		// Resort me is removed, so we need a new current
		it.Next();
		resortMe->treeNode.Remove();

		while (    !it.Done()
				&& depth > it.CurrentData()->ZDepth() )
		{
			it.Next();
		}
		it.InsertBefore( resortMe->treeNode );
	}

	it.SetCurrent( resortMe->treeNode.prev );
	#ifdef DEBUG
		if ( !it.Done() ) GLASSERT( it.CurrentData()->Parent() == this );
	#endif

	if (    !it.Done()
	     && depth < it.CurrentData()->ZDepth() )
	{
		it.Prev();
		resortMe->treeNode.Remove();

		while (    !it.Done()
				&& depth < it.CurrentData()->ZDepth() )
		{
			it.Prev();
		}
		it.InsertAfter( resortMe->treeNode );
	}
	#ifdef VALIDATE_DEBUG
		GlInsideNodeIt< KrImNode* > testIt( child );

		testIt.Begin();
		int testZ = testIt.CurrentData()->ZDepth();

// 		GLOUTPUT( "zdepth " );
		for( ; !testIt.Done(); testIt.Next() )
		{
			GLASSERT( testIt.CurrentData()->ZDepth() >= testZ );
// 			GLOUTPUT( "%x=%d ", testIt.CurrentData(), testIt.CurrentData()->ZDepth() );
			testZ = testIt.CurrentData()->ZDepth();
		}
// 		GLOUTPUT( "\n" );
	#endif
}


void KrImNode::CalcTransform( int i )
{
	// Running performance on this gave about equal
	// time consumption in the 3 sections. Color, quality, position.

	// Now transform this.
	compositeXForm[i] = xTransform[i];
	compositeCForm[i] = cTransform[i];

	if ( parent )
	{
		// Spacial
		compositeXForm[i].Composite( parent->CompositeXForm( i ) );

		// Color
		// This can actually be a little expensive, so check that
		// we don't have an identity first.
		if ( !parent->CompositeCForm( i ).IsIdentity() )
		{
			compositeCForm[i].Composite( parent->CompositeCForm( i ) );
   		}

		// Transform the quality
		if ( compositeXForm[i].IsScaled() )
		{
			compositeQuality[i] = quality[i];
			if ( compositeQuality[i] == KrQualityNone )
			{
				KrImNode* node;
				for( node = parent; node; node = node->parent )
				{
					if ( node->quality[i] != KrQualityNone )
					{
						compositeQuality[i] = node->quality[i];
						break;
					}
				}
			}
			if ( compositeQuality[i] == KrQualityNone )
			{
				compositeQuality[i] = KrQualityFast;
			}
  		}
		else
		{
			compositeQuality[i] = KrQualityFast;
		}
	}
//	GLOUTPUT( "KrImNode '%s' win=%d rel=%f,%f  composite=%f,%f\n", 
//			  NodeName().c_str(),
//			  i,
//			  xTransform[i].x.ToDouble(),
//			  xTransform[i].y.ToDouble(),
//			  compositeXForm[i].x.ToDouble(),
//			  compositeXForm[i].y.ToDouble() );
}


void KrImNode::SetZDepth( int z )
{
	if ( z != depth )
	{
		depth   = z;

// 		int count = KR_MAX_WINDOWS;
// 		if ( Engine() ) count = Engine()->NumWindows();
// 		for( int i=0; i<count; ++i )
// 		{
// 			invalid[i] = true;
// 		}
		Invalidate( KR_ALL_WINDOWS );

		// If there is no parent, this node is not in a tree. That's
		// fine -- we'll sort it in correctly when it gets added.
		if ( parent )
		{
			// Ask our parent to move us to the correct z-position.
			parent->Resort( this );
		}
	}
}


bool KrImNode::IsVisible( int window )
{
	KrImNode* node;

	for( node = this; node; node = node->parent )
	{
		if ( node->visible[window] == false )
			return false;
	}
	return true;
}


void KrImNode::SetVisible( bool _visible, int win )
{
	GLASSERT( win >= KR_ALL_WINDOWS && win < KR_MAX_WINDOWS );

	if ( win == KR_ALL_WINDOWS )
	{
		int count = KR_MAX_WINDOWS;
		if ( Engine() ) count = Engine()->NumWindows();

		for( int i=0; i<count; ++i )
		{
			if ( visible[i] != _visible )
			{
				visible[i] = _visible;
				invalid[i] = true;
			}	
		}
	}
	else
	{
		if ( visible[win] != _visible )
		{
			visible[win] = _visible;
			invalid[win] = true;
		}	
	}
}


void KrImNode::ScreenToObject( int x, int y, Vector2< GlFixed >* object, int i )
{
	object->x = ( GlFixed( x ) - compositeXForm[i].x ) / compositeXForm[i].xScale;
	object->y = ( GlFixed( y ) - compositeXForm[i].y ) / compositeXForm[i].yScale;
}


void KrImNode::AddedtoTree()
{
	GLASSERT( Engine() );
	for( int i=0; i<Engine()->NumWindows(); ++i )
	{
		CalcTransform( i );
		Invalidate( i );
	}
}


void KrImNode::LeavingTree()
{
	// Nothing!
}


//KrWidget* KrImNode::TopWidget()
//{
//	KrWidget* w = 0;
//	for( KrImNode* node = Parent(); 
//		 node; 
//		 node = node->Parent() )
//	{
//		if ( node->ToWidget() )
//			w = node->ToWidget();
//		else
//			break;		// if it's not a widget, it will never recurse down to this object,
//						// so it can't be our parent.
//	}
//	return w;
//}


//KrWidget* KrImNode::GetWidget()
//{
//	if ( ToWidget() )
//		return ToWidget();
//	else if ( Parent() && Parent()->ToWidget() )
//		return Parent()->ToWidget();
//	else
//		return 0;
//}


KrImNode* KrImNode::Clone()
{
	return new KrImNode();
}
