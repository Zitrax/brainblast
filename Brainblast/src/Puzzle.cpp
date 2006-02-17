#include "Puzzle.h"

Puzzle::Puzzle(int width_, int height_) 
    : m_width(width_), m_height(height_), solution(0), current(0) 
{
    if(bbc::debug) std::cerr << "Puzzle::Puzzle(" << m_width << "," << m_height << ")\n";

    solution = new Brick*[m_width*m_height];
    current  = new Brick*[m_width*m_height];

    // initialize all positions to empty
    for(uint i=0; i<m_width*m_height; i++) {
        solution[i] = 0;
        current[i] = 0;
    }

}

Puzzle::Puzzle(const Puzzle& pz): m_width(pz.m_width),
                                  m_height(pz.m_height),
                                  solution(0),
                                  current(0)
{

    for( uint i=0; i<m_width*m_height; i++) {
        solution[i] = new Brick(*pz.solution[i]);
        current[i]  = new Brick(*pz.current[i]); 
    }

}

Puzzle&
Puzzle::operator=(const Puzzle& pz) 
{ 
    m_width = pz.m_width;
    m_height = pz.m_height;

    for( uint i=0; i<m_width*m_height; i++) {
        solution[i] = new Brick(*pz.solution[i]);
        current[i]  = new Brick(*pz.current[i]); 
    }

    return *this;
}

Puzzle::~Puzzle()
{
    if(bbc::debug) std::cerr << "Puzzle::~Puzzle()\n";

    zapArr(solution);
    zapArr(current);
}

bool
Puzzle::checkSolution()
{
    if(bbc::debug) std::cerr << "Puzzle::checkSolution()\n";
  
    for(uint i=0; i<m_width*m_height; i++)
        if( *solution[i] != *current[i] )
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
    return current[idx];
}

Brick*
Puzzle::getSolutionBrickWithIdx(int idx) const
{
    return solution[idx];
}

void
Puzzle::setCurrentBrickWithIdx(Brick* b, int idx)
{
    if(bbc::debug) std::cerr << "Puzzle::setCurrentBrickWithIdx(" << b << "," << idx << ")\n";
    current[idx] = b;
}

void
Puzzle::setSolutionBrickWithIdx(Brick* b, int idx)
{
    if(bbc::debug) std::cerr << "Puzzle::setSolutionBrickWithIdx(" << b << "," << idx << ")\n";
    solution[idx] = b;
}
