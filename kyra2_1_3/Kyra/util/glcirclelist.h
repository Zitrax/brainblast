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


#ifndef KYRA_CIRCLELIST_INCLUDED
#define KYRA_CIRCLELIST_INCLUDED

#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "../../grinliz/gldebug.h"

// OPT improvements:
// - add memory allocator
// - remove the 'data' from circle node, so the overhead isn't in
//	 the sentinels.

template <class T>
struct GlCircleNode
{
	T data;

	GlCircleNode<T>* next;
	GlCircleNode<T>* prev;
};

/*
	A circular, double linked list.
*/
template <class T>
class GlCircleList
{
  public:
	GlCircleList()		{ sentinel.next = &sentinel; sentinel.prev = &sentinel; }
	~GlCircleList()		{ Clear(); }

	bool	Empty() const	{ return sentinel.next == &sentinel; }
	T&		Front() const	{ return sentinel.next->data; }
	T&		Back()  const	{ return sentinel.prev->data; }
	GlCircleNode<T>* FrontNode() const	{ return sentinel.next; }
	GlCircleNode<T>* BackNode()  const	{ return sentinel.prev; }

	void Clear()			{	GlCircleNode<T>* temp;
								while( sentinel.next != &sentinel )
								{
									temp = sentinel.next;
									sentinel.next = sentinel.next->next;
									delete temp;
								}
								sentinel.prev = &sentinel;
							}
	void PushFront( const T& insert ) {
								GlCircleNode<T>* node = new GlCircleNode<T>;
								node->data = insert;

								node->prev = &sentinel;
								node->next = sentinel.next;
								sentinel.next->prev = node;
								sentinel.next = node;
							}
	void PushBack( const T& insert ) {
								GlCircleNode<T>* node = new GlCircleNode<T>;
								node->data = insert;

								node->prev = sentinel.prev;
								node->next = &sentinel;
								sentinel.prev->next = node;
								sentinel.prev = node;
							}
	void PopFront()			{
								GLASSERT( sentinel.next != &sentinel );
								GlCircleNode<T>* node = sentinel.next;
// 								node->prev->next = node->next;
// 								node->next->prev = node->prev;
// 								delete node;
								Delete( node );
							}
	void PopBack()			{
								GLASSERT( sentinel.prev != &sentinel );
								GlCircleNode<T>* node = sentinel.prev;
// 								node->prev->next = node->next;
// 								node->next->prev = node->prev;
// 								delete node;
								Delete( node );
							}

	void Delete( GlCircleNode<T>* node )	{
												GLASSERT( node != &sentinel );
												node->prev->next = node->next;
												node->next->prev = node->prev;
												delete node;
											}
	GlCircleNode<T>* Find( T value )		{
												GlCircleNode<T>* node = sentinel.next;
												while ( node != &sentinel )
												{
													if ( node->data == value )
														return node;
													node = node->next;
												}
												return 0;
											}

	// Scoping problems. Pretend this is private.
	GlCircleNode<T> sentinel;
};


template <class T>
class GlCircleListIterator
{
  public:
	GlCircleListIterator( GlCircleList<T>& _list ) : current( 0 ), list( &_list )	{}

	void Begin()		{ current = list->sentinel.next; }
	void Next()			{ current = current->next; }
	void Prev()			{ current = current->prev; }
	bool Done()			{ return ( current == &(list->sentinel) ); }

	/// Return a reference to the data - will crash horribly if one does not exist.
	T& Current()					{ return (current->data); }

	void InsertBefore( const T& addMe )
	{
		GlCircleNode<T>* node = new GlCircleNode<T>;
		node->data = addMe;

		node->prev = current->prev;
		node->next = current;
		current->prev->next = node;
		current->prev = node;
	}

	void InsertAfter( const T& addMe )
	{
		GlCircleNode<T>* node = new GlCircleNode<T>;
		node->data = addMe;

		node->prev = current;
		node->next = current->next;
		current->next->prev = node;
		current->next = node;
	}

	void Remove()
	{
		GLASSERT( current != &(list->sentinel) );

		GlCircleNode<T>* temp = current;
		current = current->next;

		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		delete temp;
	}

  private:
	GlCircleNode<T>* current;	
	GlCircleList<T>* list;
};


#endif
