#include"board.h"
#include <curses.h>
#include <string>
#include <sstream>

using namespace std;
const string Board::LINE_THIN="+---+---+---++---+---+---++---+---+---+";
const string Board::LINE_THICK="+===+===+===++===+===+===++===+===+===+";

Board::Board():cursor(new Cursor()), board(9*9)
{
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
    init_pair( COLOR_NUMBER, COLOR_GREEN, COLOR_BLACK );

    clearBoard();

}
Board::~Board()
{
    delete cursor;
    endwin();
}
void Board::clearBoard()
{
    for(int i = 0; i != 9*9; ++i)
        board[i] =0;

}
void Board::drawAll()const
{
    int row = 0, col=0;
    clear();
    attron( COLOR_PAIR( COLOR_NORMAL ) );
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

    draw();

}
void Board::draw()const
{
    drawNumbers();
    cursor->draw();
}
void Board::setNumber(int value)
{
    int number = value - '0'; //We want the character '0' to be of value 0.

    int x = cursor->getX();
    int y = cursor->getY();

    board[x+9*y] = number;
}

void Board::drawNumbers()const
{
    for(int i = 0 ; i != 9; ++i)
        for(int j=0 ; j!=9; ++j)
        {
            int number = board[i+9*j];
            ostringstream oss;
            if(number)
                oss<<number;
            else
                oss<<" ";

            attron( COLOR_PAIR( COLOR_NUMBER ) );
            mvprintw( getSquareY(j), Board::getSquareX(i)+1, oss.str().c_str());
            attron( COLOR_PAIR( COLOR_NORMAL ) );
        }
    return;
}

void Board::play()
{
    drawAll();

    int ch;

    while( ( ch=getch() ) != 'q' )
    {

        switch(ch)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                setNumber(ch);
                break;
            case 'C':
                clearBoard();
                break;
            case 'R':
                drawAll();
                break;
            case KEY_LEFT:
                cursor->moveLeft();
                break;
            case KEY_RIGHT:
                cursor->moveRight();
                break;
            case KEY_DOWN:
                cursor->moveDown();
                break;
            case KEY_UP:
                cursor->moveUp();
                break;
        }
        draw();
    }
}

int Board::getSquareX(int x){
    int i = x*4 +3;
    if(x >=3)
        i++;
    if(x >=6)
        i++;
    return i;
}
int Board::getSquareY(int y){
    return 2*y +2;
}
