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

bool dead = 0;
bool newGame = 1;

#define draw_rect_fill(x, y, w, h, color_index)     gc_SetColorIndex(color_index); gc_NoClipRectangle(x, y, w, h);
#define draw_rect(x, y, w, h, color_index)          gc_SetColorIndex(color_index); gc_NoClipRectangleOutline(x, y, w, h);

void main_game_loop(MenuEventArgs* menuEventArgs);
void die(Minefield* minefield, int8_t cursorX, int8_t cursorY);
void win_game(Minefield* minefield);
void draw_demo_board(MenuEventArgs* menuEventArgs);

#endif