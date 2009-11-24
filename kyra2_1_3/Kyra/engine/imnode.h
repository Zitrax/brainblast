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

#ifndef KYRA_IMAGE_NODE_INCLUDED
#define KYRA_IMAGE_NODE_INCLUDED

#include "../util/glinsidelist.h"
#include "SDL.h"
#include "krmath.h"
#include "color.h"
#include "kyrabuild.h"
#include "../../grinliz/glgeometry.h"

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <string>
#include <vector>


class KrEngine;
class KrImage;
class KrSprite;
class KrTile;
class KrCanvas;
class KrTextBox;
class KrBox;
class KrWidget;


const int KR_ALL_WINDOWS = -1;

/**	The base class of everything that can be in an ImageTree.
	The class in not abstract -- it can be very useful to
	have a KrImNode that is used as a container for other 
	objects.

	Children always draw on top of their parents.
*/
class KrImNode
{
	friend class KrImageTree;

  public:
	// The nodes:
	// Not really methods, not really data members. Here
	// but not to be used.
	GlInsideNode<KrImNode*>	treeNode;	// We are in someone else's child list.

  public:
	KrImNode();
	virtual ~KrImNode()				{}

	/**	Every node has a Z-Depth within the node that contains it.
		The lowest z-depth is the farthest from the viewer. Any 
		integer value is acceptable.

		Nodes at the same z-depth are sorted by the order they were
		added to the tree. The last one added is the closest to the
		viewer. (It is not a good idea to mix z-depth changing
		and "order sorting." Go with one or the other.

		Changing the z-depth of the root is meaningless. (It has
		no siblings.)
	*/
	int  ZDepth()					{ return depth; }

	/// Set the z-depth of this node relative to its siblings.
	void SetZDepth( int z );

	/// Return the x postion of this node, relative to its parent.
	int X( int window=0 )			{	GLASSERT( ( xTransform[window].x.v & 0xffff ) == 0 );
										return xTransform[window].x.ToInt(); }
	/// Return the y position of this node, relative to its parent.
	int Y( int window=0 )			{	GLASSERT( ( xTransform[window].y.v & 0xffff ) == 0 );
										return xTransform[window].y.ToInt(); }

	/// Return the x scale of this node, untransformed by parent.
	GlFixed XScale( int window=0 )				{ return xTransform[window].xScale; }
	/// Return the x scale of this node, untransformed by parent.
	GlFixed YScale( int window=0 )				{ return xTransform[window].yScale; }

	/// Query the entire transformation matrix.
	const KrMatrix2& Transform( int window=0 )	{ return xTransform[window]; }

	/**	Set the x and y positon of the ImNode, relative to 
		the node's parent.

		Even non-drawing nodes have position since changing the
		position moves the node's children.

		Setting the positon of the root node scrolls the screen.
	*/
	void SetPos( int x, int y, int window=KR_ALL_WINDOWS );

	/// Set the scale of this ImNode, which changes all the children as well.
	void SetScale( GlFixed xScale, GlFixed yScale, int window=KR_ALL_WINDOWS );

	/// Set the transformation matrix of this ImNode: combines the SetPos and SetScale functionality.
	void SetTransform( KrMatrix2& xForm, int window=KR_ALL_WINDOWS );

	/** Set the drawing quality for scaling. Quality is inherited
		from the parent the same way other properties are, if the
		quality is set to KrQualityNone, else the set quality is
		used.
		Cached scaling is always at maximum quality. 
	*/
	void SetQuality( int quality, int window=KR_ALL_WINDOWS );

	/// Offsets the node position.
	void DeltaPos( int deltaX, int deltaY, int window=KR_ALL_WINDOWS )		{ SetPos( X() + deltaX, Y() + deltaY, window ); }

	/** Get the color transformation of the node.
	*/
	const KrColorTransform& CTransform( int window=0 )			{ return cTransform[window]; }

	/** Set the color of a node. This color transforms all
		the node's children as well.
	*/
	virtual void SetColor( const KrColorTransform& color, int window=KR_ALL_WINDOWS );

	virtual KrImage*  ToImage()		{ return 0; }	///< Return a pointer if this is an image.
	virtual KrSprite* ToSprite()	{ return 0; }	///< Return a pointer if this is a sprite.
	virtual KrTile*   ToTile()		{ return 0; }	///< Return a pointer if this is a tile.
	virtual KrCanvas* ToCanvas()	{ return 0; }	///< Return a pointer if this is a canvas.
	virtual KrTextBox*ToTextBox()	{ return 0; }	///< Return a pointer if this is a text box.
	virtual KrBox*	  ToBox()		{ return 0; }	///< Return a pointer if this is a box.
	virtual KrWidget* ToWidget()	{ return 0; }	///< Return a pointer if this is a widget.

	/**	A generic cast. The Kyra library is not as extensible as it could be;
		this implements a "cheap RTTI" if you add your own sub-classes. Arbitrary
		casting to any type in the string. For example,
		@verbatim
			MyClassType* myType = (MyClassType*) node->ToExtended( "MyClassType" );
		@endverbatim
		Somewhat inelegant, but a very useful hook. The build in classes will not 
		respond to this. (That is, ToExtended( "Sprite" ) does not work.)
	*/
	virtual KrImNode* ToExtended( const std::string& type )	{ return 0; }

	/** Any ImNode can have a name or id. They are completely 
		managed by the user -- they are not used by the engine
		in any way. They should be unique to the tree, and can
		be used as a way to keep track of nodes.

		An empty name or an id<0 indicate no name and no id,
		respectively.
	*/
	const std::string& NodeName()		{ return nodeName; }

	/** Any ImNode can have a name or id. They are completely 
		managed by the user -- they are not used by the engine
		in any way. They should be unique to the tree, and can
		be used as a way to keep track of nodes.

		An empty name or an id<0 indicate no name and no id,
		respectively.
	*/
	int                NodeId()			{ return nodeId; }

	/** Set the name of this node. This is completely user defined.
		The name of a node should be unique. In DEBUG mode, an assertion
		will fire if the same name is used twice.
	*/
	void SetNodeName( const std::string& name );

	/** Set the id of this node. It must be > 0. This is completely user defined.
		Like the name, this should be unique. In DEBUG mode, an assertion
		will fire if an id is re-used.

		You can later find a node by the id you set by calling KrImageTree::FindNodeById().
	*/
	void SetNodeId( int id );	
	
	/// Fetch an iterator to walk the children with.
	GlInsideNodeIt<KrImNode*> ChildTreeIterator()	{ return GlInsideNodeIt<KrImNode*>( child ); }

	/// Hit test this object. Returns true if there was a hit, and 'this' will be added to results.
	virtual bool HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int window )  { return false; }
	
	/** Return the visibility of this node, taking into account	
		the state of all its parents.
	*/
	bool IsVisible( int window=0 );

	/** Return the visibility of this node, NOT taking into account	
		the state of all its parents.
	*/
	bool IsThisVisible( int window=0 )		{ return visible[window]; }

	/** If visible is set to false, this node and all its children
		will not show up on the stage.

		Returns true if the visiblity was changed by this call.
	*/
	virtual void SetVisible( bool visible, int window=KR_ALL_WINDOWS );

	/** Transform screen (pixel) coordinates to local object
		coordinates. The composite Xform is used for this
		and must be initialized -- practically, the object
		must be in a Tree to call this method.
	*/
	void ScreenToObject( int x, int y, grinliz::Vector2< GlFixed >* object, int window=0 );

	/// A hook to set user defined data.
	void SetUserData( void* data )				{ userData = data; }

	/// Fetches the user defined data.
	void* GetUserData()							{ return userData; }

	/** Return a copy if this object, not in a Tree(). It is
		return untransformed. In the case of a sprite, the action
		and frame will be set to the current value.	
	*/
	virtual KrImNode* Clone();

	// -- internal -- //
	// Will be true if this needs to be repainted.
	bool IsInvalid( int window )	{	GLASSERT( window >= 0 && window < KR_MAX_WINDOWS );
										return invalid[ window ]; }

	// Adds the invalid rectangles of this node the the 
	// engine's DR list and removes the node from the
	// start list. Called, with CalcTransform and
	// AddToDrawLists, by KrImageTree::Walk.
	// This relies on the current "bounds" for the invalid
	// rect, so it must be called before CalcTransform.
	//
	// If "cache" is true, no rectangle is sent, but it is stored.
	// Used to pair up overlapping rectangles.
	virtual void FlushInvalid( int window, bool cache )	{}

	// Calculate the:
	//	- Transformation, both color and spacial.
	//	- Bounds.
	// Note that the transformation will depend on the parent
	// already having been transformed, unless transformFromRoot
	// is true.
	virtual void CalcTransform( int win );

	// The node has been added to the tree: its links are valid.
	virtual void AddedtoTree();
	// The node is being removed from the tree.
	virtual void LeavingTree();

	void Invalidate( int window );

	// Properties of the node.
	// Were protected...but are sometimes useful to get to.
	// The transformation matrix, expressed as such.
	const KrMatrix2&		XTransform( int window )			{ return xTransform[window]; }
	// The composite (calculated absolute) transformation matrix.
	const KrMatrix2&		CompositeXForm( int window )		{ return compositeXForm[window]; }
	// The composite (calculated absolute) color transformation.
	const KrColorTransform& CompositeCForm( int window )		{ return compositeCForm[window]; }
	// The composite (calculated absolute) quality for scaling setting.
	int						CompositeQuality( int window )		{ return compositeQuality[window]; } 
	// The composite (calculated absolute) bounds. May be invalid.
	const grinliz::Rectangle2I&	CompositeBounds( int window )		{ return compositeBounds[window]; }

	// The bounds for this object; may be invalid.
	const grinliz::Rectangle2I&	Bounds( int window )						{ return bounds[window]; }
	// Get the child sentinel.
	GlInsideNode<KrImNode*>* Child() { return &child; }

	/// The parent of this KrImNode
 	KrImNode* Parent()								{ return parent; }
	/// The engine this KrImNode is in.
	KrEngine* Engine()								{ return engine; }

  protected:
	void ClearInvalid( int window )					{ invalid[window] = false; }

	/** Resort can be called if a node's depth has changed
		and it needs to move elsewhere in the list.
	*/
	void Resort( KrImNode* resortMe );

	// Sentinel for our child nodes
	GlInsideNode<KrImNode*> child;

	// Anything that can be drawn has bounds. Bounds are
	// calulated in the CalcTransform call.
	grinliz::Rectangle2I	bounds[KR_MAX_WINDOWS];

  private:

	int			depth;		// user-specified depth

	KrMatrix2			xTransform[KR_MAX_WINDOWS];		// (x,y) relative to parent (set)
	KrMatrix2			compositeXForm[KR_MAX_WINDOWS];	// (x,y) world (calculated)
	KrColorTransform	cTransform[KR_MAX_WINDOWS];		// color relative to parent
	KrColorTransform	compositeCForm[KR_MAX_WINDOWS];	// color world (calculated)
	int					quality[KR_MAX_WINDOWS];
	int					compositeQuality[KR_MAX_WINDOWS];
	grinliz::Rectangle2I compositeBounds[KR_MAX_WINDOWS];

	KrImNode*	parent;			// The parent in the ImageTree, only null for the root.
	KrEngine*	engine;			// The engine this Image is attached to.
	bool		invalid[KR_MAX_WINDOWS];		// Does this need be drawn?

	std::string nodeName;
	int         nodeId;
	bool		visible[KR_MAX_WINDOWS];
	void*		userData;
};


#endif

