/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */
#include "Puzzle.h"

#undef min // Stupid! It seems something else has defined min
#include "Brainblast.h"

Puzzle::Puzzle(int width, int height, SDL_Rect rect)
  : Field(width,height), 
	m_rect(rect),
	m_solution(0), 
	m_current(0),
	m_solution_tree(0),
	m_background_tree(0)
{
    if(bbc::debug) std::cerr << "Puzzle::Puzzle(" << m_width << "," << m_height << "," 
							 << rect.x << "," << rect.y << "," << rect.w << "," << rect.h << ")\n";

    m_solution = new Brick*[m_width*m_height];
    m_current  = new Brick*[m_width*m_height];

    // initialize all positions to empty
    for(uint i=0; i<m_width*m_height; i++) {
        m_solution[i] = 0;
        m_current[i] = 0;
    }

}

Puzzle::Puzzle(const Puzzle& pz): Field(pz.m_width,pz.m_height),
								  m_rect(pz.m_rect),
                                  m_solution(0),
                                  m_current(0),
								  m_solution_tree(0),
								  m_background_tree(0)
{

    for( uint i=0; i<m_width*m_height; i++) {
        m_solution[i] = new Brick(*pz.m_solution[i]);
        m_current[i]  = new Brick(*pz.m_current[i]); 
    }
	// Todo: Is this correct ?
	if( pz.m_solution_tree ) {
		m_solution_tree = pz.m_solution_tree->Clone();
		Brainblast::instance()->engine()->Tree()->AddNode(0, m_solution_tree);
		m_solution_tree->SetZDepth(5);
	}
	if( pz.m_background_tree ) {
		m_background_tree = pz.m_background_tree->Clone();
		Brainblast::instance()->engine()->Tree()->AddNode(0, m_background_tree);
		m_background_tree->SetZDepth(15);
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
	// Todo: Is this correct ?
	if( pz.m_solution_tree ) {
		m_solution_tree = pz.m_solution_tree->Clone();
		Brainblast::instance()->engine()->Tree()->AddNode(0, m_solution_tree);
		m_solution_tree->SetZDepth(5);
	}
	if( pz.m_background_tree ) {
		m_background_tree = pz.m_background_tree->Clone();
		Brainblast::instance()->engine()->Tree()->AddNode(0, m_background_tree);
		m_background_tree->SetZDepth(15);
	}
	
    return *this;
}

Puzzle::~Puzzle()
{
    if(bbc::debug) std::cerr << "Puzzle::~Puzzle()\n";

    zapArr(m_solution);
    zapArr(m_current);

	//assert("make sure solution are cleaned"==0);
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
Puzzle::setSolutionBrickWithIdx(const Brick* const b, int idx)
{
    if(bbc::debug) std::cerr << "Puzzle::setSolutionBrickWithIdx(" << b << "," << idx << "," << b->getSprite()->NodeId() << ")\n";

	KrSprite* s = b->getSprite();
	if( s )
	{
		if( !m_solution_tree ) {
			m_solution_tree = new KrImNode;
			assert(Brainblast::instance()->engine()->Tree());
			Brainblast::instance()->engine()->Tree()->AddNode(0, m_solution_tree);
			m_solution_tree->SetZDepth(5);
		}
		KrSprite* sprite = s->Clone()->ToSprite(); 
		Brick* nb = new Brick(sprite,b->id());
		Brainblast::instance()->engine()->Tree()->AddNode(m_solution_tree, sprite);
		int xspace = m_rect.w/m_width;
		int yspace = m_rect.h/m_height;
		nb->setPos(m_rect.x + (idx%m_width)*xspace+xspace/2,
				   m_rect.y + (idx/m_height)*yspace+yspace/2);
		m_solution[idx] = nb;
	}
}

void
Puzzle::setBackgroundTile(KrTile* tile)
{
	// Delete old tree if existing
	if( m_background_tree )
		Brainblast::instance()->engine()->Tree()->DeleteNode(m_background_tree);
	// Create new tree and insert in the global tree
	m_background_tree = new KrImNode;
	Brainblast::instance()->engine()->Tree()->AddNode(0, m_background_tree);
	m_solution_tree->SetZDepth(15);
	
	// Create and position all bg tiles
	for(int x=0;x<m_width;x++)
	{
		int xspace = m_rect.w/m_width;
		
		for(int y=0;y<m_height;y++)
		{
			int yspace = m_rect.h/m_height;
			
			KrTile* ctile = tile->Clone()->ToTile();
			Brainblast::instance()->engine()->Tree()->AddNode(m_background_tree, ctile);
			ctile->SetPos(m_rect.x + x*xspace+xspace/2-ctile->Size()/2,
						  m_rect.y + y*yspace+yspace/2-ctile->Size()/2);
		}
	}
}

// int 
// Puzzle::getXCoord(int x) const
// {
// }

// int 
// Puzzle::getYCoord(int y) const
// {
	
// }
