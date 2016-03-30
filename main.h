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

#define Tile_Height 20
#define Tile_Width 20

#define MIN(a, b)	a < b ? a : b
#define MAX(a, b)	a > b ? a : b

#define FILLED -1
#define FLAGGED -2
#define QUESTION -3
#define EXPLOSION -4

#define MAX_WIDTH	19
#define MAX_HEIGHT	8

#define LCD_WIDTH_PX 320
#define LCD_HEIGHT_PX 240

int8_t numMines = 8;
int8_t fieldWidth = 8;
int8_t fieldHeight = 8;
bool dead = 0;
bool newGame = 1;

int8_t maxMines = MAX_WIDTH*MAX_HEIGHT - 1;
int8_t minMines = 5;
int8_t maxWidth = MAX_WIDTH;
int8_t minWidth = 5;
int8_t maxHeight = MAX_HEIGHT;
int8_t minHeight = 5;

int8_t xOff = 0;
int8_t yOff = 0;

int8_t total = 0;
int8_t numFlags = 0;

int8_t visibleField[MAX_WIDTH][MAX_HEIGHT];
int8_t mineField[MAX_WIDTH][MAX_HEIGHT];

#define DrawRectFill(x, y, w, h, color_index) gc_SetColorIndex(color_index); gc_NoClipRectangle(x, y, w, h);
#define DrawRect(x, y, w, h, color_index) gc_SetColorIndex(color_index); gc_NoClipRectangleOutline(x, y, w, h);

void DrawField();
void CreateFields(int8_t x, int8_t y);
int8_t Cascade(int8_t x, int8_t y, int8_t total, bool initialClick);
int8_t CountFlags(int8_t x, int8_t y);
int8_t CountNeighbors(int8_t x, int8_t y);
void DrawTile(uint8_t* tile, int8_t i, int8_t j);
uint8_t* getVisibleType(int8_t i, int8_t j);
void DrawVisibleTile(int8_t i, int8_t j);
void Die(int8_t cursorX, int8_t cursorY);
void Win();

#endif