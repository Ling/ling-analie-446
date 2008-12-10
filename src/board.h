#ifndef _BOARD_H_
#define _BOARD_H_

#include <string>
#include <vector>
#include "cursor.h"

class Board{
    public:
        Board();
        ~Board();

        void play();
        void drawAll()const;
        void draw()const;
        void drawNumbers()const;

        bool locked(){return is_locked;}

        static int getSquareX(int x);
        static int getSquareY(int y);

        enum Color {COLOR_NORMAL=1, COLOR_ROW_COL, COLOR_ERROR,
            COLOR_ERROR_ROW_COL,COLOR_NUMBER,COLOR_ERROR_NUMBER};
    private:
        void clearBoard();
        void setNumber(int value);
        void lockNumbers();

        bool validate()const;
        bool validateRow(int i)const;
        bool validateCol(int i)const;
        bool validateSquare(int squareX, int squareY)const;

        int valueAt(int index)const;

        int board[81];
        int fixed[81];

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
        //int makeSquareFromI(int i)const{return i/3;}
        //int makeCellFromI(int i)const{return i%3;}

        //Constants
        const static std::string LINE_THIN, LINE_THICK;
};



#endif
