/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#include "HighScore.h"

#include <fstream>
#include <iostream>
#include <algorithm> // copy
#include <iterator>  // ofstream_iterator
#include <errno.h>

HighScore::HighScore(string file, unsigned int max_entries)
	: m_file(file), m_max_entries(max_entries), m_cached_entries()
{
}

void HighScore::reloadCache()
{
	m_cached_entries.clear();
	read(m_cached_entries);
}

bool HighScore::highEnough(int score)
{
	// Read current file (if not cached)
	if( m_cached_entries.empty() )
		reloadCache();

	// If the list is not yet full we can always add...
	if( m_cached_entries.size() < m_max_entries )
		return true;

	// ... if not we need to check if we have more points than the worst entry.
	vector<Entry>::iterator it = min_element(m_cached_entries.begin(),m_cached_entries.end(),score_cmp());
	if( (it != m_cached_entries.end()) && (*it).score > score )
		return false;

	return true;
}

void HighScore::addEntry(string name,int score,int level,LEVEL_SET level_set)
{
	Entry entry;
	entry.name = name;
	entry.score = score;
	entry.level = level;
	entry.level_set = level_set;

	if( !highEnough(score) )
		return; // Score too low

	// The score is high enough, so insert current entry
	m_cached_entries.push_back(entry);

	// Reverse Sorting
	sort(m_cached_entries.rbegin(),m_cached_entries.rend(),score_cmp());

	// And write back the result
	write(m_cached_entries);
	
	// Clear the cache
	m_cached_entries.clear();
	
}

vector<HighScore::Entry> HighScore::getEntries() const
{
	vector<Entry> entries;
	read(entries);
	sort(entries.rbegin(),entries.rend(),score_cmp());
	return entries;
}

bool HighScore::read(vector<Entry>& entries) const
{
	ifstream in;

	in.open(m_file.c_str());
	if( !in )
	{
		cerr << "WARNING - Could not read highscore (" << m_file.c_str() << ") (" << strerror(errno) << ").\n";
		in.close();
		return false;
	}
	
	// Trying to use C++ as much as possible
	// Might look a bit cryptic but this line iterates over the in istream
	// and copies each Entry using the operator>> and uses back_inserter
	// to insert each entry into the entries vector.
	copy(istream_iterator<Entry>(in),istream_iterator<Entry>(),back_inserter(entries));

	//debugOut(entries);

	in.close();
	return true;
}

bool HighScore::write(vector<Entry>& entries)
{
	string new_file(m_file);
	string bak_file(m_file);
	new_file += ".new";
	bak_file += "~";

	ofstream out(new_file.c_str());
	if( !out )
	{
		cerr << "ERROR - Could open file for writing highscore (" << new_file.c_str() << ") (" << strerror(errno) << ").\n";
		out.close();
		return false;
	}

	// Trying to use C++ as much as possible
	// A bit simpler than the version in read. Here we simply
	// copy all or the max number of Entry in entries to the out ostream, 
	// and separate them each with a \n.
	copy(entries.begin(),
		 entries.size()>m_max_entries ? entries.begin()+m_max_entries : entries.end(),
		 ostream_iterator<Entry>(out,"\n"));
	
	// Make sure the file is written to disk
	// and check that the flushing went alright
	out.flush();
	if( !out )
	{
		cerr << "ERROR - Could not flush file for writing highscore (" << new_file.c_str() << ") (" << strerror(errno) << ").\n";
		out.close();
		return false;
	}
	out.close();

	// Make backup and apply the new file
	// This might be overly pedantic for a simple
	// highscore table but feels good :)
	rename(m_file.c_str(),bak_file.c_str());
	rename(new_file.c_str(),m_file.c_str());

	return true;
}

void HighScore::debugOut(vector<Entry>& entries) const
{
	vector<Entry>::const_iterator it;
	vector<Entry>::const_iterator end = entries.end();
	for( it = entries.begin(); it!=end; ++it )
		cout << *it;
}
