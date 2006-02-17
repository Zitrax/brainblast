// -*- mode: C++; -*-

#ifndef PUZZLE_H
#define PUZZLE_H

#include "Brick.h"

class Puzzle
{
 public:

  Puzzle(int width_, int height_);
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
  
  unsigned int m_width;
  unsigned int m_height;

  Brick** solution;
  Brick** current;

};

#endif
