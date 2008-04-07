/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */
// -*- mode: C++; -*-

#ifndef PUZZLE_H
#define PUZZLE_H

#include "Brick.h"
#include "Field.h"
class BrainSprite;

class Puzzle : public Field
{
public:
	
	Puzzle(int width, int height, SDL_Rect rect);
	Puzzle(const Puzzle& pz);
	~Puzzle();
	
	Puzzle& operator=(const Puzzle& pz);
	
	bool checkSolution();
	
	Brick* getCurrentBrickWithIdx(int idx) const;
	Brick* getSolutionBrickWithIdx(int idx) const;
	void setCurrentBrickWithIdx(Brick* b, int idx);
	void setSolutionBrickWithIdx(const Brick* const b, unsigned int idx);
	
	void setBackgroundTile(KrTile* tile);

	void setVisibleSolution(bool visible) { m_solution_tree->SetVisible(visible); }

	std::vector<int> getSolutionTypes() const;

	void setSelectionTile(KrTile* tile) { m_selection_tile = tile; }
	void startSelection(BrainSprite* bs);
	enum direction { LEFT,RIGHT,UP,DOWN };
	void navigate(enum direction dir) { m_s_coord.move(dir); updateSelection(); }
	bool isSelecting() const { return m_selection_sprite != 0; } 
	/**
	 * This creates a brick of the moved type at the current
	 * position. 
	 *
	 * If bs i set the brick has taken over the 
	 * ownership of the sprite.
	 *
	 * Returns true if the selections was correct
	 */
	bool select(BrainSprite** bs);

//  	int getXCoord(int x) const;
// 	int getYCoord(int y) const;

	/**
	 * Each level give their own score per correct brick
	 * which can be retrieved here.
	 */
	int brickScore() const;

private:

	void updateSelection();
	
	SDL_Rect  m_rect;
	Brick**   m_solution;
	Brick**   m_current;
	KrImNode* m_current_tree;
	KrImNode* m_solution_tree;
	KrImNode* m_background_tree;
	KrTile**  m_back;

	KrTile*   m_selected_tile;
	KrTile*   m_selection_tile;
	BrainSprite* m_selection_sprite;

	class SelectCoord
	{
	public:
		SelectCoord(const Puzzle* const p) : m_x(0), m_y(0), m_p(p) {}
		
		int x() const { return m_x; }
		int y() const { return m_y; }
		int i() const { return m_x+m_y*m_p->m_width; }

		void move(enum direction dir) {
			switch(dir) {
			case LEFT:   left(); break;
			case RIGHT: right(); break;
			case UP:       up(); break;
			case DOWN:   down(); break;
			}
		}

	private:

		void left()  { if( decV(&m_x,m_p->m_width)  ) decV(&m_y,m_p->m_height); }
		void right() { if( incV(&m_x,m_p->m_width)  ) incV(&m_y,m_p->m_height); }
		void up()    { if( decV(&m_y,m_p->m_height) ) decV(&m_x,m_p->m_width);  }
		void down()  { if( incV(&m_y,m_p->m_height) ) incV(&m_x,m_p->m_width);  } 

		bool incV(int* v, int max) {	
			++(*v); 
			if( *v == max ) *v = 0; 
			return *v == 0;
		}
		bool decV(int* v, int max) {
			--(*v); 
			if( *v == -1 ) *v = max-1; 
			return *v == max-1;
		}

		int  m_x;
		int  m_y;
		const Puzzle* const m_p;
	};

 	SelectCoord m_s_coord;

	static const int SOLZ = 20;
	static const int BACZ = -1; // Not sure why this has to be negative

};

#endif
