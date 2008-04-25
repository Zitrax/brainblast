/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */
#include "Puzzle.h"

#include "Brainblast.h"
#include "../images/bb.h"

Puzzle::Puzzle(int width, int height, SDL_Rect rect)
  : Field(width,height), 
	m_rect(rect),
	m_solution(0), 
	m_current(0),
	m_current_tree(0),
	m_solution_tree(0),
	m_background_tree(0),
	m_back(0),
	m_selected_tile(0),
	m_selection_tile(0),
	m_selection_sprite(0),
	m_correct_bricks(0),
	m_total_solution_bricks(0),
	m_s_coord(this)
{
    if(bbc::debug) std::cerr << "Puzzle::Puzzle(" << m_width << "," << m_height << "," 
							 << rect.x << "," << rect.y << "," << rect.w << "," << rect.h << ")\n";

    m_solution = new Brick*[m_width*m_height];
    m_current  = new Brick*[m_width*m_height];
	m_back     = new KrTile*[m_width*m_height];

    // initialize all positions to empty
    for(uint i=0; i<m_width*m_height; i++) {
        m_solution[i] = 0;
        m_current[i] = 0;
    }

}

Puzzle::~Puzzle()
{
    if(bbc::debug) std::cerr << "Puzzle::~Puzzle()\n";

	// Apparently we need to remove the childnodes from the trees first
	// would be convenient if that was handled automatically.
	for( uint i=0; i<m_width*m_height; i++) 
	{
		if( m_solution[i] ) 
			Brainblast::instance()->engine()->Tree()->DeleteNode(m_solution[i]->getSprite());
		if( m_current[i] ) 
			Brainblast::instance()->engine()->Tree()->DeleteNode(m_current[i]->getSprite());
		if( m_back[i] )
			Brainblast::instance()->engine()->Tree()->DeleteNode(m_back[i]);
	}

	if( m_current_tree )
		Brainblast::instance()->engine()->Tree()->DeleteNode(m_current_tree);
	if( m_background_tree )
		Brainblast::instance()->engine()->Tree()->DeleteNode(m_background_tree);
	if( m_solution_tree )
		Brainblast::instance()->engine()->Tree()->DeleteNode(m_solution_tree);

	for( uint i=0; i<m_width*m_height; i++) 
	{
		delete m_solution[i];
		delete m_current[i];
	}
	zapArr(m_solution);
	zapArr(m_current);
	zapArr(m_back); // The KrTiles are deleted along with the background tree

	// Notes about other pointers
	// --------------------------
	// m_selected_tile    - points to a tile in m_back
	// m_selection_sprite - if we have something here it still belongs to Brainblast
	// m_selection_tile   - belong to the background tree  
}

bool
Puzzle::checkSolution()
{
    if(bbc::debug) std::cerr << "Puzzle::checkSolution()\n";
  
    for(uint i=0; i<m_width*m_height; i++)
		if( m_solution[i] )
		{
			if( !m_current[i] ) return false;
			if( *m_solution[i] != *m_current[i] ) return false; 
		}
			
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
Puzzle::setSolutionBrickWithIdx(const Brick* const b, unsigned int idx)
{
    if(bbc::debug) std::cerr << "Puzzle::setSolutionBrickWithIdx(" << b << "," << idx << "," << b->getSprite()->NodeId() << ")\n";

	assert(idx < m_width*m_height );

	KrSprite* s = b->getSprite();
	if( s )
	{
		if( !m_solution_tree ) {
			m_solution_tree = new KrImNode;
			assert(Brainblast::instance()->engine()->Tree());
			Brainblast::instance()->engine()->Tree()->AddNode(0, m_solution_tree);
			m_solution_tree->SetZDepth(SOLZ);
		}
		KrSprite* sprite = s->Clone()->ToSprite(); 
		Brick* nb = new Brick(sprite,b->id());
		Brainblast::instance()->engine()->Tree()->AddNode(m_solution_tree, sprite);
		int xspace = m_rect.w/m_width;
		int yspace = m_rect.h/m_height;
		nb->setPos(m_rect.x + (idx%m_width)*xspace+xspace/2,
				   m_rect.y + (idx/m_width)*yspace+yspace/2);
		m_solution[idx] = nb;
		m_total_solution_bricks++;
	}
}

void
Puzzle::setBackgroundTile(KrTile* tile)
{
	// Delete old tree if existing
	if( m_background_tree )
		Brainblast::instance()->engine()->Tree()->DeleteNode(m_background_tree);
	// Create new tree and insert in the global tree
	if( !m_background_tree ) {
		m_background_tree = new KrImNode;
		Brainblast::instance()->engine()->Tree()->AddNode(0, m_background_tree);
		m_background_tree->SetZDepth(BACZ);
	}
	
	// Create and position all bg tiles
	for(uint x=0;x<m_width;x++)
	{
		int xspace = m_rect.w/m_width;
		
		for(uint y=0;y<m_height;y++)
		{
			int yspace = m_rect.h/m_height;
			
			KrTile* ctile = tile->Clone()->ToTile();
			Brainblast::instance()->engine()->Tree()->AddNode(m_background_tree, ctile);
			ctile->SetPos(m_rect.x + x*xspace+xspace/2-ctile->Size()/2,
						  m_rect.y + y*yspace+yspace/2-ctile->Size()/2);
			m_back[y*m_width+x] = ctile;
		}
	}
}

void Puzzle::updateSelection()
{
	if( m_selected_tile )
		m_selected_tile->SetVisible(true);

	int x = m_s_coord.x();
	int y = m_s_coord.y();
	int i = m_s_coord.i();

	m_selected_tile = m_back[i];
	m_back[i]->SetVisible(false);

	int xspace = m_rect.w/m_width;
	int yspace = m_rect.h/m_height;

	if( !m_selection_tile )
	{
		KrTileResource* tileRes = Brainblast::instance()->engine()->Vault()->GetTileResource( BB_GRAYC );
		assert(tileRes);
		m_selection_tile = new KrTile(tileRes);
		Brainblast::instance()->engine()->Tree()->AddNode(m_background_tree, m_selection_tile);
	}
	
	m_selection_tile->SetVisible(true);

	m_selection_tile->SetPos(m_rect.x + x*xspace+xspace/2-m_selection_tile->Size()/2,
							 m_rect.y + y*yspace+yspace/2-m_selection_tile->Size()/2);
	
	m_selection_sprite->SetPos(m_rect.x + x*xspace+xspace/2,
							   m_rect.y + y*yspace+yspace/2);
}

void Puzzle::stopSelection()
{
	m_selection_sprite = 0;
	if( m_selection_tile )
		m_selection_tile->SetVisible(false);
	if( m_selected_tile )
		m_selected_tile->SetVisible(true);
}

void Puzzle::startSelection(BrainSprite* bs)
{ 
	bs->setTemporary(false);
	m_selection_sprite=bs; 
	updateSelection(); 
}

bool Puzzle::navigateTowards()
{
	// Find non-filled brick for id
	int target_id = m_selection_sprite->NodeId();
	int nav_id = -1;

	for(uint i=0; i<m_width*m_height; i++)
		if( m_solution[i] && (m_solution[i]->id() == target_id) ) // Is there the solution we look for here
		{
			if( !m_current[i] || (m_solution[i]->id() != m_current[i]->id()) ) // Is it not yet correct
			{
				nav_id = i;
				break;
			}
		}

	assert(nav_id!=-1); // We should never look for id's that do not exist
	if(nav_id==-1)
		return false;

	if( nav_id == m_s_coord.i() )
		return true;

	if( m_s_coord.x() != static_cast<int>(nav_id%m_width) )
		navigate(LEFT);
	else
		navigate(UP);

	return false;
}

bool Puzzle::select(BrainSprite** bs)
{
	// First check that there is not already
	// a correct brick at this location.
	int idx = m_s_coord.i();
	if( m_current[idx] && m_solution[idx] && 
		(m_current[idx]->id() == m_solution[idx]->id()) )
		return false;

	if( m_selected_tile )
		m_selected_tile->SetVisible(true);
	if( m_selection_tile )
		m_selection_tile->SetVisible(false);

	bool ret = false;

	// Create solution brick
	Brick* b = new Brick(m_selection_sprite,m_selection_sprite->NodeId());

	if( !m_current_tree )
	{
		m_current_tree = new KrImNode;
		Brainblast::instance()->engine()->Tree()->AddNode(0, m_current_tree);
		m_current_tree->SetZDepth(SOLZ);
	}

	// Remove previous selection if exists
	if( m_current[idx] )
		Brainblast::instance()->engine()->Tree()->DeleteNode(m_current[idx]->getSprite());
	
	// A correct brick
	if(  m_solution[idx] && ( b->id() == m_solution[idx]->id() ) )
	{
		int x = m_back[idx]->X();
		int y = m_back[idx]->Y();
		Brainblast::instance()->engine()->Tree()->DeleteNode(m_back[idx]);
		KrTileResource* tileRes = Brainblast::instance()->engine()->Vault()->GetTileResource( BB_CORRECT );
		m_back[idx] = new KrTile(tileRes);
		Brainblast::instance()->engine()->Tree()->AddNode(m_background_tree, m_back[idx]);
		m_back[idx]->SetPos(x,y);
		ret = true;
		m_selected_tile = 0;
		m_correct_bricks++;
	}

	m_current[idx] = b;

	*bs = m_selection_sprite;
	m_selection_sprite = 0;
	return ret;
}

std::vector<int> Puzzle::getSolutionTypes() const
{
	// TODO: Can be optimized to just be calculated
	//       the first time.
	std::vector<int> types;
	for( uint i=0; i<m_width*m_height; i++) 
	{
		if( m_solution[i] ) 
			types.push_back(m_solution[i]->id());
	}
	return types;
}

int Puzzle::brickScore() const
{
	// TODO: We can precalculate the content in this function
	std::vector<int> types = getSolutionTypes();
	std::sort(types.begin(),types.end());
	types.erase(std::unique(types.begin(),types.end()),types.end());
	
	int s=0;
    for( uint i=0; i<m_width*m_height; i++)
		if( m_solution[i] ) s++;

	// Using a 10 multiplier
	return types.size()*m_width*m_height*s*10;
}
