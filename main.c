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

/* Shared libraries */
#include <lib\ce\graphc.h>

#include "main.h"
#include "sprites.h"
#include "key_helper.h"
#include "menu.h"

void main(void) {	
    bool firstRun = 1;
    int8_t x;
    int8_t y;
    int8_t old_x;
    int8_t old_y;
    bool redraw;
    bool flag;

    Key_init();
    gc_InitGraph();
    
    gc_SetPalette(sprites_pal, sizeof(sprites_pal));
    gc_SetTransparentColor(sprites_transpcolor_index);
    gc_FillScrn(0);

start:
    total = 0;
    x = 0;
    y = 0;
    old_x = x;
    old_y = y;
    numFlags = 0;
    CreateFields(0, 0);
    redraw = true;
    flag = false;

    while (!Key_isDown(Key_Del)) {
        if (redraw) {
            DrawField();
            redraw = false;
        }
        DrawTile(cursor, x, y);

        Key_scanKeys(200);

        old_x = x;
        old_y = y;

        if (Key_isDown(Key_Up)) { y = y == 0 ? fieldHeight - 1 : y - 1; }
        else if (Key_isDown(Key_Down)) { y = y == fieldHeight - 1 ? 0 : y + 1; }
        else if (Key_isDown(Key_Left)) { x = x == 0 ? fieldWidth - 1 : x - 1; }
        else if (Key_isDown(Key_Right)) { x = x == fieldWidth - 1 ? 0 : x + 1; }
        else if (Key_justPressed(Key_2nd)) {
            if (visibleField[x][y] != FLAGGED && visibleField[x][y] != 0) {
                int8_t num;
                num = Cascade(x, y, total, 1);
                if (num == -1) {
                    Die(x, y);
                    goto start;
                }
                total += num;
                if (total == fieldHeight*fieldWidth - numMines) {
                    Win();
                    goto start;
                }
                redraw = true;
            }
        } else if (Key_justPressed(Key_Alpha)) {
            if (visibleField[x][y] == FILLED) {
                visibleField[x][y] = FLAGGED;
                numFlags++;
            } else if (visibleField[x][y] == FLAGGED) {
                visibleField[x][y] = QUESTION;
                numFlags--;
            } else if (visibleField[x][y] == QUESTION) {
                visibleField[x][y] = FILLED;
            }
            flag = true;
        }

        if (old_x != x || old_y != y || flag) {
            DrawVisibleTile(old_x, old_y);
            flag = false;
        }
    }
        
    Key_reset();
    gc_CloseGraph();
    pgrm_CleanUp();
}

void DrawTile(uint8_t* tile, int8_t i, int8_t j) {
    gc_NoClipDrawTransparentSprite(tile, i * 20 + xOff, j * 20 + yOff, Tile_Width, Tile_Height);
}

void Win() {
    int8_t i, j;
    for (i = 0; i < fieldWidth; i++) {
        for (j = 0; j < fieldHeight; j++) {
            DrawVisibleTile(i, j);
            if (mineField[i][j]) {
                DrawTile(win, i, j);
            }
        }
    }
    //DefineStatusMessage("  Minesweeper!    You win! :D", 1, 0, 0);
    //DisplayStatusArea();
    //SetBottomText("                            New");

    do {
        Key_scanKeys(0);
    } while (!(Key_isDown(Key_Del) || Key_isDown(Key_Enter) || Key_isDown(Key_Clear)));
}

void Die(int8_t cursorX, int8_t cursorY) {
    int8_t i, j;
    for (i = 0; i < fieldWidth; i++) {
        for (j = 0; j < fieldHeight; j++) {
            DrawVisibleTile(i, j);
            if (mineField[i][j]) {
                if ((i == cursorX && j == cursorY) || (visibleField[i][j] == EXPLOSION)) {
                    DrawTile(explode, i, j);
                } else {
                    DrawTile(visibleField[i][j] == FLAGGED ? flagged : mine, i, j);
                }
            } else if (visibleField[i][j] == FLAGGED) {
                DrawTile(misflagged, i, j);
            }
        }
    }
    /*DefineStatusMessage("  Minesweeper!    You lose! D:", 1, 0, 0);
    DisplayStatusArea();
    SetBottomText("                            New");*/

    do {
        Key_scanKeys(0);
    } while (!(Key_isDown(Key_Del) || Key_isDown(Key_Enter) || Key_isDown(Key_Clear)));
}

int8_t Cascade(int8_t x, int8_t y, int8_t total, bool initialClick) {
    int8_t i, j;
    int8_t num;
    int8_t neighbors;
    int8_t cas;

    if (newGame == 1) {
        CreateFields(x, y);
        newGame = 0;
    }

    if (x < 0 || y < 0 || x >= fieldWidth || y >= fieldHeight) { return 0; }
    if (visibleField[x][y] == FLAGGED || visibleField[x][y] == 0) { return 0; }
    if (mineField[x][y] == 1) {
        visibleField[x][y] = EXPLOSION;
        return -1;
    }
    if (visibleField[x][y] > 0) {
        if (!initialClick) { return 0; }
        if (visibleField[x][y] != CountFlags(x, y)) { return 0; }
        num = 0;
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                cas = Cascade(x + i, y + j, num, 0);
                if (cas == -1) { return -1; } else { num += cas; }
            }
        }

        return num;
    }

    neighbors = CountNeighbors(x, y);
    visibleField[x][y] = neighbors;
    num = 1;

    if (neighbors == 0) {
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                num += Cascade(x + i, y + j, num, 0);
            }
        }
    }
    return num;
}

int8_t CountFlags(int8_t x, int8_t y) {
    int8_t flags = 0;
    int8_t i, j;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (!(x + i < 0 || y + j < 0 || x + i >= fieldWidth || y + j >= fieldHeight)) {
                flags += visibleField[x + i][y + j] == FLAGGED;
            }
        }
    }

    return flags;
}

int8_t CountNeighbors(int8_t x, int8_t y) {
    int8_t neighbors = 0;
    int8_t i, j;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (!(x + i < 0 || y + j < 0 || x + i >= fieldWidth || y + j >= fieldHeight))
                neighbors += mineField[x + i][y + j];
        }
    }

    return neighbors;
}

void CreateFields(int8_t x, int8_t y) {
    int8_t i, j;
    srand(rtc_Time());

    for (i = 0; i < fieldWidth; i++) {
        for (j = 0; j < fieldHeight; j++) {
            visibleField[i][j] = FILLED;
            mineField[i][j] = 0;
        }
    }

    for (i = 0; i < numMines; i++) {
        int mineX, mineY;
        do {
            mineX = rand() % fieldWidth;
            mineY = rand() % fieldHeight;
        } while (mineField[mineX][mineY] != 0 || (mineX == x && mineY == y));
        mineField[mineX][mineY] = 1;
    }
}

void DrawField() {
    int8_t i, j;
    // Inefficient? Sure. But I don't care.
    xOff = LCD_WIDTH_PX / 2 - (20 * fieldWidth) / 2;
    yOff = LCD_HEIGHT_PX / 2 - (20 * fieldHeight) / 2;
    for (i = 0; i < fieldWidth; i++) {
        for (j = 0; j < fieldHeight; j++) {
            DrawTile(getVisibleType(i, j), i, j);
        }
    }
}

void DrawVisibleTile(int8_t i, int8_t j) {
    DrawTile(getVisibleType(i, j), i, j);
}

uint8_t* getVisibleType(int8_t i, int8_t j) {
    switch (visibleField[i][j]) {
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
            return explode;
    }
}