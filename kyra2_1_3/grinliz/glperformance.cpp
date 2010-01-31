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
	#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <stdio.h>
#include <vector>
#include <algorithm>
//#include <functional>

#include "gldebug.h"
#include "glperformance.h"

using namespace grinliz;
using namespace std;

PerformanceData* Performance::map[ GL_MAX_PROFILE_DATAITEM ];
ProfileData Performance::profile;
ProfileData Performance::sortedProfile;

int Performance::numMap = 0;


PerformanceData::PerformanceData( const char* name )
{ 
	this->name = name;
	count = 0;
	totalTime = 0;
	Performance::map[ Performance::numMap ] = this;
	++Performance::numMap;
}

///////////////////////////////////////////////////////


void Performance::Clear()
{
	for( int i=0; i<numMap; ++i )
	{
		map[i]->count = 0;
		map[i]->totalTime = 0;
	}
}


/*static*/ const ProfileData& Performance::GetData()
{
	U64 total = 0;

	int i;
	
	// Compute the total time, and the highest bit.
	for( i=0; i<numMap; ++i )
	{
		total += map[i]->totalTime;
	}

	U64 highBit = 0;
	U64 one = 1;
	while ( one < total )
	{
		one *= 2;
		++highBit;
	}

	int lowBit = (int)highBit - 31;	// Get away from the sign bit.
	if ( lowBit < 0 )
		lowBit = 0;

	profile.count = numMap;
	profile.totalTime = (U32)( total >> lowBit );
	if ( profile.totalTime == 0 )
		profile.totalTime = 1;

	for( i=0; i<numMap; ++i )
	{
		profile.item[i].count = map[i]->count;
		profile.item[i].name  = map[i]->name;
		profile.item[i].totalTime = (U32) (map[i]->totalTime >> lowBit);
		if ( profile.item[i].totalTime == 0 )
			profile.item[i].totalTime = 1;
	}
	return profile;
}


//struct PDIgreater : binary_function<ProfileDataItem, ProfileDataItem, bool> {
struct PDIgreater {
	bool operator()(const ProfileDataItem& _X, const ProfileDataItem& _Y) const
		{return (_X.totalTime > _Y.totalTime ); }
};


/*static*/ const grinliz::ProfileData& Performance::GetSortedData()
{
	GetData();
	sortedProfile = profile;
	sort( &sortedProfile.item[0], 
		  &sortedProfile.item[ sortedProfile.count ], 
		  PDIgreater() );
	return sortedProfile;
}


void Performance::Dump( FILE* fp, const char* desc )
{
	const ProfileData& profile = GetData();

	fprintf( fp, "%s\n", desc );
	U32 i;
	for( i=0; i<profile.count; ++i )
	{
		fprintf( fp, "%26s calls=%8d time=%10d time/call=%10d percent=%.1f%%\n",
				profile.item[i].name,
				profile.item[i].count,
				profile.item[i].totalTime,
				profile.item[i].totalTime / profile.item[i].count,
				100.0 * double( profile.item[i].totalTime ) / double( profile.totalTime ) );
	}
	fprintf( fp, "Total time: %d\n", profile.totalTime );
}

