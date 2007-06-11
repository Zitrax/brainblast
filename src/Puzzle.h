/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006
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

  Puzzle(int width=5, int height=5);
  Puzzle(const Puzzle& pz);
  ~Puzzle();

  Puzzle& operator=(const Puzzle& pz);

  bool checkSolution();

  Brick* getCurrentBrickWithIdx(int idx) const;
  Brick* getSolutionBrickWithIdx(int idx) const;
  void setCurrentBrickWithIdx(Brick* b, int idx);
  void setSolutionBrickWithIdx(Brick* b, int idx);
  
  unsigned int getWidth() const;
  unsigned int getHeight() const;

 private:
  
  Brick** m_solution;
  Brick** m_current;

};

#endif
