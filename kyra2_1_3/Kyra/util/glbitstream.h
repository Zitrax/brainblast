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

#ifndef BITSTREAMER_INCLUDED
#define BITSTREAMER_INCLUDED

#include "../../grinliz/gltypes.h"
#include "SDL_rwops.h"


class GlBitStream
{
  public:
	static int BitsNeeded( U32 maxValue );

};

class GlWriteBitStream : public GlBitStream
{
  public:
	GlWriteBitStream( SDL_RWops* fp );
	~GlWriteBitStream();

	void WriteBits( U32 data, int nBitsInData );
	void Flush();

  private:
	SDL_RWops* fp;

	U8 accum;
	int bitsLeft;
};


class GlReadBitStream : public GlBitStream
{
  public:
	GlReadBitStream( SDL_RWops* fp );
	~GlReadBitStream()				{}

	U32 ReadBits( int nBitsInData );
	void Flush();

  private:
	SDL_RWops* fp;
	int bitsLeft;
	U8 accum;
};

#endif
