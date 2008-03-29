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
	
//  	int getXCoord(int x) const;
// 	int getYCoord(int y) const;

private:
	
	SDL_Rect m_rect;
	Brick** m_solution;
	Brick** m_current;
	KrImNode* m_solution_tree;
	KrImNode* m_background_tree;

};

#endif
