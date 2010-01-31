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

#include "imagetree.h"
#include "SDL.h"
#include "engine.h"
#include "../../grinliz/glgeometry.h"
//#include "../util/glprimitive.h"

#ifndef KYRA_BUILD_INCLUDED
	#error Build file missing.
#endif

using namespace grinliz;

KrImageTree::KrImageTree( KrEngine* _engine )
{
	int i;

	engine = _engine;
	root		= new KrImNode;
	offsetRoot	= new KrImNode;
	
	AddNode( root, offsetRoot );

	for( i=0; i<engine->NumWindows(); ++i )
	{
		root->SetPos(	engine->ScreenBounds(i).min.x, 
						engine->ScreenBounds(i).min.y,
						i );
		root->CalcTransform( i );
	}

	//GLOUTPUT(( "Root: %x  Offset: %x\n", root, offsetRoot ));
}


KrImageTree::~KrImageTree()
{
	Clear( root );
}


void KrImageTree::Clear( KrImNode* parent )
{
	// Recursively delete the tree. It is important that
	// we delete from the outside in.

	GlInsideNode< KrImNode* >* child = &(parent->child);
	child = child->next;	// skip the sentinel
	while ( !child->IsSentinel() )
	{
		GlInsideNode< KrImNode* >* temp = child;
		child = child->next;

		Clear( temp->data );
	}
	delete parent;
}


void KrImageTree::AddNode( KrImNode* parent, KrImNode* addMe )
{
	if ( !parent )
	{
		parent = offsetRoot;
	}

	addMe->parent = parent;
	addMe->engine = engine;

	// Adds a node, keeping the depth sorted correctly.
	GlInsideNodeIt< KrImNode* > it = parent->ChildTreeIterator();

	for( it.Begin(); !it.Done(); it.Next() )
	{
		if ( addMe->ZDepth() < it.CurrentData()->ZDepth() )
		{
			it.InsertBefore( addMe->treeNode );
			break;
		}
	}
	if ( it.Done() )
	{
		// Add at the end. (It's a circular list - the end
		// is just before the sentinel.)
		it.InsertBefore( addMe->treeNode );
	}

	#ifdef VALIDATE_DEBUG
		// Validate!
		for( it.Begin(); !it.Done(); it.Next() )	
		{
			GlInsideNode< KrImNode* >* prev = it.CurrentNode()->prev;
			GlInsideNode< KrImNode* >* next = it.CurrentNode()->next;

			if ( !prev->IsSentinel() )
				GLASSERT( it.CurrentData()->ZDepth() >= prev->data->ZDepth() );
			if ( !next->IsSentinel() )
				GLASSERT( it.CurrentData()->ZDepth() <= next->data->ZDepth() );
		}		 
	#endif

	addMe->Invalidate( KR_ALL_WINDOWS );

	if ( addMe->NodeId() >= 0 )
	{
        idMap[ addMe->NodeId() ] = addMe;
		GLOUTPUT(( "Node id=%d added\n", addMe->NodeId() ));
	}
	if ( !addMe->NodeName().empty() && addMe->NodeName().length() > 0 )
	{
        nameMap[ addMe->NodeName() ] = addMe;
	}

	// WARNING: the widgets will add nodes during this call, which makes
	// this function recursive.
	addMe->AddedtoTree();

	#ifdef VALIDATE_DEBUG
		ValidateTree( root );
	#endif
}


bool KrImageTree::DeleteNode( KrImNode* removeMe )
{
	bool ok = true;
	// This call is recursive -- it takes out the given
	// node and all the children.
	GlInsideNode< KrImNode* >* child = &(removeMe->child);
	child = child->next;	// skip the sentinel
	while ( !child->IsSentinel() )
	{
		GlInsideNode< KrImNode* >* temp = child;
		child = child->next;

		DeleteNode( temp->data );
	}

	// Unlink the node.
	removeMe->treeNode.Remove();
	// Give it a chance to clean up.
	removeMe->LeavingTree();

	if ( removeMe->NodeId() >= 0 )
	{	
		idMap.erase( removeMe->NodeId() );
	}
	if ( !removeMe->NodeName().empty() )
	{	
		nameMap.erase( removeMe->NodeName() );
	}
	delete removeMe;
	
	#ifdef VALIDATE_DEBUG
		ValidateTree( root );
	#endif
	return ok;
}


void KrImageTree::Walk()
{	
	for( int i=0; i<engine->NumWindows(); ++i )
	{
		treeDepth = 1;	// reserve depth 0 for background. Note that windows don't overlap,
						// so we can save some depths doing this
		Walk( root, root->IsInvalid( i ), root->IsThisVisible(), i );
//		if ( i == 2 && engine->DirtyRectangle( i )->NumRect())
//			engine->DirtyRectangle( i )->PrintRects( "KrImageTree::Walk done tree 2" );
	}
}


void KrImageTree::Walk( KrImNode* walkNode, bool invalid, bool visible, int currentWin )
{
	// The child will use the parents composite transformation
	// in its calculation, so parent nodes *must* be transformed
	// before the children.
	if ( invalid || walkNode->IsInvalid(currentWin) )
	{
		walkNode->FlushInvalid( currentWin, true );	// Adds current bounds to DR

		// Even if invisible, we need the previous FlushInvalid, because
		// the visibility could have changed between the last frame and
		// this one. However, if the walkNode is now invisible, 
		// its new position can't cause a repaint.
		//
		// This means that invisible nodes don't have correct transforms.

		if ( visible )
		{
			walkNode->CalcTransform( currentWin );		// Calculates new bounds.
			walkNode->FlushInvalid( currentWin, false );	// Adds new bounds to DR
		}
		invalid = true;
	}

	// Our children will be drawn on top of us. Only used for OpenGL. In normal mode,
	// we draw the tree in order and don't need an explicit z-value.
	//GLASSERT( walkNode != root || treeDepth == 1 );
//	walkNode->treeDepth = ++treeDepth;

	// A check for openGL. See notes where constant is defined.
//	GLASSERT( treeDepth < kKrOglDistance );

	if ( visible )
	{
		// As we walk each child node, add its composite bounds to ours.
		walkNode->compositeBounds[currentWin] = walkNode->bounds[currentWin];
	}

	#ifdef DEBUG
		// Text -- damn it -- can have invalid bounds.
		// if ( walkNode->ToImage() ) GLASSERT( walkNode->bounds.IsValid() );
		if ( !walkNode->ToImage() ) GLASSERT( !walkNode->bounds[currentWin].IsValid() );
	#endif

	// Even if not visible, it is critical to walk the children, in case
	// they were visible on the last frame.
	//
	for( 	GlInsideNode< KrImNode* >* childNode = walkNode->child.next;		// start after the sentinel
			childNode != &walkNode->child;     									// look for the sentinel
			childNode = childNode->next )
	{
		KrImNode* child = childNode->data;
		Walk( child, invalid, visible, currentWin );

		// Check the child's composite bounds and add them in.
		// Note here, we can use visibility.
		if ( visible && child->CompositeBounds(currentWin).IsValid() )
		{
			if ( walkNode->compositeBounds[currentWin].IsValid() )
				walkNode->compositeBounds[currentWin].DoUnion( child->CompositeBounds(currentWin) );
			else
				walkNode->compositeBounds[currentWin] =  child->CompositeBounds(currentWin);
		}
	}

	// We are done with this node and all its children, so
	// its invalid state will be cleared. It will be re-drawn
	// based on the dirty rectangle, not the invalid state.
 	walkNode->ClearInvalid( currentWin );
}


void KrImageTree::DrawWalk( const Rectangle2I& dr, KrPaintInfo* info, int win )
{
	#ifdef DEBUG
 		if ( info->OpenGL() )
		{
			GLASSERT( dr == engine->ScreenBounds( win ) );
			//GLASSERT( root->CompositeBounds(win).IsValid() ); Can be invalid for tree with no images.
			//GLASSERT( root->CompositeBounds(win).Intersect( dr ) );
		}
	#endif

	if (    root->CompositeBounds(win).IsValid()
		 && root->CompositeBounds(win).Intersect( dr ) )
	{
		DrawWalk( dr, root, info, win );
	}
}


// Child nodes in Kyra are drawn on top of their parents. This recursive
// call needs to be organized so that this property is maintained.
// It is assumed that the caller has called with valid composite
// bounds.

void KrImageTree::DrawWalk( const Rectangle2I& dr, KrImNode* node, KrPaintInfo* info, int win )
{
	GLASSERT( node->CompositeBounds(win).IsValid() );
	GLASSERT( dr.Intersect( node->CompositeBounds(win) ) );

	// Draw the children first on top, so the parent draws first.
	// For opengl mode, the depth has been written to the image nodes,
	// since there is no draw order.
	if ( node->Bounds(win).IsValid() && node->IsVisible(win) )
	{
		if ( dr.Intersect( node->Bounds(win) ) )
		{
			GLASSERT( node->ToImage() );
			KrImage* image = node->ToImage();
			if ( image )
			{
				image->Draw( info, dr, win );
			}
		}
	}

	GlInsideNodeIt< KrImNode* > it( node->child );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		//TLW: here's the expensive part of the code... by changing the order of 
		//	operations, get a time savings, as the most expensive to least expense goes
		//		Intersect(quickest), IsVisible, IsValid(slowest)
		//
		//	Also, If its not valid, it'll never intersect... unnecessary test
		//		it.CurrentData()->CompositeBounds(win).IsValid()
		//
		if (	it.CurrentData()->CompositeBounds(win).Intersect( dr )
			 && it.CurrentData()->IsVisible(win) )
		{
			DrawWalk( dr, it.CurrentData(), info, win );
		}
	}
}


void KrImageTree::AddNodeNameHash( const std::string& name, KrImNode* node )
{
    nameMap[ name ] = node;
}


void KrImageTree::RemoveNodeNameHash( const std::string& name )
{
	nameMap.erase( name );
}


void KrImageTree::RemoveNodeIdHash( int id )
{
	idMap.erase( id );
}


void KrImageTree::AddNodeIdHash( int id, KrImNode* node )
{
    idMap[ id ] = node;
}

KrImNode* KrImageTree::FindNodeById( int id )
{
//	KrImNode* ret = 0;
//	idMap.Find( id, &ret );
	std::map< U32, KrImNode* >::iterator it = idMap.find( id );
	if ( it != idMap.end() )
		return it->second;
	return 0;
}


KrImNode* KrImageTree::FindNodeByName( const std::string& name )
{
//	KrImNode* ret = 0;
//	nameMap.Find( name, &ret );
//	return ret;
	std::map< std::string, KrImNode* >::iterator it = nameMap.find( name );
	if ( it != nameMap.end() )
		return it->second;
	return 0;
}

#ifdef DEBUG
void KrImageTree::ValidateTree( KrImNode* root )
{
	GlInsideNodeIt< KrImNode* > it( root->child );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		// Check the depths and the parents.
		GLASSERT( it.CurrentData()->Parent() == root );

		if ( !it.CurrentNode()->prev->IsSentinel() )
			GLASSERT( it.CurrentNode()->prev->data->ZDepth() <= it.CurrentData()->ZDepth() );
		if ( !it.CurrentNode()->next->IsSentinel() )
			GLASSERT( it.CurrentNode()->next->data->ZDepth() >= it.CurrentData()->ZDepth() );

		ValidateTree( it.CurrentData() );
	}
}
#endif


void KrImageTree::HitTest( int x, int y, KrImNode* startHere, int flags, std::vector<KrImage*>* outputArray, int* window )
{
	outputArray->resize(0);
	*window = -1;

	// Figure out which window this hit-test is in.
	for( int i=0; i<engine->NumWindows(); ++i )
	{
		Rectangle2I bounds = engine->ScreenBounds( i );
		if ( bounds.Intersect( x, y ) )
		{
			*window = i;
			break;
		}
	}
	
	if ( !startHere )
		startHere = offsetRoot;

	if ( *window >= 0 )
	{
		HitTestRec( startHere, x, y, flags, outputArray, *window );	
	}
}


bool KrImageTree::HitTestRec( KrImNode* node, int x, int y, int flags, std::vector<KrImage*>* output, int window )
{
	// Check our composite bounds, if these don't intersect, no dice.
 	if ( !node->CompositeBounds( window ).Intersect( x, y ) )
	{
		return false;
	}
	
	// Walk the children first, since they are on top of the parent.
	// Note that we need to go in reverse order, so the object
	// closest to the user is clicked first.
	GlInsideNodeIt< KrImNode* > it( node->child );

	for( it.Last(); !it.Done(); it.Prev() )
	{
// 		HitTestRec( it.CurrentData(), x, y, flags, output, window );
		bool abort = HitTestRec( it.CurrentData(), x, y, flags, output, window );
		if ( abort == true )
			return true;
	}

	if ( node->ToImage() )
	{
		// Ignore transparent and invisible items.
// 		if (    node->CompositeCForm().Alpha() > 0 )
// // // 			 || flags & HIT_TRANSPARENT_IMAGES )
// 		{	
// 			if ( node->IsVisible() )
// 			{ 
				bool hit = node->HitTest( x, y, flags, output, window );
				if ( hit )
				{
					if ( flags & GET_ALL_HITS )
						return false;	// don't abort: get everything.
					else	
						return true;	// return on the first hit.
				}
// 			}
// 		}
	}
	return false;	// keep going.
}


bool KrImageTree::CheckSiblingCollision( KrImNode* node, std::vector<KrImage*>* outputArray, int window )
{
	bool ret = false;
	outputArray->resize(0);

	KrImNode* parent   = node->Parent();
	KrImage* checkThis = node->ToImage();
	if (	parent 
		 && checkThis )
	{
		GlInsideNodeIt< KrImNode* > it( parent->child );

		for( it.Begin(); !it.Done(); it.Next() )
		{
			if (    it.CurrentData() != checkThis
				 && it.CurrentData()->ToImage()
			     && checkThis->CheckCollision( it.CurrentData()->ToImage(), window ) )
			{
				ret = true;
				outputArray->push_back( it.CurrentData()->ToImage() );
			}
		}
	}
	return ret;
}


bool KrImageTree::CheckChildCollision( KrImNode* check, KrImNode* parent, std::vector<KrImage*>* outputArray, int window )
{
	bool ret = false;
	outputArray->resize(0);

	KrImage* checkThis = check->ToImage();
	if (	checkThis
		 && parent->CompositeBounds( window ).Intersect( checkThis->Bounds( window ) ) )
	{
		GlInsideNodeIt< KrImNode* > it( parent->child );

		for( it.Begin(); !it.Done(); it.Next() )
		{
			if (    it.CurrentData() != checkThis
				 && it.CurrentData()->ToImage()
			     && checkThis->CheckCollision( it.CurrentData()->ToImage(), window ) )
			{
				ret = true;
				outputArray->push_back( it.CurrentData()->ToImage() );
			}
		}
	}
	return ret;
}


bool KrImageTree::CheckAllCollision( KrImNode* checkThis, std::vector<KrImage*>* outputArray, int window )
{
	bool ret = false;
	outputArray->resize(0);

	if ( checkThis->ToImage() )
		CheckAllCollisionWalk( &ret, Root(), checkThis->ToImage(), outputArray, window );
	return ret;
}


void KrImageTree::CheckAllCollisionWalk( bool* hit, KrImNode* node, KrImage* checkThis, std::vector<KrImage*>* outputArray, int window )
{
	// check all the children:
	GlInsideNodeIt< KrImNode* > it( node->child );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		if (    it.CurrentData() != checkThis
			 && it.CurrentData()->ToImage()
			 && checkThis->CheckCollision( it.CurrentData()->ToImage(), window ) )
		{
			*hit = true;
			outputArray->push_back( it.CurrentData()->ToImage() );
		}

		if ( it.CurrentData()->CompositeBounds( window ).Intersect( checkThis->Bounds( window ) ) )
			CheckAllCollisionWalk( hit, it.CurrentData(), checkThis, outputArray, window );
	}
}

