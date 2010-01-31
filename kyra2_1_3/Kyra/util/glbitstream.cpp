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

#include "glbitstream.h"
#include "../../grinliz/gldebug.h"


int GlBitStream::BitsNeeded( U32 max )
{
	if ( max == 0 ) return 0;

	int bits = 0;
	while ( max >= U32( 1 << bits ) )
		++bits;

	return bits;
}


GlWriteBitStream::GlWriteBitStream( SDL_RWops* _fp )
{
	fp = _fp;
	accum = 0;
	bitsLeft = 8;
}


GlWriteBitStream::~GlWriteBitStream()
{
	Flush();
}


void GlWriteBitStream::WriteBits( U32 data, int nBitsInData )
{
	if ( nBitsInData == 0 )
		return;

	GLASSERT( U32( 1 << ( nBitsInData ) ) > data );

	while ( nBitsInData )
	{
		if ( nBitsInData <= bitsLeft )
		{
			accum |= data << ( bitsLeft - nBitsInData );
			bitsLeft -= nBitsInData;
			nBitsInData = 0;
			data = 0;
		}
		else
		{
			accum |= data >> ( nBitsInData - bitsLeft );

			nBitsInData -= bitsLeft;
			bitsLeft = 0;

			// Lop off the top of data
			U32 mask = 0xffffffff;
			mask >>= ( 32 - nBitsInData );
			data &= mask;
		}

		if ( bitsLeft == 0 )
		{
			//fputc( (int) accum, fp );
			SDL_RWwrite( fp, &accum, 1, 1 );
			accum = 0;
			bitsLeft = 8;
		}
	}
	GLASSERT( data == 0 );
}


void GlWriteBitStream::Flush()
{
	if ( bitsLeft != 8 )
	{
		//fputc( (int) accum, fp );
		SDL_RWwrite( fp, &accum, 1, 1 );
		accum = 0;
		bitsLeft = 8;
	}
}


GlReadBitStream::GlReadBitStream( SDL_RWops* _fp )
{
	fp = _fp;

	bitsLeft = 0;
	accum = 0;
}


U32 GlReadBitStream::ReadBits( int nBitsInData )
{
	if ( nBitsInData == 0 )
		return 0;

	U32 val = 0;

	while ( nBitsInData )
	{
		if ( bitsLeft == 0 )
		{
			bitsLeft = 8;
			//accum = fgetc( fp );
			SDL_RWread( fp, &accum, 1, 1 );
		}

		if ( nBitsInData <= bitsLeft )
		{
			val |= accum >> ( bitsLeft - nBitsInData );

			bitsLeft -= nBitsInData;
			nBitsInData = 0;

			// Trim the accumulator
			U32 mask = 0xff;
			mask >>= ( 8 - bitsLeft );
			accum &= mask;

		}
		else
		{
			// There are more bits needed than what is in
			// the accumulator

			val |= accum << ( nBitsInData - bitsLeft );

			nBitsInData -= bitsLeft;
			bitsLeft = 0;
		}
	}
	return val;
}

void GlReadBitStream::Flush()
{
	bitsLeft = 0;
	accum = 0;
}

