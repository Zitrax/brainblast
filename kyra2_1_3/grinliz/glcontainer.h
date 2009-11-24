#ifndef GRINLIZ_CONTAINER_INCLUDED
#define GRINLIZ_CONTAINER_INCLUDED

#include "gldebug.h"

namespace grinliz
{

template< typename T, typename Equal >
class PtrHashTable
{
public:
	PtrHashTable( int _size = 256 ) : size( 0 ), population( 0 ), table( 0 )
	{
		Resize( _size );
	}

	~PtrHashTable() { delete [] table; }

	void Clear()
	{
		memset( table, 0, size*sizeof(T*) );
		population = 0;
	}

	void Put( T* t ) 
	{
		if ( population > size / 2 ) {
			GLOUTPUT(( "PtrHashTable resize %d to %d\n", size, size*4 ));
			Resize( size*4 );
		}

		U32 hash = t->HashCode();
		unsigned index = hash % size;
		GLASSERT( index < size );

		// Linear probe
		while( table[index] ) {
			++index;
			if ( index == size )
				index = 0;
		}
		table[index] = t;
		++population;
	}

	T* Contains( const T& key ) 
	{
		U32 hash = key.HashCode();
		unsigned index = hash % size;
		while( table[index] ) 
		{
			Equal equal;
			if ( equal( *(table[index]), key ) )
				return table[index];

			++index;
			if ( index == size )
				index = 0;
		}
		return 0;
	}

	void Resize( unsigned newSize ) 
	{
		if ( newSize < population / 4 )
			newSize = population*4;

		T** oldTable = table;
		unsigned oldSize = size;

		size = newSize;
		table = new T*[size];
		Clear();

		for( unsigned i=0; i<oldSize; ++i ) 
		{
			if ( oldTable[i] )
				Put( oldTable[i] );
		}
		delete [] oldTable;
	}

private:
	unsigned size;
	unsigned population;
	T** table;
};


}	// namespace grinliz
#endif