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

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>

#endif	//_WIN32

#include "gldebug.h"



#ifdef _WIN32

void WinDebugBreak()
{
	DebugBreak();
}

void dprintf( const char* format, ... )
{
    va_list     va;
    char		buffer[1024];

    //
    //  format and output the message..
    //
    va_start( va, format );
    vsprintf( buffer, format, va );
    va_end( va );

    OutputDebugString( buffer );

	printf( "%s", buffer );
	fflush( 0 );
}
#endif
