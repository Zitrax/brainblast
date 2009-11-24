/*
Copyright (c) 2000-2003 Lee Thomason (www.grinninglizard.com)

Grinning Lizard Utilities. Note that software that uses the 
utility package (including Lilith3D and Kyra) have more restrictive
licences which applies to code outside of the utility package.


This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/

#ifndef KYRA_LIST_INCLUDED
#define KYRA_LIST_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "../../grinliz/gldebug.h"


/** A node of a single linked list. Used by the GlSList template.
*/
template <class T>
struct GlSListNode
{
	GlSListNode* next;	
	T data;				///< The template data.
};


/**	A template for a singly-linked list, of the simplest sort. Uses
	GlSListNodes for members of the list, and can be walked with 
	the GlSListIterator.
*/
template <class T>
class GlSList
{
  public:
	GlSList()		{ root = 0; }
	~GlSList()		{ Clear(); }

	/// The number of items in the list -- O(n) performance!
	int  Size() const		{ return Count(); }
	/// The number of items in the list -- O(n) performance!
	int  Count() const		{	GlSListNode<T>* node;
								int count = 0;
								for( node = root; node; node = node->next )
									++count;
								return count;
							}
	/// A very fast check for an empty list.
	bool Empty() const		{ return root == 0; }
	/// Returns the data at the front of the list.
	T&	 Front() const		{ return root->data; }
	/// Get the node, not just the data, at the front of the list.
	GlSListNode<T>*	 FrontNode() const		{ return root; }

	/// Deletes all the items in the list.
	void Clear()	{	GlSListNode<T>* temp;

						while( root )
						{
							temp = root;
							root = root->next;
							delete temp;
						}
					}

	/// Adds an item (by copy) to the list. (Fast)
	void PushFront( const T& insert )
					{
						GlSListNode<T>* node = new GlSListNode<T>;
						node->data = insert;
						node->next = root;
						root = node;
					}

	/// Adds an item (by copy) to the list. (Slower, since it must seek to the end.)
	void PushBack( const T& insert )
	{
		GlSListNode<T>* end;
		for ( end=root; end && end->next; end = end->next )
		{}

		if ( !end )
		{
			PushFront( insert );
		}
		else
		{
			GlSListNode<T>* node = new GlSListNode<T>;
			node->data = insert;
			node->next = 0;
			end->next = node;
		}
	}

	/** Pull off the root entry in the list.
	*/
	void PopFront()
	{
		if ( root )
		{
			GlSListNode<T>* temp = root->next;
			delete root;
			root = temp;
		}
	}

	/**	Delete an arbitrary element. If elements are repeated,
		removes the first instance.
	*/
	void Pop( const T& thisone )
	{
		GlSListNode<T>* node;
		GlSListNode<T>* prev;

		for( node = root, prev = 0;
			 node;
			 prev = node, node = node->next )
		{
			if ( node->data == thisone )
			{
				if ( prev )
				{
					prev->next = node->next;
				}
				else
				{
					root = node->next;
				}
				delete node;
				break;
			}
		}
	}

	/// Find the first occurance of node in the list. Linear search.
	GlSListNode<T>* Find( const T& findthis )
	{
		GlSListNode<T>* node;
		for( node=root; node; node=node->next )
		{
			if ( node->data == findthis )
				return node;
		}	
		return 0;
	}
		
	/// Find and delete instance if found. Do nothing if find fails. Return if delete.
	bool FindAndDelete( const T& findthis )
	{
		GlSListNode<T>* node;
		GlSListNode<T>* prev;

		for(	node=root, prev = 0; 
				node; 
				prev = node, node=node->next )
		{
			if ( node->data == findthis )
			{
				if ( prev )
					prev->next = node->next;
				else
					root = node->next;
				delete node;
				return true;
			}
		}	
		return false;
	}


  private:
	GlSListNode<T>* root;
	GlSList( const GlSList& that );
	GlSList operator=( const GlSList& that );
};


/** An iterator to walk a singly linked list.
*/
template <class T>
class GlSListIterator
{
  public:
	GlSListIterator( const GlSList<T>& _list ) : current( 0 ), list( &_list )	{}

	void Begin()		{ current = list->FrontNode(); }
	void Next()			{ current = current->next; }
	bool Done()			{ return ( current == 0 ); }

	/// Return a reference to the data - will crash horribly if one does not exist.
	T& Current()					{ return (current->data); }

	GlSListNode<T>* CurrentNode()	{ return current; }

  private:
	GlSListNode<T>* current;	
	const GlSList<T>* list;
};


#endif
