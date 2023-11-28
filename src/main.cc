#include <ncurses.h>

#include <clocale>
#include <cstdlib>
#include <ctime>

#include "game.hh"

int main() {
  srand(time(NULL));
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  Game* game = Game_new();
  Game_mainloop(game);
  Game_free(game);
  endwin();
  return 0;
}
