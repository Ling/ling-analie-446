#ifndef _CURSOR_H_
#define _CURSOR_H_

class Cursor{
    public:
        Cursor();
        void moveUp();
        void moveDown();
        void moveRight();
        void moveLeft();
        void draw()const;

        /**Get the current sudoku column of the cursor.  \return The column position */
        int getX()const{return x;}
        /**Get the current sudoku row of the cursor.  \return The row position */
        int getY()const{return y;}

    private:

        static const char *const LEFT_CHARACTER;
        static const char *const RIGHT_CHARACTER;

        int x, y;
        int oldx, oldy;

};

#endif
