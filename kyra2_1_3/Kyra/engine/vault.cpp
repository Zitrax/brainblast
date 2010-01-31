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


#include <time.h>
#include "sdlutil.h"
#include "SDL.h"
#include "vault.h"
#include "SDL_rwops.h"
#include "SDL_endian.h"
#include "pixelblock.h"
#include "../../grinliz/glperformance.h"

using namespace grinliz;

KrResourceVault::~KrResourceVault()
{
	while( !resList.Empty() )
	{
		KrResource* resource = resList.Front();
		delete resource;
		resList.PopFront();
	}

	delete memoryPoolLine;
	delete memoryPoolRGBA;
	delete memoryPoolSegment;
}


bool KrResourceVault::LoadDatFile( const char* fileName )
{
	//GLOUTPUT( "LoadDatFile(filename) called\n" );
	SDL_RWops* data = SDL_RWFromFile( fileName, "rb" );
	if ( !data )
	{
		GLOUTPUT((	"KrResourceVault::LoadDatFile failed to open file '%s'\n",fileName ));
		return false;
	}
	bool success = LoadDatFile( data );
	SDL_RWclose( data );
	//SDL_FreeRW( data );;
	return success;
}


bool KrResourceVault::LoadDatFileFromMemory( void* memory, int size )
{
	SDL_RWops* data = SDL_RWFromMem( memory, size );
	if ( !data )
	{
		GLOUTPUT(( "KrResourceVault::LoadDatFileFromMemory failed.\n" ));
		return false;
	}
	bool success = LoadDatFile( data );
	SDL_RWclose( data );
	//SDL_FreeRW( data );;
	return success;
}


bool KrResourceVault::LoadDatFile( SDL_RWops* data )
{
 	char magic[ 5 ];
	std::string version;
	bool ret = false;

	//GLOUTPUT( "LoadDatFile called\n" );
	// Can only load 1 dat file.
	GLASSERT( memoryPoolRGBA == 0 );
	GLASSERT( memoryPoolLine == 0 );
	GLASSERT( memoryPoolSegment == 0 );

	KrResourceFactory* factory = KrResourceFactory::Instance();
	GLASSERT( factory );

	if ( data && factory )
	{
		// Read the magic and the version.
		SDL_RWread( data, magic, 4, 1 );
		magic[4] = 0;

		GLASSERT( magic[0] == 'K' );
		GLASSERT( magic[1] == 'Y' );
		GLASSERT( magic[2] == 'R' );
		GLASSERT( magic[3] == 'A' );

		ReadString( data, &version );

		GLOUTPUT(( "File magic=%s version=%s\n", magic, version.c_str() ));

		U32 numRGBA    = SDL_ReadLE32( data );
		U32 numLine    = SDL_ReadLE32( data );
		U32 numSegment = SDL_ReadLE32( data );

		GLOUTPUT(( "Tally count: rgba=%d line=%d segment=%d\n", numRGBA, numLine, numSegment  ));

		memoryPoolRGBA    = new grinliz::LinearMemoryPool( sizeof( KrRGBA ) * numRGBA );
		memoryPoolLine    = new grinliz::LinearMemoryPool( sizeof( KrRleLine ) * numLine );
		memoryPoolSegment = new grinliz::LinearMemoryPool( sizeof( KrRleSegment ) * numSegment );

		KrRle::SetMemoryPools( memoryPoolRGBA, memoryPoolLine, memoryPoolSegment );

		U32 id = 0;
		U32 size = 0;
		U32 next = 0;

		for ( ;; )
		{
			GLOUTPUT(( "here=%d ", SDL_RWtell( data ) ));
			id   = SDL_ReadLE32( data );
			size = SDL_ReadLE32( data );
			next = size + SDL_RWtell( data );
			GLOUTPUT(( "id=%d size=%d next=%d\n", id, size, next ));

			if ( id == KYRATAG_END )
			{
				break;
			}
			else
			{
				KrResource* resource;
				resource = factory->Create( id, size, data );

				if ( resource )
				{
					resList.PushBack( resource );
				}
				else
				{
					break;
				}
			}
		}
		ret = true;
	}
	delete factory;

	// We should be out of memory, since we calculated how much we need:
	#ifdef DEBUG
		GLOUTPUT(("Num RGBA: RLE: %d Block: %d total=%d\n",
				  KrRleSegment::numRGBA, KrPixelBlock::numRGBA,
				  KrRleSegment::numRGBA + KrPixelBlock::numRGBA ));
		GLASSERT( KrRle::PoolOut() );	
	#endif
//	KrRleSegment::SetMemoryPool( 0 );
//	KrPixelBlock::SetMemoryPool( 0 );
	KrRle::SetMemoryPools( 0, 0, 0 );

	return ret;
}


GlSListIterator< KrResource* > KrResourceVault::GetResourceIterator()
{
	GlSListIterator< KrResource* > it( resList );
	it.Begin();
	return it;
}


int KrResourceVault::ResourceCount()
{
	return resList.Size();
}


void KrResourceVault::AddResource( KrResource* resource )
{
	resList.PushBack( resource );
}


KrResource* KrResourceVault::GetResource( const std::string& type, const std::string& name )
{
	// OPT linear search
	GlSListIterator< KrResource* > it( resList );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		if (	it.Current()->TypeName() == type
			 && it.Current()->ResourceName() == name )
		{
			return it.Current();
		}
	}
	return 0;
}


KrResource* KrResourceVault::GetResource( U32 type, const std::string& name )
{
	// OPT linear search
	GlSListIterator< KrResource* > it( resList );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		if (	it.Current()->Type() == type
			 && it.Current()->ResourceName() == name )
		{
			return it.Current();
		}
	}
	return 0;
}


KrResource* KrResourceVault::GetResource( U32 type, U32 resid )
{
	// OPT linear search
	GlSListIterator< KrResource* > it( resList );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		if (	it.Current()->Type() == type
			 && it.Current()->ResourceId() == resid )
		{
			return it.Current();
		}
	}
	return 0;
}


KrSpriteResource* KrResourceVault::GetSpriteResource( const std::string& name )
{
	KrResource* resource = GetResource( KYRATAG_SPRITE, name );
	if ( resource )
	{	
		GLASSERT( resource->ToSpriteResource() );
		return resource->ToSpriteResource();
	}
	return 0;
}


KrSpriteResource* KrResourceVault::GetSpriteResource( U32 id )
{
	KrResource* resource = GetResource( KYRATAG_SPRITE, id );
	if ( resource )
	{	
		GLASSERT( resource->ToSpriteResource() );
		return resource->ToSpriteResource();
	}
	return 0;
}


KrTileResource*	KrResourceVault::GetTileResource( const std::string& name )
{
	KrResource* resource = GetResource( KYRATAG_TILE, name );
	if ( resource )
	{	
		GLASSERT( resource->ToTileResource() );
		return resource->ToTileResource();
	}
	return 0;
}


KrTileResource*	KrResourceVault::GetTileResource( U32 id )
{
	KrResource* resource = GetResource( KYRATAG_TILE, id );
	if ( resource )
	{	
		GLASSERT( resource->ToTileResource() );
		return resource->ToTileResource();
	}
	return 0;
}

KrFontResource*	KrResourceVault::GetFontResource( const std::string& name )
{
	KrResource* resource = GetResource( KYRATAG_FONT, name );
	if ( resource )
	{	
		GLASSERT( resource->ToFontResource() );
		return resource->ToFontResource();
	}
	return 0;
}


KrFontResource*	KrResourceVault::GetFontResource( U32 id )
{
	KrResource* resource = GetResource( KYRATAG_FONT, id );
	if ( resource )
	{	
		GLASSERT( resource->ToFontResource() );
		return resource->ToFontResource();
	}
	return 0;
}


KrTextDataResource*	KrResourceVault::GetTextDataResource( U32 id )
{
	KrResource* resource = GetResource( KYRATAG_TEXTDATA, id );
	if ( resource )
	{	
		GLASSERT( resource->ToTextDataResource() );
		return resource->ToTextDataResource();
	}
	return 0;
}

KrTextDataResource*	KrResourceVault::GetTextDataResource( const std::string& name )
{
	KrResource* resource = GetResource( KYRATAG_TEXTDATA, name );
	if ( resource )
	{	
		GLASSERT( resource->ToTextDataResource() );
		return resource->ToTextDataResource();
	}
	return 0;
}

KrBinaryDataResource* KrResourceVault::GetBinaryDataResource( U32 id )
{
	KrResource* resource = GetResource( KYRATAG_BINARYDATA, id );
	if ( resource )
	{	
		GLASSERT( resource->ToBinaryDataResource() );
		return resource->ToBinaryDataResource();
	}
	return 0;
}

KrBinaryDataResource* KrResourceVault::GetBinaryDataResource( const std::string& name )
{
	KrResource* resource = GetResource( KYRATAG_BINARYDATA, name );
	if ( resource )
	{	
		GLASSERT( resource->ToBinaryDataResource() );
		return resource->ToBinaryDataResource();
	}
	return 0;
}


void KrResourceVault::CacheScale( GlFixed xScale, GlFixed yScale )
{
	GlSListIterator< KrResource* > it( resList );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		it.Current()->CacheScale( xScale, yScale );
	}
}


void KrResourceVault::FreeScaleCache()
{
	GlSListIterator< KrResource* > it( resList );

	for( it.Begin(); !it.Done(); it.Next() )
	{
		it.Current()->FreeScaleCache();
	}
}
