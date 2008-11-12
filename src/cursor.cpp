#include "cursor.h"
#include "board.h"
#include <curses.h>

Cursor::Cursor(Board* board) : x(0), y(0), board(board)
{
}

void Cursor::moveUp(){
  if(y != 0)
    y--;
  else
    y = 8;
}

void Cursor::moveDown(){
  if(y != 8)
    y++;
  else
    y =0;
}

void Cursor::moveLeft(){
  if(x != 0)
    x--;
  else
    x = 8;
}

void Cursor::moveRight(){
  if(x != 8)
    x++;
  else
    x =0;
}

void Cursor::draw()const{

  mvprintw( board->getSquareY(y), board->getSquareX(x), "{");
  mvprintw( board->getSquareY(y), board->getSquareX(x)+2, "}");
  move(board->getSquareY(y), board->getSquareX(x)+1);
}
