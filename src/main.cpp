/// \file main.cpp
#include "cursor.h"
#include "board.h"
#include <curses.h>

/// Main is run when the program start.
/// Main will create the game board and then start the game. The game loops
/// until 'q' is pressed
/// \param argc Set my the operating system but ignored.
/// \param argv Set by the operating system but ignored.
int main(int argc, char *argv[])
{
    Board board;
    board.play();

    return 0;

}
