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
	void setSolutionBrickWithIdx(const Brick* const b, int idx);
	
	void setBackgroundTile(KrTile* tile);

	void setVisibleSolution(bool visible) { m_solution_tree->SetVisible(visible); }

	void setSelectionTile(KrTile* tile) { m_selection_tile = tile; }
	void startSelection(BrainSprite* bs);
	enum direction { LEFT,RIGHT,UP,DOWN };
	void navigate(enum direction dir) { m_s_coord.move(dir); updateSelection(); }
	bool isSelecting() const { return m_selection_sprite != 0; } 

//  	int getXCoord(int x) const;
// 	int getYCoord(int y) const;

private:

	void updateSelection();
	
	SDL_Rect  m_rect;
	Brick**   m_solution;
	Brick**   m_current;
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
		
		int X() const { return m_x; }
		int Y() const { return m_y; }

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

};

#endif
