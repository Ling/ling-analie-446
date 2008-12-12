/// \file board.cpp
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

/** Create a new sudoku board.
 *  The board is initially blank and will display a help menu and its validity.
 */
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

/**
 * Ensures we release all resource needed by Board.
 * Drop the terminal back to it's original status.
 */
Board::~Board()
{
    delete cursor;
    endwin();
}

/**
 * Clears the grid of all numbers.
 *
 * Reset the game back to the state it is when the program is started.
 */
void Board::clearBoard()
{
    clearFixedNumbers();
    clearBoardNumbers();

    grid=fillSolutionGrid();
    allValid();
    is_locked=false;

}

/** Draws the grid onto the screen.
 *
 * This is important is the screen becomes
 * corrupted. This is essentially needed when the window is resized, the player
 * will need to tell the game to redraw the board and numbers.
 */
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

/**
 * Draws.
 *
 * Draw will print out the status of the board, the cursor, and the numbers in
 * the board.
 */
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

/**
 * Display a help pane for the user.
 *
 * Displays the various command that can be used.
 */
void Board::drawHelp()const
{
    int j = getSquareY(0);
    int i = getSquareX(10)+2;
    const size_t linesSize = 13;
    const char* lines[linesSize]={
        "    Help",
        "q - Quit",
        "C - Clear entire board",
        "N - Clear entered numbers",
        "S - Save puzzle",
        "O - Open puzzle",
        "L - Lock numbers",
        "G - Generate a random new puzzle",
        "H - Hint",
        "R - Redraw screen",
        "",
        "1-9  - Set to number",
        "0    - Clear the number"
    };

    for(int k = 0 ; k!=linesSize; ++k)
    {
        mvprintw(j+k,i,lines[k]);
    }
}

/**
 * Set the value at the cursor position.
 *
 * \pre The \e value given is in the range [0-9].
 *
 * \param value The number to be set. If \e value is 0, the number is cleared.
 */
void Board::setNumber(int value)
{
    int number = value - '0'; //We want the character '0' to be of value 0.

    int x = cursor->getX();
    int y = cursor->getY();

    board[x+9*y] = number;
    validate();
}

/**
 * Make all the numbers entered into given numbers.
 *
 * The numbers contained in the board will be locked into the given puzzle.
 * This gives the player the chance to manually enter his puzzle before
 * playing.
 *
 * \pre The board is not empty.
 * \pre The board is in a valid state.
 * \pre The board has not been locked since it was cleared.
 * \post All the numbers are not part of the fixed given numbers of a puzzle.
 */
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
 * Initializes the 'invalid' arrays to valid.
 *
 * This will set the board state to valid on start and reset.
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
 *
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
 * Validate a row.
 *
 * Check that there are no duplicates in the row.
 * \param i The row to be checked.
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
 *
 * Check that there are no duplicates in a column.
 * \param i The column to be checked.
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
 * Validates a subgrid.
 *
 * Check that the square contains no duplicates.  This square is addressed in
 * the board as square rows [0-2] and columns [0-2].
 * \param squareX The x coordinate of the square.
 * \param squareY The y coordinate of the square.
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

/**
 * Gets the value at the index.
 *
 * \param index The 1-dimensional position who's value we want.
 * \return The value at that position, wether it is locked or not.
 */
int Board::valueAt(int index)const
{
    if(fixed[index])
        return fixed[index];
    if(board[index])
        return board[index];
    return 0;
}

/**
 * Display the board's numbers on the screen.
 */
void Board::drawNumbers()const
{
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
/**
 * Helper function for display.
 *
 * A valid position is one that is not inside a row, column, or square with
 * diplicate numbers.
 * \param x The x co-ordinare to check.
 * \param y The y co-ordinate to check.
 * \return Whether the position should be ok, or highlighted as an error.
 */
bool Board::validPosition(int x, int y)const
{
    return !(invalidCols[x]||invalidRows[y]||
             invalidSqs[ ((x/3))+((y/3)*3) ]);

}

/**
 * Loads a file into the playing board.
 *
 * The file format is simple, a blank in the board is denoted by 0, and a given
 * number is denoted by it's character. For example, 6 in the board will be the
 * character '6' in the file. The line of text will contain all 81 positions in
 * the board ordered from left to right, and top to bottom.
 *
 * \param in A stream containing one or two lines. The first line will contain
 * the puzzle and the optional second line has the player's progress to the
 * solution
 */
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

/**
 * Saves the playing board to a file.
 *
 * The file format is simple, a blank in the board is denoted by 0, and a given
 * number is denoted by it's character. For example, 6 in the board will be the
 * character '6' in the file. The line of text will contain all 81 positions in
 * the board ordered from left to right, and top to bottom.
 *
 * \param out A stream opened for writing and truncated.  One or two lines will
 * be printed to the file. The second line only appear if there are any numbers
 * input by the player.
 */
void Board::save(ostream& out)const
{
    copy(fixed, fixed+9*9, ostream_iterator<int>(out));
    out<<endl;
    ostringstream oss;
    copy(board, board+9*9, ostream_iterator<int>(oss));
    if(oss.str() != string(9*9,'0'))
        out<<oss.str()<<endl;
}

/** Helper function for computations.
 *
 * If we look at the board as a 3 by 3 grid or 3 by 3 squares, and
 * further, a total grid of numbers that is 9 by 9. This function maps
 * row and column co-ordinates from the board level, to thw row and
 * column of the square is belongs to.
 */
int Board::getSquareIndexFromBoardIndex(int index)const
{
    int col = indexToX(index);
    int row = indexToY(index);

    col /=3; row /= 3;
    return col + row*3;
}

/**
 * Prompt the user for a file to load.
 *
 * The user may type a full or relative path to a puzzle file to load.
 */
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

/**
 * Prompt the user for a file to save.
 *
 * The user may type a full or relative path to a puzzle file to save.
 */
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

/**
 * Main game loop.
 *
 * Draw initial screen, then loop taking user input until the user hits the \e
 * 'Q' key.
 */
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
            case ' ':
                setNumber('0');
                break;
            case 'C':
                clearBoard();
                break;
            case 'N':
                clearBoardNumbers();
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
            case 'H':
                generateSolution(grid);
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

/**
 * Helper function.
 *
 * \param x The column.
 * \return The leftmost screen column inside this column.
 */
int Board::getSquareX(int x){
    int i = x*4 +3;
    if(x >=3)
        i++;
    if(x >=6)
        i++;
    return i;
}

/**
 * Helper function.
 *
 * \param y The row.
 * \return The topmost screen row inside this row.
 */
int Board::getSquareY(int y){
    return 2*y +2;
}

/**
 * Generates a grid containing valid random numbers.
 *
 * \todo Fix this method.
 */
void Board::generateRandomGrid()
{
    clearBoard();/*clears the grid before starting*/

    int rdmNum = rand() % 9 + 1; /*generates a random number from 1-9*/
    int rdmPos = rand() % 81; /*generates a random positiqon from 0-80*/
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
 * Fill the solution grid where each cell is filled with
 * a vector of 9 numbers (1-9)
 */
map< int, vector<int> > Board::fillSolutionGrid()
{
    map<int,vector<int> > slnMap;
    vector<int> cellVec;

    for(int j=0; j != 9; ++j){/*fill each cell with a vector*/
        cellVec.push_back(j+1); /*initialize*/
    }

    for(int i=0; i != 81; ++i){
        /*assign a newly filled vector in the map which represents each grid cell*/
        slnMap[i]= cellVec;
    }

    return slnMap;
}

/**
 * Retrieves all the positions in the subgrid
 */
vector<int> Board::getAllIndicesInSameSquare(int K)const{
    int col = makeSquareFromI(indexToX(K));
    int row = makeSquareFromI(indexToY(K));
    vector<int> ret;
    for(int j = 0; j!=3;++j)
    {
        int some_row = makeIfromSquare(row,j);
        for(int i = 0;i!=3;++i)
        {
            int some_col = makeIfromSquare(col,i);
            int index = makeIndex(some_col,some_row);
            ret.push_back(index);
        }
    }
    return ret;
}


/**
 * Solves the puzzle by iterating through each row, column and subgrid.
 * Each cell (0-81) contains a vector of 9 elements. The current value is
 * removed from the vector of each corresponding row, column and subgrid.
 *
 * \todo This method is currently broken, it only partially solves the board.
 * It should be considered a hint function for the moment.
 */
void Board::generateSolution(map<int, vector<int> >& slnMap)
{
    int value = 0;
    int row = 0;
    int col = 0;
    vector<int> cellVec;
    unsigned int vecSizeBefore;
    unsigned int vecSizeAfter;


    for(int i = 0; i != 81; ++i){
        if((fixed[i] != 0 || board[i] != 0)){
            if(fixed[i] != 0){
                value = fixed[i];
            }else if(board[i] != 0){
                value = board[i];
            }

            row = indexToX(i);
            col = indexToY(i);

            /*get the vector from slnMap*/
            for(int c = 0; c != 9; ++c){
                int index = makeIndex(row, c);
                cellVec = slnMap[index]; /*assign the vector from the cell*/
                vecSizeBefore = cellVec.size();
                for (unsigned int z = 0; z != cellVec.size(); ++z){
                    if(cellVec[z] == value){
                        cellVec.erase(cellVec.begin()+z);
                        break;
                    }
                }
                //cellVec.erase(cellVec.begin() + value-1);
                //remove(cellVec.begin(), cellVec.end(),value); /*remove the number from the vector*/
                vecSizeAfter = cellVec.size();
                slnMap[index] = cellVec; /*reassign the updated vector to the map*/
            }

            /*get the vector from slnMap*/
            for(int r = 0; r < 9; ++r){
                cellVec = slnMap[makeIndex(r, col)]; /*assign the vector from the cell*/
                vecSizeBefore = cellVec.size();
                for (unsigned int z = 0; z != vecSizeBefore; ++z){
                    if(cellVec[z] == value){
                        cellVec.erase(cellVec.begin()+z);
                        break;
                    }
                }
                //remove(cellVec.begin(), cellVec.end(), value); /*remove the number from the vector*/
                vecSizeAfter = cellVec.size();
                slnMap[makeIndex(r, col)] = cellVec; /*reassign the updated vector to the map*/

            }

            vector<int> square = getAllIndicesInSameSquare(i);

            for(vector<int>::const_iterator it = square.begin(); it!=square.end();++it)
            {
                cellVec = slnMap[*it]; /*assign the vector from the cell*/
                vecSizeBefore = cellVec.size();
                for (unsigned int z = 0; z != vecSizeBefore; ++z){
                    if(cellVec[z] == value){
                        cellVec.erase(cellVec.begin()+z);
                        break;
                    }
                }
                //remove(cellVec.begin(), cellVec.end(), value); /*remove the number from the vector*/
                vecSizeAfter = cellVec.size();
                slnMap[*it] = cellVec; /*reassign the updated vector to the map*/
                cellVec.clear();
            }
        }
    }

    bool truth = false;
    for(int i = 0; i != 81; ++i){
        int sz = slnMap[i].size();/*size of the remaining vector*/
        if(sz==1){
            board[i] = slnMap[i][0];
            truth = true;
        }
    }
    if(truth){
        mvprintw(getSquareY(10)+1,getSquareX(0),"Something is true");

    }else{
        mvprintw(getSquareY(10)+1,getSquareX(0),"Nothing is true  ");
    }

}


