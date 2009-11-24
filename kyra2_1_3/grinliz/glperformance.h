/*
Copyright (c) 2000-2003 Lee Thomason (www.grinninglizard.com)
Grinning Lizard Utilities.

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


#ifndef GRINLIZ_PERFORMANCE_MEASURE
#define GRINLIZ_PERFORMANCE_MEASURE

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "gltypes.h"
#include "gldebug.h"

namespace grinliz {

const int GL_MAX_PROFILE_DATAITEM = 64;

struct PerformanceData
{
	PerformanceData( const char* name );

	const char* name;
	U32 count;
	U64 totalTime;
};


struct ProfileDataItem
{
	const char* name;
	U32 count;			// # of calls
	U32 totalTime;		// total time - in no particular unit (multiple of clock cycle)
};

struct ProfileData
{
	U32 totalTime;		// total time of all items - no particular unit
	U32 count;			// number of items
	grinliz::ProfileDataItem item[ GL_MAX_PROFILE_DATAITEM ];
};


#ifdef _MSC_VER
	inline U64 FastTime()
	{
		union 
		{
			U64 result;
			struct
			{
				U32 lo;
				U32 hi;
			} split;
		} u;
		u.result = 0;

		_asm {
			//pushad;	// don't need - aren't using "emit"
			cpuid;		// force all previous instructions to complete - else out of order execution can confuse things
			rdtsc;
			mov u.split.hi, edx;
			mov u.split.lo, eax;
			//popad;
		}				
		return u.result;
	}

#else
	inline U64 FastTime()
	{
		#ifdef __GNUC__
			U64 val;
    		 __asm__ __volatile__ ("rdtsc" : "=A" (val));
    		 return val;
    	#else
			return SDL_GetTicks();
		#endif
	}
#endif

/**
	Used to automatically track performance of blocks of code. Should
	be used to measure code that is blocked out like this:

	@verbatim
	#ifdef L3PERF
	static PerformanceData data( "L3TerrainMesh_Stream" );
	Performance perf( &data );
	#endif
	@endverbatim
*/
class Performance
{
	friend struct PerformanceData;
  public:

	Performance( PerformanceData* data )	{
		this->data = data;
		++data->count;
		start = FastTime();
	}

	~Performance()
	{
		U64 end = FastTime();
		GLASSERT( end >= start );
		data->totalTime += ( end - start );
	}

	/// Write the results of performance testing to a file.
    static void Dump( FILE* fp, const char* desc );
	/// Get the profiling data is a useable format.  
	static const grinliz::ProfileData& GetData();
	/// Reset the profiling data.
	static void Clear();
	/// Sort the data on total time.
	static const grinliz::ProfileData& GetSortedData();


  protected:

	static PerformanceData* map[ GL_MAX_PROFILE_DATAITEM ];
	static ProfileData	  profile;
	static ProfileData	  sortedProfile;
	static int numMap;

	PerformanceData* data;
	U64 start;
};
};		

#endif
