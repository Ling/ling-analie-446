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

        static int getSquareX(int x);
        static int getSquareY(int y);

        /// These are described as COLOR_PAIRs in the ncurses system. We assign them symbolic names.
        enum Color {COLOR_NORMAL=1, COLOR_ROW_COL, COLOR_ERROR ,
            COLOR_ERROR_ROW_COL,COLOR_NUMBER,COLOR_ERROR_NUMBER};
    private:
        void clearBoard();
        void clearBoardNumbers(){std::fill(board,board+9*9,0);}
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

        int board[81];
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
