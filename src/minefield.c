/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <debug.h>

/* Standard headers - it's recommended to leave them included */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lib\ce\graphc.h>

#include "minefield.h"
#include "sprites.h"

void minefield_create_fields(Minefield* minefield, int8_t x, int8_t y);

Minefield* minefield_create(int8_t width, int8_t height, int8_t numMines) {
    int8_t i, j;
    Minefield* minefield;

    minefield = malloc(sizeof(Minefield));
    minefield->fieldHeight = height;
    minefield->fieldWidth = width;
    minefield->numMines = numMines;
    
    minefield->xOff = LCD_WIDTH_PX / 2 - (TILE_WIDTH * width) / 2;
    minefield->yOff = LCD_HEIGHT_PX / 2 - (TILE_HEIGHT * height) / 2;

    minefield->totalVisible = 0;
    minefield->numFlags = 0;
    minefield->fieldsGenerated = false;

    minefield->totalNonMineTiles = width*height - numMines;

    minefield->visibleField = malloc(width * sizeof(int8_t*));
    minefield->mines = malloc(width * sizeof(int8_t*));

    for (i = 0; i < width; i++) {
        minefield->visibleField[i] = malloc(height);
        minefield->mines[i] = malloc(height);
        
        for (j = 0; j < height; j++) {
            minefield->visibleField[i][j] = FILLED;
            minefield->mines[i][j] = 0;
        }
    }

    minefield_create_fields(minefield, 0, 0);
    minefield->gameState = GameState_Running;
    return minefield;
}

void minefield_delete(Minefield* minefield) {
    int8_t i;

    for (i = 0; i < minefield->fieldWidth; i++) {
        free(minefield->visibleField[i]);
        free(minefield->mines[i]);
    }

    free(minefield->visibleField);
    free(minefield->mines);

    free(minefield);
}

void minefield_draw_tile(Minefield* minefield, uint8_t* tile, int8_t i, int8_t j) {
    gc_NoClipDrawTransparentSprite(tile, i * TILE_WIDTH + minefield->xOff, j * TILE_HEIGHT + minefield->yOff, TILE_WIDTH, TILE_HEIGHT);
}

int8_t minefield_count_flags(Minefield* minefield, int8_t x, int8_t y) {
    int8_t flags = 0;
    int8_t i, j;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (!(x + i < 0 || y + j < 0 || x + i >= minefield->fieldWidth || y + j >= minefield->fieldHeight)) {
                flags += minefield->visibleField[x + i][y + j] == FLAGGED;
            }
        }
    }

    return flags;
}

int8_t minefield_count_neighbors(Minefield* minefield, int8_t x, int8_t y) {
    int8_t neighbors = 0;
    int8_t i, j;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (x + i < 0 || y + j < 0) { continue; }
            if (x + i >= minefield->fieldWidth || y + j >= minefield->fieldHeight) { continue; }

            if (minefield->mines[x + i][y + j]) {
                neighbors++;
            }
        }
    }

    return neighbors;
}

bool minefield_cascade_internal(Minefield* minefield, int8_t x, int8_t y, bool initialClick) {
    int8_t i, j;
    int8_t neighbors;

    if (!minefield->fieldsGenerated) {
        minefield_create_fields(minefield, x, y);
    }

    if (x < 0 || y < 0 || x >= minefield->fieldWidth || y >= minefield->fieldHeight) { return false; }
    if (minefield->visibleField[x][y] == FLAGGED || minefield->visibleField[x][y] == 0) { return false; }

    if (minefield->mines[x][y] == 1) {
        minefield->visibleField[x][y] = EXPLOSION;
        return true;
    }

    if (minefield->visibleField[x][y] > 0) {
        if (minefield->visibleField[x][y] != minefield_count_flags(minefield, x, y)) { return false; }
        if (!initialClick) { return false; }
        
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (minefield_cascade_internal(minefield, x + i, y + j, false)) { return true; }
            }
        }
        return false;
    }

    neighbors = minefield_count_neighbors(minefield, x, y);
    minefield->visibleField[x][y] = neighbors;
    minefield->totalVisible += 1;
    
    if (neighbors == 0) {
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                minefield_cascade_internal(minefield, x + i, y + j, false);
            }
        }
    }
    return false;
}

void minefield_cascade(Minefield* minefield, int8_t x, int8_t y) {
    bool hit_mine;
    hit_mine = minefield_cascade_internal(minefield, x, y, true);
    if (hit_mine) {
        minefield->gameState = GameState_Lost;
    } else {
        if (minefield->totalVisible == minefield->totalNonMineTiles) {
            minefield->gameState = GameState_Won;
        }
    }
}

void minefield_create_fields(Minefield* minefield, int8_t x, int8_t y) {
    int8_t i, j;
    srand(rtc_Time());

    minefield->fieldsGenerated = true;

    for (i = 0; i < minefield->numMines; i++) {
        int mineX, mineY;
        do {
            mineX = rand() % minefield->fieldWidth;
            mineY = rand() % minefield->fieldHeight;
        } while (minefield->mines[mineX][mineY] != 0 || (mineX == x && mineY == y));
        minefield->mines[mineX][mineY] = 1;
    }
}

void minefield_draw_in_game_field(Minefield* minefield) {
    int8_t i, j;
    
    for (i = 0; i < minefield->fieldWidth; i++) {
        for (j = 0; j < minefield->fieldHeight; j++) {
            minefield_draw_tile(minefield, minefield_get_visible_type(minefield, i, j), i, j);
        }
    }
}

void minefield_draw_demo_field(Minefield* minefield) {
    int8_t i, j;
    
    for (i = 0; i < minefield->fieldWidth; i++) {
        for (j = 0; j < minefield->fieldHeight; j++) {            
            if (minefield->mines[i][j]) {
                minefield_draw_tile(minefield, mine, i, j);
            } else {
                minefield_draw_tile(minefield, minefield_get_tile(minefield_count_neighbors(minefield, i, j)), i, j);
            }
        }
    }
}

void minefield_draw_die_field(Minefield* minefield, int8_t x, int8_t y) {
    int8_t i, j;

    for (i = 0; i < minefield->fieldWidth; i++) {
        for (j = 0; j < minefield->fieldHeight; j++) {
            minefield_draw_visible_tile(minefield, i, j);
            if (minefield->mines[i][j]) {
                if ((i == x && j == y) || (minefield->visibleField[i][j] == EXPLOSION)) {
                    minefield_draw_tile(minefield, explode, i, j);
                } else {
                    minefield_draw_tile(minefield, minefield->visibleField[i][j] == FLAGGED ? flagged : mine, i, j);
                }
            } else if (minefield->visibleField[i][j] == FLAGGED) {
                minefield_draw_tile(minefield, misflagged, i, j);
            }
        }
    }
}

void minefield_draw_win_field(Minefield* minefield) {
    int8_t i, j;
    for (i = 0; i < minefield->fieldWidth; i++) {
        for (j = 0; j < minefield->fieldHeight; j++) {
            minefield_draw_visible_tile(minefield, i, j);
            if (minefield->mines[i][j]) {
                minefield_draw_tile(minefield, win, i, j);
            }
        }
    }
}

void minefield_draw_visible_tile(Minefield* minefield, int8_t i, int8_t j) {
    minefield_draw_tile(minefield, minefield_get_visible_type(minefield, i, j), i, j);
}

uint8_t* minefield_get_tile(int8_t tileNum) {
    switch (tileNum) {
        case FILLED:
            return filled;
        case FLAGGED:
            return flagged;
        case QUESTION:
            return question;
        case EXPLOSION:
            return explode;
        case 0:
            return zero;
        case 1:
            return one;
        case 2:
            return two;
        case 3:
            return three;
        case 4:
            return four;
        case 5:
            return five;
        case 6:
            return six;
        case 7:
            return seven;
        case 8:
            return eight;
        default:
            return NULL;
    }
}