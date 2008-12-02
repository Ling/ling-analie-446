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

        int getX()const{return x;}
        int getY()const{return y;}

    private:

        static const char *const LEFT_CHARACTER;
        static const char *const RIGHT_CHARACTER;

        int x, y;
        int oldx, oldy;

};

#endif
