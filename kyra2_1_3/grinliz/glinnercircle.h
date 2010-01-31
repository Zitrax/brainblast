#ifndef GRINLIZ_INNER_CIRCLE_INCLUDED
#define GRINLIZ_INNER_CIRCLE_INCLUDED

namespace grinliz
{

template<class T>
class InnerCircle
{
  public:
	enum Sentinel
	{
		SENTINEL
	};

	InnerCircle( Sentinel ) {
		next = prev = this;
		container = 0;
	} 

	InnerCircle( T* container )	{ 
		GLASSERT( container );
		this->container = container;
		next = prev = 0;
	}

	~InnerCircle() {
		if ( !Sentinel() )
			Remove();
		container = 0;	// silly, but useful for debugging
	}

	void Add( InnerCircle* addThis )
	{
		GLASSERT( addThis->next == 0 );
		GLASSERT( addThis->prev == 0 );
		GLASSERT( addThis->Sentinel() == false );

		if ( next )
		{
			next->prev = addThis;
			addThis->next = next;
		}
		next = addThis;
		addThis->prev = this;
	}

	void Remove() {
		GLASSERT( ( prev && next ) || (!prev && !next ) );
		GLASSERT( !Sentinel() );
		if ( prev ) {
			prev->next = next;
			next->prev = prev;
			prev = next = 0;
		}
	}

	void RemoveAndDelete() {
		Remove();
		delete container;
	}

	bool InList() {
		GLASSERT( !Sentinel() );
		GLASSERT( ( prev && next ) || (!prev && !next ) );
		return prev != 0;
	}

	T* Container()			{ return container; }
	InnerCircle<T>* Next()	{ return next; }
	InnerCircle<T>* Prev()	{ return prev; }
	bool Sentinel()			{ return !container; }

  private:
	InnerCircle *next, *prev;
	T* container;
};


};	// namespace grinliz

#endif
