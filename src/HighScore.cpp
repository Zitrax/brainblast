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
	: m_file(file), m_max_entries(max_entries)
{
}

void HighScore::addEntry(string name,int score,int level)
{
	Entry entry;
	entry.name = name;
	entry.score = score;
	entry.level = level;

	// 1. Read current file
	vector<HighScore::Entry> entries;
	read(entries);

	// 2. Insert current entry
	entries.push_back(entry);

	// 3. Sort
	sort(entries.begin(),entries.end(),score_cmp());

	// 4. And write back the result
	write(entries);
	
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
