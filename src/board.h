/// \file board.h
#ifndef _BOARD_H_
#define _BOARD_H_

#include <string>
#include <vector>
#include <map>
#include <iosfwd>
#include <algorithm>
#include "cursor.h"

/// The sudoku playing board.
///
/// The playing board manages both the state of the game and handles input and
/// output through ncurses.
class Board{
    public:
        Board();
        ~Board();

        void play();
        void drawAll()const;
        void draw()const;

        void load(std::istream& in);
        void save(std::ostream& out)const;

        /// Are there locked numbers on the board.
        /// \return Status of the board, whether the numbers have been locked yet or not.
        bool locked(){return is_locked;}

        /** Get the screen column of a well.  \param x The square's board column.  \return The leftmost screen column of square x. */
        static int getSquareX(int x);
        /** Get the screen row of a well.  \param y The square's board row.  \return The leftmost screen row of square x. */
        static int getSquareY(int y);

        /** These are described as COLOR_PAIRs in the ncurses system.  
         *
         * We assign them symbolic names.
         */
        enum Color {COLOR_NORMAL=1,/**<Normal text*/ 
            COLOR_ROW_COL,/**<\deprecated Higlight row and column containing the cursor.*/
            COLOR_ERROR, /**<Highlight cell part of a row, column, or square containing an error.*/
            COLOR_ERROR_ROW_COL,/**<\deprecated Highlight of rows and columns containing the cursor and errors.*/
            COLOR_NUMBER,/**<Given numbers.*/
            COLOR_ERROR_NUMBER/**<Given numbers part of a row, column, or square containing an error.*/
        };
    private:
        void clearBoard();
        /**Reset the player numbers to blank.*/
        void clearBoardNumbers(){std::fill(board,board+9*9,0);}
        /**Reset the given numbers to blank.*/
        void clearFixedNumbers(){std::fill(fixed,fixed+9*9,0);}
        void setNumber(int value);
        void lockNumbers();
        void drawNumbers()const;
        void drawHelp()const;

        // These two should be private, only called by the interface.
        void loadFile();
        void saveFile()const;

        bool validate()const;
        bool validateRow(int i)const;
        bool validateCol(int i)const;
        bool validateSquare(int squareX, int squareY)const;

        void generateRandomGrid();
        void generateSolution(std::map<int, std::vector<int> >& slnMap);
        std::map< int, std::vector<int> > fillSolutionGrid();
        std::vector<int> getAllIndicesInSameSquare(int K)const;
        int valueAt(int index)const;

        /** The player entered numbers.*/
        int board[81];
        /** The numbers given in the puzzle.  These number may not be changed. */
        int fixed[81];

        std::map< int, std::vector<int> > grid;

        mutable bool invalidRows[9], invalidCols[9], invalidSqs[9];
        bool is_locked;
        Cursor* cursor;


        //Utility

        void allValid()const;
        bool validPosition(int x, int y)const;
        int getSquareIndexFromBoardIndex(int index)const;

        //X means cols
        //Y means rows
        /**
         * Utility function.
         *
         * Translate from 2 dimensional cell co-ordinates to one dimensional array co-ordinates.
         *
         * \param x The column of the cell
         * \param y The row of the cell
         * \return The index to the cell in the board arrays and friends.
         */
        int makeIndex(int x, int y)const {return x+y*9;}
        int indexToX(int i)const {return i%9;}
        int indexToY(int i)const {return i/9;}
        int makeIfromSquare(int squareX, int cellX)const{return cellX+squareX*3;}
        int makeSquareFromI(int i)const{return i/3;}
        int makeCellFromI(int i)const{return i%3;}

        //Constants
        const static std::string LINE_THIN, LINE_THICK;
};



#endif
