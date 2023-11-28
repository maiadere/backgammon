#include <ncurses.h>

#include <clocale>
#include <cstdlib>

#include "board.hh"

int main() {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  Board *board = Board_new();

  for (;;) {
    clear();
    Board_print(board);
    refresh();
    if (getch() == 'q') {
      break;
    }
  }

  endwin();
  free(board);
  return 0;
}
