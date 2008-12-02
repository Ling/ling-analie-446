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

        void setNumber(int value);

        static int getSquareX(int x);
        static int getSquareY(int y);

        enum Color {COLOR_NORMAL=1, COLOR_ROW_COL, COLOR_ERROR, COLOR_ERROR_ROW_COL};
    private:
        void clearBoard();

        std::vector<int> board;
        Cursor* cursor;

        const static std::string LINE_THIN, LINE_THICK;
};



#endif
