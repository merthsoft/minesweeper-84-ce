#ifndef MAIN_H
#define MAIN_H

/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

/* Standard headers - it's recommended to leave them included */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minefield.h"
#include "menu.h"

#define FOREGROUND_COLOR 1
#define BACKGROUND_COLOR 2

void main_game_loop(MenuEventArgs* menuEventArgs);
void die(Minefield* minefield, int8_t cursorX, int8_t cursorY);
void win_game(Minefield* minefield);
void draw_demo_board(MenuEventArgs* menuEventArgs);
void print_help_text(MenuEventArgs* menuEventArgs);

#endif