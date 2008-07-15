/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
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

bool HighScore::highEnough(int score)
{
	m_cached_entries.clear();
	read(m_cached_entries);
	
	vector<Entry>::iterator it = min_element(m_cached_entries.begin(),m_cached_entries.end(),score_cmp());
	if( (it != m_cached_entries.end()) && (*it).score > score )
		return false;

	return true;
}

void HighScore::addEntry(string name,int score,int level)
{
	Entry entry;
	entry.name = name;
	entry.score = score;
	entry.level = level;

	// Read current file (if not cached)
	if( m_cached_entries.empty() && !highEnough(score) )
	{
		m_cached_entries.clear();
		return; // Score too low
	}

	// Insert current entry
	m_cached_entries.push_back(entry);

	// Reverse Sorting
	sort(m_cached_entries.rbegin(),m_cached_entries.rend(),score_cmp());

	// And write back the result
	write(m_cached_entries);
	
	// Clear the cache
	m_cached_entries.clear();
	
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

	in.close();
	return true;
}

bool HighScore::write(vector<Entry>& entries)
{
	ofstream out;
	out.open(m_file.c_str());
	if( !out )
	{
		cerr << "ERROR - Could open file for writing highscore (" << m_file.c_str() << ") (" << strerror(errno) << ").\n";
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
	
	out.close();
	return true;
}

