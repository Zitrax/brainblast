/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2010
 * @author Daniel Bengtsson
 */

#include <string>

using namespace std;

/**
 * This class is used to report back when a string
 * input is done which was requested using
 * Brainblast::startTextInput()
 *
 * When a string is ready textReady() will be
 * called with the string and it's id.
 */
class TextListener
{
public:
	virtual ~TextListener(){}
		
	virtual void textReady(string s,int id) = 0;
		
	static int id() { return m_text_id++; }
		
private:
	static int m_text_id;
};
	
/** Functor for use in for_each iterations */
struct text_ready
{
	text_ready(string s, int id) : m_s(s), m_id(id) {}
	void operator() (TextListener* tl) { tl->textReady(m_s,m_id); }
	string m_s;
	int m_id;
};
