#include <ncurses.h>

#include <clocale>
#include <cstdlib>
#include <ctime>

#include "game.hh"

void init_color_hex(int color, int hex) {
  int r = 1000 * ((hex >> 16) & 0xff) / 255;
  int g = 1000 * ((hex >> 8) & 0xff) / 255;
  int b = 1000 * (hex & 0xff) / 255;
  init_color(color, r, g, b);
}

void override_colors() {
  init_color_hex(COLOR_BLACK, 0x071013);
  init_color_hex(COLOR_BLUE, 0x373b3e);
  init_color_hex(COLOR_RED, 0xEB5160);
  init_color_hex(COLOR_YELLOW, 0xffdd63);
  init_color_hex(COLOR_WHITE, 0xFFFFFF);
  init_color_hex(COLOR_GREEN, 0x99a0a4);
}

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
  override_colors();
  cbreak();
  noecho();
  Game* game = Game_new();
  Game_mainloop(game);
  Game_free(game);
  endwin();
  return 0;
}
