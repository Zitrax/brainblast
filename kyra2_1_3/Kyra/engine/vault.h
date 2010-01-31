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


#ifndef IO_COMPRESSOR_INCLUDED
#define IO_COMPRESSOR_INCLUDED

#define RLE_WRITE

#include "SDL.h"
#include "SDL_rwops.h"

#include "../../grinliz/glutil.h"
#include "../util/gllist.h"
//#include "../util/glstring.h"

#include "rle.h"
#include "kyraresource.h"

class KrResource;
class KrSpriteResource;
class KrTileResource;
class KrFontResource;

/**	The vault is a container for resources. The sequence of
	steps generally followed is:
	
	- A vault is created and resources are read from a stream. 
	- Nhe engine is created, used, and destroyed.
	- The vault is deleted.

	It would be bad to delete resources the engine is using.

	Note that the engine has a "default vault" Engine()->Vault() 
	that it manages for you. It is usually simpler to use that
	Vault rather than manage your own.
*/
class KrResourceVault
{
  public:
	KrResourceVault()			{ memoryPoolRGBA = 0; memoryPoolLine = 0; memoryPoolSegment = 0; }
	virtual ~KrResourceVault();

	/**	Load in a .dat file from disk. Only one dat files can be
		loaded into a vault; but the engine can use multiple vaults.
	*/
	bool LoadDatFile( const char* fileName );

	/**	Load in a .dat file from disk. Only one dat files can be
		loaded into a vault; but the engine can use multiple vaults.
	*/
	bool LoadDatFileFromMemory( void* memory, int size );

	/// How many resources are in the vault?
	int ResourceCount();
	/** Get an interator that can walk all the resources in this vault.
		Useful if you want to list resources, for instance if making
		an editor or allowing the user to choose between available fonts.
	*/
	GlSListIterator< KrResource* > GetResourceIterator();

	/// Add user created resources - generally canvases. Will be owned and deleted by the vault.
	void AddResource( KrResource* resource );

	/// Return a pointer to a resource. Null if the resource isn't found.
	KrResource* GetResource( const std::string& type, const std::string& name );
	/// Return a pointer to a resource. Null if the resource isn't found.
	KrResource* GetResource( U32 type, const std::string& name );
	/// Return a pointer to a resource. Null if the resource isn't found.
	KrResource* GetResource( U32 type, U32 resourceId );

	/// Return a pointer to a resource. Null if the resource isn't found.
	KrSpriteResource*	GetSpriteResource( const std::string& name );
	/// Return a pointer to a resource. Null if the resource isn't found.
	KrSpriteResource*	GetSpriteResource( U32 resourceId );

	/// Return a pointer to a resource. Null if the resource isn't found.
	KrTileResource*		GetTileResource( const std::string& name );
	/// Return a pointer to a resource. Null if the resource isn't found.
	KrTileResource*		GetTileResource( U32 resourceId );

	/// Return a pointer to a resource. Null if the resource isn't found.
	KrFontResource*		GetFontResource( const std::string& name );
	/// Return a pointer to a resource. Null if the resource isn't found.
	KrFontResource*		GetFontResource( U32 resourceId );

	/// Return a pointer to a resource. Null if the resource isn't found.
	KrTextDataResource*		GetTextDataResource( const std::string& name );
	/// Return a pointer to a resource. Null if the resource isn't found.
	KrTextDataResource*		GetTextDataResource( U32 resourceId );

	/// Return a pointer to a resource. Null if the resource isn't found.
	KrBinaryDataResource*	GetBinaryDataResource( const std::string& name );
	/// Return a pointer to a resource. Null if the resource isn't found.
	KrBinaryDataResource*	GetBinaryDataResource( U32 resourceId );

	/// For cached scaling, cache all the resources in this vault.
	virtual void CacheScale( GlFixed xScale, GlFixed yScale );
	/// For cached scaling, free all the scale caches in this vault.
	virtual void FreeScaleCache();

  private:
	bool LoadDatFile( SDL_RWops* stream );

	GlSList< KrResource* > resList;

	// The vault is the owner of these memory allocators:
	grinliz::LinearMemoryPool*	memoryPoolRGBA;
	grinliz::LinearMemoryPool*	memoryPoolLine;
	grinliz::LinearMemoryPool*	memoryPoolSegment;
};
	
	

#endif
