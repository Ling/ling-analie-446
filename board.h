#ifndef _BOARD_H_
#define _BOARD_H_

#include <string>

class Board{
 public:
  Board();
  ~Board();
  void draw();
  int getSquareX(int x)const;
  int getSquareY(int y)const;

  enum Color {COLOR_NORMAL=1, COLOR_ROW_COL, COLOR_ERROR, COLOR_ERROR_ROW_COL};
 private:
  int board[81];

  const static std::string LINE_THIN, LINE_THICK;
  
};



#endif
