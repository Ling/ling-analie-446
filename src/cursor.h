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

  static const char *const LEFT_CHARACTER;
  static const char *const RIGHT_CHARACTER;

  int x, y;
  int oldx, oldy;
  Board* board;
  
};

#endif
