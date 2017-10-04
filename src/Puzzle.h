/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
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
	~Puzzle();

	/** 
	 * Brainblast will tell the Puzzle when navigation is alllow
	 * such that the players will not navigate around in other modes
	 */
	void allowNavigation(bool allow) { m_allow_navigation = allow; }

	bool checkSolution();
	int correctBricks() const { return m_correct_bricks; }
	int totalSolutionBricks() const { return m_total_solution_bricks; }

	Brick* getCurrentBrickWithIdx(int idx) const;
	Brick* getSolutionBrickWithIdx(int idx) const;
	void setCurrentBrickWithIdx(Brick* b, int idx);
	void setSolutionBrickWithIdx(const Brick* const b, unsigned int idx);
	
	void setBackgroundTile(KrTile* tile);

	void setVisibleSolution(bool visible) { m_solution_tree->SetVisible(visible); }

	std::vector<int> getSolutionTypes() const;

	void setSelectionTile(KrTile* tile) { m_selection_tile = tile; }
	void startSelection(BrainSprite* bs);
	/// If the selection sprite is removed it must be reset
	void stopSelection();
	enum direction { LEFT,RIGHT,UP,DOWN };
	void navigate(enum direction dir);
	/**
	 * Returns true when we are at the correct id.
	 * The caller has to make sure that the id is indeed
	 * a valid remaining solution or we will just 
	 * not do anything in this function.
	 */
	bool navigateTowards();

	bool isSelecting() const { return m_selection_sprite != 0; } 
	const BrainSprite* getSelectionSprite() const { return m_selection_sprite; }
	/**
	 * This creates a brick of the moved type at the current
	 * position. 
	 *
	 * If bs is set the brick has taken over the 
	 * ownership of the sprite.
	 *
	 * Returns true if the selections was correct
	 */
	bool select(BrainSprite** bs);

	/**
	 * Each level give their own score per correct brick
	 * which can be retrieved here.
	 */
	int brickScore() const;

private:

	Puzzle(const Puzzle& bb);
    Puzzle& operator=(const Puzzle& bb);

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

	int m_correct_bricks;         // TODO: This might also be used in checkSolution.
	int m_total_solution_bricks;  // TODO: This might also be used in checkSolution.

	bool m_allow_navigation;

	/**
	 * Handles coordinate selection on the grid.
	 */
	class SelectCoord
	{
	public:
		explicit SelectCoord(const Puzzle* const p) : m_x(0), m_y(0), m_p(p) {}
		
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

		SelectCoord(const SelectCoord&);
		SelectCoord& operator=(const SelectCoord&);

		void left()  { decV(m_x,m_p->m_width);  }
		void right() { incV(m_x,m_p->m_width);  }
		void up()    { decV(m_y,m_p->m_height); }
		void down()  { incV(m_y,m_p->m_height); } 
		
		bool incV(int& v, int max) {	
			++v; 
			if( v == max ) v = 0; 
			return v == 0;
		}
		bool decV(int& v, int max) {
			--v; 
			if( v == -1 ) v = max-1; 
			return v == max-1;
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
