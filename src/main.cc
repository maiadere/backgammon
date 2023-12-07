#include <ncurses.h>

#include <clocale>
#include <cstdlib>
#include <ctime>

#include "game.hh"

void init_colorpairs() {
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

int main() {
  srand(time(NULL));
  setlocale(LC_ALL, "");
  initscr();
  keypad(stdscr, TRUE);
  start_color();
  init_colorpairs();
  cbreak();
  noecho();
  Game* game = Game_new();
  Game_mainloop(game);
  Game_free(game);
  endwin();
  return 0;
}
