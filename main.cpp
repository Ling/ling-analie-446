#include "cursor.h"
#include "board.h"
#include <curses.h>

int main(int argc, char *argv[])
{
  int ch;
  Board board;
  Cursor cursor(&board);
  board.draw();
  while( ( ch=getch() ) != 'q' )
    {

       if( ch == KEY_LEFT  )
        {
	  cursor.moveLeft();
        }
        else if( ch == KEY_RIGHT )
        {
	  cursor.moveLeft();
        }
        else if( ch == KEY_DOWN  )
        {
	  cursor.moveLeft();
        }
        else if( ch == KEY_UP    )
        {
	  cursor.moveLeft();
	}
       
       board.draw();
       cursor.draw();
    }


  return 0;

}
