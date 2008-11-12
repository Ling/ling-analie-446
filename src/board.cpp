#include"board.h"
#include <curses.h>
#include <string>

using namespace std;
const string Board::LINE_THIN="+---+---+---++---+---+---++---+---+---+";
const string Board::LINE_THICK="+===+===+===++===+===+===++===+===+===+";

Board::Board(){
  initscr( ); //Turn on ncurses
  noecho( );  //Don't echo characters
  cbreak( );  //input is parsed per character, instead of at endlines.
  keypad( stdscr, TRUE ); //Turn on the keypad (F keys, arrows)
  curs_set( 0 );  //make the cursor invisible
  start_color( );


  init_pair( COLOR_NORMAL, COLOR_WHITE, COLOR_BLACK );
  init_pair( COLOR_ROW_COL, COLOR_BLUE, COLOR_BLACK );
  init_pair( COLOR_ERROR, COLOR_WHITE, COLOR_RED  );
  init_pair( COLOR_ERROR_ROW_COL, COLOR_BLUE, COLOR_RED );
}
Board::~Board()
{
  endwin();
}

void Board::draw(){
  int row = 0, col=0;
    attron( COLOR_PAIR( 1 ) );
    attron( A_BOLD );
    mvprintw( row, col+4, "1   2   3    4   5   6    7   8   9" );
    attroff( A_BOLD );

    for(int i = 0; i !=10; ++i)
      {
	mvprintw( 2*i+row+1, col+2, ( (i%3==0)? LINE_THICK : LINE_THIN ).c_str()  );
      }

    //Draw the board:
    for( int i=0; i<9; ++i )
    {
        std::string num( 1, (char)(i+1) + 48 );
        attron( A_BOLD );
        mvprintw( row + 2 + 2*i, col, num.c_str( ) );
        attroff( A_BOLD );
        mvprintw( row + 2 + 2*i, col+2, "|   |   |   ||   |   |   ||   |   |   |" );
    }
    
}

int Board::getSquareX(int x)const{
  int i = x*4 +3;
  if(x >=3)
    i++;
  if(x >=6)
    i++;
  return i;
}
int Board::getSquareY(int y)const{
  return 2*y +2;
}
