#ifndef MINEFIELD_H_
#define MINEFIELD_H_

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

#define minefield_get_visible_type(minefield, i, j)                      minefield_get_tile(minefield->visibleField[i][j])
#define TILE_HEIGHT 20
#define TILE_WIDTH 20

#define MIN(a, b)	a < b ? a : b
#define MAX(a, b)	a > b ? a : b

#define FILLED -1
#define FLAGGED -2
#define QUESTION -3
#define EXPLOSION -4

#define LCD_WIDTH_PX 320
#define LCD_HEIGHT_PX 240

typedef enum GameState {
    GameState_Running = 0,
    GameState_Won = 1,
    GameState_Lost = 2
} GameState;

typedef struct Minefield {
    int8_t numMines;
    int8_t fieldWidth;
    int8_t fieldHeight;

    int8_t xOff;
    int8_t yOff;

    int totalVisible;
    int totalNonMineTiles;
    int8_t numFlags;

    int8_t** visibleField;
    int8_t** mines;

    bool fieldsGenerated;

    GameState gameState;
} Minefield;

Minefield* minefield_create(int8_t width, int8_t height, int8_t numMines);
void minefield_delete(Minefield* minefield);

void minefield_cascade(Minefield* minefield, int8_t x, int8_t y);

void minefield_draw_tile(Minefield* minefield, uint8_t* tile, int8_t i, int8_t j);
uint8_t* minefield_get_tile(int8_t tileNum);
void minefield_draw_visible_tile(Minefield* minefield, int8_t i, int8_t j);

void minefield_draw_in_game_field(Minefield* minefield);
void minefield_draw_demo_field(Minefield* minefield);
void minefield_draw_win_field(Minefield* minefield);
void minefield_draw_die_field(Minefield* minefield, int8_t x, int8_t y);

#endif