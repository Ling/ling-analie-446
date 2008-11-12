#include "cursor.h"
#include "board.h"
#include <curses.h>

int main(int argc, char *argv[])
{
  int ch;
  Board board;
  Cursor cursor(&board);
  board.draw();
  cursor.draw();
  while( ( ch=getch() ) != 'q' )
    {

       if( ch == KEY_LEFT  )
        {
	  cursor.moveLeft();
        }
        else if( ch == KEY_RIGHT )
        {
	  cursor.moveRight();
        }
        else if( ch == KEY_DOWN  )
        {
	  cursor.moveDown();
        }
        else if( ch == KEY_UP    )
        {
	  cursor.moveUp();
	}
       
       board.draw();
       cursor.draw();
    }


  return 0;

}
