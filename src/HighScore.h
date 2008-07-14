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

using namespace std;

class HighScore
{
public:
	HighScore(string file);
	~HighScore(){}

	void addEntry(string name,int score,int level);

private:

	struct Entry
	{
		Entry() : name(),score(0),level(0) {}

		string name;
		int score;
		int level;

	};

	/**
	 * This operator is used in write()
	 */
	friend ostream& operator<<(ostream& out, const Entry& entry)
		{
			out << entry.name << " " << entry.score << " " << entry.level;
			return out;
		}

	/**
	 * This operator is used in read()
	 */
	friend istream& operator>>(istream& in, Entry& entry)
		{
			in >> entry.name >> entry.score >> entry.level;
			return in;
		}

	/**
	 * Functor used to sort based on score
	 */
	class score_cmp
	{
	public:
		bool operator() (const Entry& lhs, const Entry& rhs) const { return lhs.score > rhs.score; }
	};

	bool read(vector<HighScore::Entry>& entries) const;
	bool write(vector<Entry>& entries);

	string m_file;

};

#endif // HIGHSCORE_H
