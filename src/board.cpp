#include"board.h"
#include <curses.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;
const string Board::LINE_THIN="+---+---+---++---+---+---++---+---+---+";
const string Board::LINE_THICK="+===+===+===++===+===+===++===+===+===+";

Board::Board():cursor(new Cursor())
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
    init_pair( COLOR_ERROR_NUMBER, COLOR_GREEN, COLOR_RED );

    clearBoard();

}
Board::~Board()
{
    delete cursor;
    endwin();
}

void Board::clearBoard()
{
    for(int i = 0; i != 9*9; ++i){
        board[i] =0;
        fixed[i] =0;
    }
    allValid();
    is_locked=false;

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

    drawHelp();
    draw();


}
void Board::draw()const
{
    drawNumbers();
    cursor->draw();
    if(validate())
        mvprintw(getSquareY(10),getSquareX(0),"The board is valid.  ");
    else
    {

        attron( COLOR_PAIR( COLOR_ERROR ) );
        mvprintw(getSquareY(10),getSquareX(0),"The board is invalid.");
        attron( COLOR_PAIR( COLOR_NORMAL ) );
    }
}

void Board::drawHelp()const
{
	int j = getSquareY(0);
	int i = getSquareX(10)+2;
	const char* lines[6]={
	"    Help",
	"C - Clear board",
	"S - Save puzzle",
	"O - Open puzzle",
	"L - Lock numbers",
	"R - Redraw screen"};
	for(int k = 0 ; k!=6; ++k)
	{
		mvprintw(j+k,i,lines[k]);
	}
}

void Board::setNumber(int value)
{
    int number = value - '0'; //We want the character '0' to be of value 0.

    int x = cursor->getX();
    int y = cursor->getY();

    board[x+9*y] = number;
    validate();
}

void Board::lockNumbers()
{
    //Let's not lock if the player can't input a CORRECT puzzle.
    if(!validate())
        return;
    if(locked())
        return;
    for(int i = 0; i!= 9*9; ++i)
    {
        fixed[i]=board[i];
        board[i]=0;
    }
    is_locked=true;
}

void Board::allValid()const
{
    for(int i = 0; i!=9; ++i)
    {
        invalidRows[i]= invalidCols[i]= invalidSqs[i]= false;
    }
}

bool Board::validate()const
{
    allValid();
    for(int i = 0; i != 9; ++i)
    {
        if(!validateRow(i))
            invalidRows[i]=true;
        if(!validateCol(i))
            invalidCols[i]=true;
    }
    for(int i = 0; i!=3; ++i)
        for(int j = 0; j!=3; ++j)
            if(!validateSquare(i,j))
               invalidSqs[i+j*3]=true;
    
    for(int i = 0; i!=9;++i)
        if(invalidRows[i]||invalidCols[i]||invalidSqs[i])
            return false;
    return true;
}

bool Board::validateRow(int i)const
{
    bool used[9];
    for(int a = 0; a!=9; ++a)used[a]=false;

    for(int c = 0; c!=9 ; ++c)
    {
        int index = c+9*i; //find the correct position in the array.

        int value = (fixed[index])? fixed[index] : board[index];
        if(!value)
            continue;
        value--; //we have valued 1..9 but indices 0..8.

        if(used[value])
            return false;
        used[value]=true;
    }
    return true; //If we get here we didn't return false in the loop so ok.
}
/** I am a comment to be replaced */
bool Board::validateCol(int i)const
{
    bool used[9];
    for(int a = 0; a!=9; ++a)used[a]=false;

    for(int r = 0; r!=9 ; ++r)
    {
        int index = r*9+i; //find the correct position in the array.

        int value = (fixed[index])? fixed[index] : board[index];
        if(!value)
            continue;
        value--; //we have valued 1..9 but indices 0..8.

        if(used[value])
            return false;
        used[value]=true;
    }
    return true; //If we get here we didn't return false in the loop so ok.
}
bool Board::validateSquare(int squareX, int squareY)const
{
    bool used[9];
    for(int a = 0; a!=9; ++a)used[a]=false;
    
    for(int j = 0; j!=3 ; ++j)
    {
        int row = makeIfromSquare(squareY, j); 
        for(int i = 0; i!=3; ++i)
        {
            int col = makeIfromSquare(squareX, i);
            int index = makeIndex(col,row);
            int value = valueAt(index);
            
            if(!value)
                continue;
            value--;

            if(used[value])
                return false;
            used[value]=true;
        }
    }
    return true;
}
int Board::valueAt(int index)const
{
    if(fixed[index])
        return fixed[index];
    if(board[index])
        return board[index];
    return 0;
}

void Board::drawNumbers()const
{
//    We were testing our calculate methods.
//    for(int i = 0 ; i != 3; ++i)
//        for(int j=0 ; j!=3; ++j)
//            for(int x=0 ; x!=3; ++x)
//                for(int y=0 ; y!=3; ++y)
//                {
//                    int col = makeIfromSquare(i,x);
//                    int row = makeIfromSquare(j,y);
//
//
//                    int number = 1+x+3*y;
//                    ostringstream oss;
//                    if(number)
//                        oss<<number;
//                    else
//                        oss<<" ";
//
//                    attron( COLOR_PAIR( COLOR_NUMBER ) );
//                    mvprintw( getSquareY(row), Board::getSquareX(col)+1, oss.str().c_str());
//                    attron( COLOR_PAIR( COLOR_NORMAL ) );
//                }
    for(int i = 0 ; i != 9; ++i)
        for(int j=0 ; j!=9; ++j)
        {
            bool given = true;
            int number = fixed[i+9*j];
            if(!number)
            {
                given = false;
                number = board[i+9*j];
            }
            
            ostringstream oss;
            if(number)
                oss<<number;
            else
                oss<<" ";
            
            if(validPosition(i,j))
                attron( COLOR_PAIR( ((given)?COLOR_NUMBER:COLOR_NORMAL) ) );
            else
            {
                attron( A_BOLD );
                attron( COLOR_PAIR( ((given)?COLOR_ERROR_NUMBER:COLOR_ERROR) ) );
                attroff( A_BOLD );
            }
            mvprintw( getSquareY(j), Board::getSquareX(i)+1, oss.str().c_str());
            attron( COLOR_PAIR( COLOR_NORMAL ) );
        }
    return;
}
bool Board::validPosition(int x, int y)const
{
    return !(invalidCols[x]||invalidRows[y]||
             invalidSqs[ ((x/3))+((y/3)*3) ]);

}
void Board::load(istream& in)
{
    if(!in)
        return;

    string line;
    getline(in, line);
    istringstream iss(line);
    char c;
    int buffer[9*9];
    int i = 0;
    while(i!=9*9)
    {
        iss >> c;
        if(!iss) return;
        int n = c-'0';
        if(n >=0 && n  <=9)
        {
            buffer[i]=n;
            ++i;
        }
    }
    std::copy(buffer, buffer+9*9, fixed);
    is_locked=true;

}
void Board::save(ostream& out)const
{
   copy(fixed, fixed+9*9, ostream_iterator<int>(out)); 
   out<<endl;
   ostringstream oss;
   copy(board, board+9*9, ostream_iterator<int>(oss));
   if(oss.str() != string(9*9,'0'))
    out<<oss.str()<<endl;
}
int Board::getSquareIndexFromBoardIndex(int index)const
{
    int col = indexToX(index);
    int row = indexToY(index);

    col /=3; row /= 3;
    return col + row*3;
}
void Board::loadFile()
{
    char fileName[100];

    mvprintw(getSquareY(10)-1,getSquareX(0),"                                                  ");
    mvprintw(getSquareY(10)-1,getSquareX(0),"File to open: ");
    echo();
    getstr(fileName);
    noecho();
    ifstream ifs(fileName);
    if(!ifs)
    {
         mvprintw(getSquareY(10)-1,getSquareX(0),"Couldn't read %s.", fileName);
         return;
    }
    load(ifs);

    mvprintw(getSquareY(10)-1,getSquareX(0),"                                                  ");
}
void Board::saveFile()const
{
    char fileName[100];

    mvprintw(getSquareY(10)-1,getSquareX(0),"                                                  ");
    mvprintw(getSquareY(10)-1,getSquareX(0),"File to save: ");
    echo();
    getstr(fileName);
    noecho();
    ofstream ofs(fileName, ios_base::out | ios_base::trunc);
    if(!ofs)
    {
         mvprintw(getSquareY(10)-1,getSquareX(0),"Couldn't write %s.", fileName);
         return;
    }
    save(ofs);

    mvprintw(getSquareY(10)-1,getSquareX(0),"                                                  ");

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
            case 'S':
                saveFile();
                break;
            case 'O':
                loadFile();
                break;
            case 'L':
                lockNumbers();
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
