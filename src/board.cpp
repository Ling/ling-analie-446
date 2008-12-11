#include"board.h"
#include <curses.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>
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

    srand ( time(NULL) );/*initialize random seed*/

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
	const size_t linesSize = 8;
	const char* lines[linesSize]={
	"    Help",
	"C - Clear board",
	"S - Save puzzle",
	"O - Open puzzle",
	"L - Lock numbers",
	"G - Generate a random new puzzle",
	"V - solVe the current puzzle",
	"R - Redraw screen"};
	for(int k = 0 ; k!=linesSize; ++k)
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

/**
* Initializes the 'invalid' arrays to valid
*/
void Board::allValid()const
{
    for(int i = 0; i!=9; ++i)
    {
        invalidRows[i]= invalidCols[i]= invalidSqs[i]= false;
    }
}

/**
* Validation of the grid.
* Returns false is there is 2 or more same numbers in a row, column or subgrid
*/
bool Board::validate()const
{
    allValid();

    /*Validates the rows and the columns*/
    /*Stores the invalid coordinates*/
    for(int i = 0; i != 9; ++i)
    {
        if(!validateRow(i))
            invalidRows[i]=true;
        if(!validateCol(i))
            invalidCols[i]=true;
    }

    /*Validates the subgrid*/
    for(int i = 0; i!=3; ++i)
        for(int j = 0; j!=3; ++j)
            if(!validateSquare(i,j))
               invalidSqs[i+j*3]=true;

    /*If just one number is invalid, the grid is invalid*/
    for(int i = 0; i!=9; ++i)
        if(invalidRows[i]||invalidCols[i]||invalidSqs[i])
            return false;
    return true;
}

/**
* Validates a row
* Parameter: i-> row to be checked
*/
bool Board::validateRow(int i)const
{
    bool used[9];/*keeps count of the number of times a number is used*/
    for(int a = 0; a!=9; ++a)used[a]=false;/*initialize*/

    for(int c = 0; c!=9 ; ++c)
    {
        int index = c+9*i; /*find the correct position in the array.*/

        int value = (fixed[index])? fixed[index] : board[index];
        if(!value)
            continue;
        value--; /*we have valued 1..9 but indices 0..8.*/

        if(used[value])
            return false;
        used[value]=true;
    }
    return true; /*If we get here we didn't return false in the loop so ok.*/
}

/**
 * Validates a column
 * Parameter: i-> column to be checked
 */
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

/**
 * Validates a subgrid
 * Parameters:  squareX -> x coordinate
 *              squareY -> y coordinate
 */
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
            case 'G':
                generateRandomGrid();
                drawAll();
                break;
            case 'V':
                //generateSolution(fillSolutionGrid());
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

/**
* Generates a random number grid
*/
void Board::generateRandomGrid()
{
    clearBoard();/*clears the grid before starting*/

    int rdmNum = rand() % 9 + 1; /*generates a random number from 1-9*/
    int rdmPos = rand() % 81; /*generates a random position from 0-80*/
    int rdmPosNext = rand() % 81; /*generates the next random position from 0-80*/
    int rdmCount = rand() % 30+1;/*quantity of numbers generated. Ranges from 5 to 30 numbers.*/
    bool isValid = false; /*start with an invalid board*/

    while (rdmCount < 5 ){ /*makes sure there are at least 5 random numbers generated*/
        rdmCount = rand() % 30+1;/*TODO: make the maximum number variable depending on the difficulty level*/
    }

    for ( int i=0;i<81;++i )
        fixed[i] = 0;/*reset the elements of fixed array to 0*/

    for(int i = 0; i < rdmCount; ++i){
        fixed[rdmPos] = rdmNum;/*assign the number to the fixed array*/
        board[rdmPos] = rdmNum; /*insert into the board to check if it's valid*/

        isValid = validate();

        while (!isValid) {/*validate*/
            fixed[rdmPos] = 0;/*if the number is invalid reset the board position to 0*/
            board[rdmPos] = 0;/*if the number is invalid reset the fixed position to 0*/

            /*keep generating a new number until a valid one comes up*/
            rdmNum = rand() % 9 + 1; /*generates a random number from 1-9*/
            fixed[rdmPos] = rdmNum;/*assign the number to the fixed array*/
            board[rdmPos] = rdmNum; /*insert into the board to check if it's valid*/
            isValid = validate();/*validate*/
        }

        /*generates a new position*/
        /*can't be the same as the current one*/
        while(rdmPos == rdmPosNext){
            rdmPosNext = rand() % 81;
        }

        rdmPos = rdmPosNext; /*assign the next position to the current one*/
        rdmNum = rand() % 9 + 1; /*generate a new number*/
        rdmPosNext = rand() % 81; /*generate the next new position*/
    }
}

/**
* Fill the solution grid
*/

map< int, vector<int> > Board::fillSolutionGrid()
{
    map<int,vector<int> > slnMap;
    vector<int> cellVec;

    for(int i=0; i < 80; ++i){
        for(int j=1; j < 10; ++j){//fill each cell with an array
            cellVec.push_back(j); //initialize
        }

        slnMap.insert(pair< int,vector<int> >(i, cellVec));
    }

    return slnMap;
}

vector<int> getAllIndicesInSameSquare(int K){
    int col = makeSquareFromI(makeIndexToX(K));
    int row = makeSquareFromI(makeIndexToY(K));
    vector<int> ret;
    for(int j = 0; j!=3;++j)
        int somer = makeIfromSquare(row,j);
        for(int i = 0;i!=3;++i)
        {
            int somec makeIfromSquare(col,i)
            int index  = makeIndex(somec,somec);
            ret.push_back(index);
        }
    return ret;
}


/**
* Solves the puzzle
*/
void Board::generateSolution(map<int, vector<int> >& slnMap)
{
    int value = 0;
    int row = 0;
    int col = 0;
    vector<int> cellVec;

    for(int i = 0; i < 80; ++i){
        if((fixed[i] != 0 || board[i] != 0)){
            if(fixed[i] != 0){
                value = fixed[i];
            }else if(board[i] != 0){
                value = board[i];
            }

            row = indexToX(i);
            col = indexToY(i);

            /*get the vector from slnMap*/
            for(int c = 0; c < 9; ++c){
            //for (map<int,vector<int> >::iterator iter = slnMap.begin(); iter != slnMap.end(); ++iter ) {
                cellVec = slnMap[makeIndex(row, c)]; /*assign the vector from the cell*/
                cellVec[value-1] = 1; /*set value to 1 to show that it is used*/
                slnMap[makeIndex(row, c)] = cellVec; /*reassign the updated vector to the map*/
            }

            /*get the vector from slnMap*/
            for(int r = 0; r < 9; ++r){
            //for (map<int,vector<int> >::iterator iter = slnMap.begin(); iter != slnMap.end(); ++iter ) {
                cellVec = slnMap[makeIndex(r, col)]; /*assign the vector from the cell*/
                cellVec[value-1] = 1; /*set value to 1 to show that it is used*/
                slnMap[makeIndex(r, col)] = cellVec; /*reassign the updated vector to the map*/
            }

            for(int rr = 0; rr!=3 ; ++rr){
                int pos = makeIfromSquare(row, col);
                for(int i = 0; i!=3; ++i){


                }
            }
        }
    }
}

