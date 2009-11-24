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


#ifndef KYRA_RENDER_INCLUDED
#define KYRA_RENDER_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "../util/gllist.h"
#include "SDL.h"
#include "../engine/krmath.h"
//#include "../util/glmap.h"

// Include the other image headers, so clients just need
// to #include "image.h"

#include "sprite.h"

class KrEngine;
class KrMappedRect;


/**	The ImageTree contains all the Image Nodes.
*/
class KrImageTree
{
  public:
	/*  The tree has to live in an engine in order
		to get to dirty rectangles and StartLists.
	*/
	KrImageTree( KrEngine* engine );
	~KrImageTree();

	/// Return the very base node of the tree. It is a KrImNode and never a more derived type.
	KrImNode* Root()	{ return offsetRoot; }

	/** Add a ImNode to the tree. The parent can be specified; if
		not, the root will be used.
		The node added -- 'addMe' -- is handed over by ownership. It
		will be deleted when the tree is deleted or DeleteNode is
		called.

		Node children cannot directly be added. There is no
		KrImNode::AddChild function for instance; everything must
		be added through the tree.
	*/
	void	AddNode( KrImNode* parent, KrImNode* addMe );
	
	/** Call this to delete a node and remove it from the tree.

		@WARNING: DeleteNode( this ) can cause a crash.
				  It's a temptingly useful thing, but in the case
				  where parent objects hold on to pointers, those
				  pointers go dangling. It's best to avoid
				  DeleteNode( this ) usage.
	*/
	bool	DeleteNode( KrImNode* removeMe );

	///	Find a ImNode by its id.
	KrImNode*	FindNodeById( int id );
	///	Find a ImNode by its name.
	KrImNode*	FindNodeByName( const std::string& name );

	/** Given a point -- in screen coordinates -- determine
		what ImageNodes are under that point. Note that only
		images have bounds and are drawn, so only images
		can be hit. 

		If 'startHere' is null the whole tree will be searched,
		else the search will cover 'startHere' and its children.
		
		HitTest, by defualt, stops at the first non-transparent
		pixel it hits. There are flags to modify this behavior:

		- ALWAYS_INSIDE_BOX hits inside boxes that aren't filled
		- GET_ALL_HITS, not just the first
	*/
	void   HitTest( int x, int y, KrImNode* startHere,
					int flags,
					std::vector<KrImage*>* outputArray,
					int *windowIndex );

	/**	Given a node, check if it collides with any of its own siblings. Returns
		true if any collisions occur, and returns an array (outputArray)
		of all the KrImages collided with.

		See KrImage::CheckCollision for a discussion of collision issues.
	*/
	bool CheckSiblingCollision( KrImNode* checkThis,
								std::vector<KrImage*>* outputArray, int window = 0 );

	/**	Given a node "checkThis" and a parent, check if "checkThis"
		collides with any of the children of the parent. There
		does not need to be any relationship between "checkThis" and
		"parent".
		
		Returns	true if any collisions occur, and returns an array (outputArray)
		of all the KrImages collided with.

		See KrImage::CheckCollision for a discussion of collision issues.
	*/
	bool CheckChildCollision( KrImNode* checkThis, KrImNode* parent,
							  std::vector<KrImage*>* outputArray, int window = 0 );

	/**	Given a node, check if it collides with anything in the Tree.
		Returns true if any collisions occur, and returns an array
		of everything collided with.

		See KrImage::CheckCollision for a discussion of collision issues.
	*/
	bool CheckAllCollision( KrImNode* checkThis, std::vector<KrImage*>* outputArray, int window = 0 );
	
	enum 
	{
		/** Hit the inside of a box, even if the box is not filled. */
		ALWAYS_INSIDE_BOX			= 0x01,
		/** Get every object hit, not just the first */
		GET_ALL_HITS				= 0x04,
	};

	/// Flushes the cached state of the tree and makes it ready to draw.
	void Walk();

	void DrawWalk( const grinliz::Rectangle2I& dr, KrPaintInfo* info, int window );

	// The tree maintains a look up table to the objects in it.
	// These methods are called by the KrImNodes.
	void AddNodeNameHash( const std::string& name, KrImNode* node );
	void RemoveNodeNameHash( const std::string& name );
	void RemoveNodeIdHash( int id );
	void AddNodeIdHash( int id, KrImNode* node );

	#ifdef DEBUG
	void ValidateTree( KrImNode* root );
	#endif

  private:
	// Used to unroll recursion.
	struct StackContext
	{
		KrImNode*				node;
		bool					invalid;
		//GlInsideNode<KrImNode>*	childNode;
	};
	
	void CheckAllCollisionWalk( bool* hit, KrImNode* parent, KrImage* checkThis, std::vector<KrImage*>* outputArray, int window );

	// Before a draw, walk the tree and do necessary pre-processing.
	void Walk(	KrImNode* node, 
				bool invalid,	// once something in the tree is invalid, all of its children are invalid.
				bool visible,	// once something is invisible, all children are invisible
				int window );

	// Walk and call the draw routines.
	void DrawWalk( const grinliz::Rectangle2I& dr, KrImNode* node, KrPaintInfo*, int window );

	void Clear( KrImNode* root );	// delete the entire tree

	// Recursive hit test walk.
	bool HitTestRec( KrImNode* node, int x, int y, int flags, std::vector<KrImage*>* outputArray, int windowIndex );

	KrImNode*  root;			// The root to position the window
	KrImNode*  offsetRoot;		// The root as returned to the client
	KrEngine*  engine;

	std::map< U32, KrImNode* >			idMap;
	std::map< std::string, KrImNode* >	nameMap;

	int treeDepth;				// a z-depth used for opengl drawing
};


#endif
