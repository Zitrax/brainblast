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

#ifndef INPLACE_IMAGENODE_INCLUDED
#define INPLACE_IMAGENODE_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "../../grinliz/gldebug.h"

// The type of this must be a pointer.
template <class T>
class GlInsideNode
{
  public:
	/// Constructs a sentinel node.
	GlInsideNode()					{ next = this; prev = this; data = 0; }

	/// If image is null, this will be a sentinel node.
	GlInsideNode( T _data )			{ next = this; prev = this; data = _data; }

	virtual ~GlInsideNode()			{}

	bool IsSentinel() const			{ return !data; }
	bool InList() const				{ return !(( next == this ) && ( prev == this )); }

	/// Insert addMe before this.
	void InsertBefore( GlInsideNode<T>* addMe )
	{
		GLASSERT( !addMe->IsSentinel() );
		addMe->prev = prev;
		prev->next = addMe;
		prev = addMe;
		addMe->next = this;
	}

	/// Insert addMe after this.
	void InsertAfter( GlInsideNode<T>* addMe )
	{
		GLASSERT( !addMe->IsSentinel() );
		addMe->prev = this;
		addMe->next = next;
		next->prev = addMe;
		next = addMe;
	}

	/// Take this node out of the list
	void Remove()
	{
		prev->next = next;
		next->prev = prev;
		prev = next = this;		// assume sentinel, again.
	}
	
	// Should be private, but I don't feel like fighting with
	// making templates friends.

	GlInsideNode<T>*	next;
	GlInsideNode<T>*	prev;
	T					data;
};


template <class T>
class GlInsideNodeIt
{
  public:
	GlInsideNodeIt( GlInsideNode<T>& _sentinel )	 
		: sentinel( &_sentinel ), current( 0 ) 
	{ 
		GLASSERT( sentinel->IsSentinel() ); 
	}

	GlInsideNode<T>*	CurrentNode()						{ return current; }
	T					CurrentData()						{ return current->data; }
	void				SetCurrent( GlInsideNode<T>* c )	{ current = c; }

	void Begin()	{ current = sentinel->next; }
	void Last()		{ current = sentinel->prev; }
	void Next()		{ current = current->next; }
	void Prev()		{ current = current->prev; }
	bool Done()		{ return current->IsSentinel(); }
		
	void InsertBefore( GlInsideNode<T>& addMe )	{ current->InsertBefore( &addMe ); }
	void InsertAfter(  GlInsideNode<T>& addMe )	{ current->InsertAfter( &addMe ); }

  private:
	GlInsideNode<T>*	sentinel;
	GlInsideNode<T>*	current;
};


#endif
