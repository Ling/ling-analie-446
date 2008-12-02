#include "cursor.h"
#include "board.h"
#include <curses.h>
#include <sstream>
#include <iomanip>

using namespace std;

const char * const Cursor::LEFT_CHARACTER = "{";
const char * const Cursor::RIGHT_CHARACTER = "}";

Cursor::Cursor() : x(0), y(0), oldx(-1), oldy(-1)
{
}

void Cursor::moveUp(){
    oldy = y;
    oldx = x;
    if(y != 0)
        y--;
    else
        y = 8;
}

void Cursor::moveDown(){
    oldy = y;
    oldx = x;
    if(y != 8)
        y++;
    else
        y =0;
}

void Cursor::moveLeft(){
    oldy = y;
    oldx = x;
    if(x != 0)
        x--;
    else
        x = 8;
}

void Cursor::moveRight(){
    oldy = y;
    oldx = x;
    if(x != 8)
        x++;
    else
        x =0;
}
void Cursor::draw()const{

    ostringstream oss;
    oss << "oldX="<<setw(3)<<oldx<<", oldY="<<setw(3)<<oldy;
    mvprintw(Board::getSquareY(10), 0, oss.str().c_str());
    if(oldx!=-1 && oldy!=-1)
    {
        mvprintw( Board::getSquareY(oldy), Board::getSquareX(oldx), " ");
        mvprintw( Board::getSquareY(oldy), Board::getSquareX(oldx)+2, " ");
    }
    mvprintw( Board::getSquareY(y), Board::getSquareX(x), LEFT_CHARACTER);
    mvprintw( Board::getSquareY(y), Board::getSquareX(x)+2, RIGHT_CHARACTER);
    move(Board::getSquareY(y), Board::getSquareX(x)+1);
}
