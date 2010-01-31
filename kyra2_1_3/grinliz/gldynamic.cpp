#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "SDL_loadso.h"
#include "SDL_error.h"

#include <string>

#include "gldynamic.h"
#include "gldebug.h"


using namespace std;

void* grinliz::GL_LoadLibrary( const char* name )
{
	string libraryName;

	#if defined( _WIN32 )
	libraryName = name;
	#elif defined(__APPLE__)
	libraryName = "lib";
	libraryName += name;
	libraryName += ".dylib";
	#else
	libraryName = "lib";
	libraryName += name;
	libraryName += ".so";
	#endif

	#if defined (__APPLE__)
	// SDL doesn't work at all for MACX
	handle = dlopen( libraryName.c_str(), RTLD_NOW );
	if ( !handle ) {
		// try again, with the special place for many dylibs...
		string alt = "/usr/local/lib/";
		alt += libraryName;
		handle = dlopen( alt.c_str(), RTLD_NOW );
	}
	#else
	void* handle = SDL_LoadObject( libraryName.c_str() );
	#endif

	if ( !handle ) {
		#if defined (__APPLE__)
		GLLOG(( "ERROR: could not load %s. Reason: %s\n", libraryName.c_str(), dlerror() ));
		#else
		GLLOG(( "ERROR: could not load %s. Reason: %s\n", libraryName.c_str(), SDL_GetError() ));
		#endif
	}
	return handle;
}


void* grinliz::GL_LoadFunction( void* handle, const char* functionName )
{
	void* func = 0;

	#ifdef __APPLE__
	func = dlsym( handle, functionName );
	#else
	func = SDL_LoadFunction( handle, functionName );
	#endif

	return func;
}
