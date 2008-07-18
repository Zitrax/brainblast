/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <string>
#include <vector>
#include <ostream>
#include <istream>

#include "bbc.h"

using namespace std;
using namespace bbc;

class HighScore
{
public:
	HighScore(string file, unsigned int max_entries);
	~HighScore(){}

	struct Entry
	{
		Entry() : name(),score(0),level(0),level_set(UNKNOWN) {}

		string name;
		int score;
		int level;
		LEVEL_SET level_set;
	};

	/**
	 * Tries to add an entry into the list.
	 */
	void addEntry(string name,int score,int level, LEVEL_SET level_set);

	/**
	 * Get all highscore entries. (Sorted)
	 */
	vector<Entry> getEntries() const;

	/**
	 * Will return true if the provided score is high enough
	 * to enter the highscore list.
	 * If used it should be used right before addEntry as the
	 * list from disk is being cached until the next addEntry call.
	 */
	bool highEnough(int score);

private:

	void debugOut(vector<Entry>& entries) const;

	/**
	 * This operator is used in write()
	 */
	friend ostream& operator<<(ostream& out, const Entry& entry)
		{
			return out << entry.name << " " << entry.score << " " << entry.level << " " << entry.level_set;
		}

	/**
	 * This operator is used in read()
	 */
	friend istream& operator>>(istream& in, Entry& entry)
		{
			int tmp;
			in >> entry.name >> entry.score >> entry.level >> tmp;
			entry.level_set = intToLevelSet(tmp);
			return in;
		}

	/**
	 * Functor used to sort based on score
	 */
	class score_cmp
	{
	public:
		bool operator() (const Entry& lhs, const Entry& rhs) const { return lhs.score < rhs.score; }
	};

	bool read(vector<Entry>& entries) const;
	bool write(vector<Entry>& entries);

	string m_file;
	unsigned int m_max_entries;

	vector<Entry> m_cached_entries;
};

#endif // HIGHSCORE_H
