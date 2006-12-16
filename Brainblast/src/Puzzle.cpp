/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006
 * @author Daniel Bengtsson
 */
#include "Puzzle.h"

Puzzle::Puzzle(int width, int height) 
    : Field(width,height), 
	  m_solution(0), 
	  m_current(0) 
{
    if(bbc::debug) std::cerr << "Puzzle::Puzzle(" << m_width << "," << m_height << ")\n";

    m_solution = new Brick*[m_width*m_height];
    m_current  = new Brick*[m_width*m_height];

    // initialize all positions to empty
    for(uint i=0; i<m_width*m_height; i++) {
        m_solution[i] = 0;
        m_current[i] = 0;
    }

}

Puzzle::Puzzle(const Puzzle& pz): Field(pz.m_width,pz.m_height),
                                  m_solution(0),
                                  m_current(0)
{

    for( uint i=0; i<m_width*m_height; i++) {
        m_solution[i] = new Brick(*pz.m_solution[i]);
        m_current[i]  = new Brick(*pz.m_current[i]); 
    }

}

Puzzle&
Puzzle::operator=(const Puzzle& pz) 
{ 
    m_width = pz.m_width;
    m_height = pz.m_height;

    for( uint i=0; i<m_width*m_height; i++) {
        m_solution[i] = new Brick(*pz.m_solution[i]);
        m_current[i]  = new Brick(*pz.m_current[i]); 
    }

    return *this;
}

Puzzle::~Puzzle()
{
    if(bbc::debug) std::cerr << "Puzzle::~Puzzle()\n";

    zapArr(m_solution);
    zapArr(m_current);
}

bool
Puzzle::checkSolution()
{
    if(bbc::debug) std::cerr << "Puzzle::checkSolution()\n";
  
    for(uint i=0; i<m_width*m_height; i++)
        if( *m_solution[i] != *m_current[i] )
            return false;

    return true;
}

unsigned int
Puzzle::getWidth() const { return m_width; }

unsigned int
Puzzle::getHeight() const { return m_height; }

Brick*
Puzzle::getCurrentBrickWithIdx(int idx) const
{
    return m_current[idx];
}

Brick*
Puzzle::getSolutionBrickWithIdx(int idx) const
{
    return m_solution[idx];
}

void
Puzzle::setCurrentBrickWithIdx(Brick* b, int idx)
{
    if(bbc::debug) std::cerr << "Puzzle::setCurrentBrickWithIdx(" << b << "," << idx << ")\n";
    m_current[idx] = b;
}

void
Puzzle::setSolutionBrickWithIdx(Brick* b, int idx)
{
    if(bbc::debug) std::cerr << "Puzzle::setSolutionBrickWithIdx(" << b << "," << idx << ")\n";
    m_solution[idx] = b;
}
