#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "board.h"

class Cursor{
 public:
  Cursor(Board* board);
  void moveUp();
  void moveDown();
  void moveRight();
  void moveLeft();
  void draw()const;
 private:

  int x, y;
  Board* board;
  
};

#endif
