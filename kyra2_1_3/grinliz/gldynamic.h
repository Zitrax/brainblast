#ifndef GRINLIZ_DYNAMIC_INCLUDED
#define GRINLIZ_DYNAMIC_INCLUDED

namespace grinliz
{
// A routine to try to get the SDL LoadObject to actually work on the platforms
// supported. (Win, Mac, Linux).
void* GL_LoadLibrary( const char* libraryName );
void* GL_LoadFunction( void* handle, const char* functionName );
};

#endif
