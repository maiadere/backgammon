#pragma once

#include <ncurses.h>

#define cprintw(color, ...)  \
  attron(COLOR_PAIR(color)); \
  printw(__VA_ARGS__);       \
  attron(COLOR_PAIR(7));
