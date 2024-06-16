#include <fileioc.h>
#include <graphx.h>
#include <sys/rtc.h>
#include <string.h>

#include "minefield.h"
#include "gfx/gfx.h"

Minefield* minefield_create(uint8_t width, uint8_t height, uint8_t numMines) {
    uint8_t i, j;
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

    minefield->visibleField = malloc(width * height * sizeof(int8_t*));
    minefield->mines = malloc(width * height * sizeof(int8_t*));

    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            minefield->visibleField[i*width + j] = FILLED;
            minefield->mines[i*width + j] = 0;
        }
    }

    minefield->gameTime = 0;
    minefield->gameState = GameState_Running;
    return minefield;
}

void minefield_delete(Minefield* minefield) {
    free(minefield->visibleField);
    free(minefield->mines);

    free(minefield);
}

void minefield_draw_tile(Minefield* minefield, gfx_sprite_t* tile, uint8_t i, uint8_t j) {
    gfx_TransparentSprite(tile, i * TILE_WIDTH + minefield->xOff, j * TILE_HEIGHT + minefield->yOff);
}

uint8_t minefield_count_flags(Minefield* minefield, uint8_t x, uint8_t y) {
    uint8_t flags = 0;
    int8_t i, j;
    uint8_t width = minefield->fieldWidth;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (!(x + i < 0 || y + j < 0 || x + i >= minefield->fieldWidth || y + j >= minefield->fieldHeight)) {
                flags += minefield->visibleField[(x + i) * width + y + j] == FLAGGED;
            }
        }
    }

    return flags;
}

int8_t minefield_count_neighbors(Minefield* minefield, uint8_t x, uint8_t y) {
    int8_t neighbors = 0;
    int8_t i, j;
    uint8_t width = minefield->fieldWidth;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (x + i < 0 || y + j < 0) { continue; }
            if (x + i >= minefield->fieldWidth || y + j >= minefield->fieldHeight) { continue; }

            if (minefield->mines[(x + i) * width + y + j]) {
                neighbors++;
            }
        }
    }

    return neighbors;
}

bool minefield_cascade_internal(Minefield* minefield, uint8_t x, uint8_t y, bool initialClick) {
    int8_t i, j;
    int8_t neighbors;
    uint8_t width = minefield->fieldWidth;

    if (!minefield->fieldsGenerated) {
        minefield_randomize(minefield, x, y);
    }

    if (x < 0 || y < 0 || x >= minefield->fieldWidth || y >= minefield->fieldHeight) { return false; }
    if (minefield->visibleField[x*width + y] == FLAGGED || minefield->visibleField[x*width + y] == 0) { return false; }

    if (minefield->mines[x*width + y] == 1) {
        minefield->visibleField[x*width + y] = EXPLOSION;
        return true;
    }

    if (minefield->visibleField[x*width + y] > 0) {
        if (minefield->visibleField[x*width + y] != minefield_count_flags(minefield, x, y)) { return false; }
        if (!initialClick) { return false; }
        
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (minefield_cascade_internal(minefield, x + i, y + j, false)) { return true; }
            }
        }
        return false;
    }

    neighbors = minefield_count_neighbors(minefield, x, y);
    minefield->visibleField[x*width + y] = neighbors;
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

void minefield_cascade(Minefield* minefield, uint8_t x, uint8_t y) {
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

void minefield_randomize(Minefield* minefield, uint8_t x, uint8_t y) {
    srand(rtc_Time());
    uint8_t width = minefield->fieldWidth;

    minefield->fieldsGenerated = true;

    for (int i = 0; i < minefield->numMines; i++) {
        int mineX, mineY;
        do {
            mineX = rand() % minefield->fieldWidth;
            mineY = rand() % minefield->fieldHeight;
        } while (minefield->mines[mineX*width + mineY] != 0 || (mineX == x && mineY == y));
        minefield->mines[mineX*width + mineY] = 1;
    }
}

void minefield_draw_in_game_field(Minefield* minefield) {
    uint8_t i, j;
    
    for (i = 0; i < minefield->fieldWidth; i++) {
        for (j = 0; j < minefield->fieldHeight; j++) {
            minefield_draw_tile(minefield, minefield_get_visible_type(minefield, i, j), i, j);
        }
    }
}

void minefield_draw_demo_field(Minefield* minefield) {
    uint8_t i, j;
    uint8_t width = minefield->fieldWidth;
    
    for (i = 0; i < minefield->fieldWidth; i++) {
        for (j = 0; j < minefield->fieldHeight; j++) {            
            if (minefield->mines[i*width + j]) {
                minefield_draw_tile(minefield, mine, i, j);
            } else {
                minefield_draw_tile(minefield, minefield_get_tile(minefield_count_neighbors(minefield, i, j)), i, j);
            }
        }
    }
}

void minefield_draw_die_field(Minefield* minefield, uint8_t x, uint8_t y) {
    uint8_t i, j;
    uint8_t width = minefield->fieldWidth;

    for (i = 0; i < minefield->fieldWidth; i++) {
        for (j = 0; j < minefield->fieldHeight; j++) {
            minefield_draw_visible_tile(minefield, i, j);
            if (minefield->mines[i*width + j]) {
                if ((i == x && j == y) || (minefield->visibleField[i*width + j] == EXPLOSION)) {
                    minefield_draw_tile(minefield, explode, i, j);
                } else {
                    minefield_draw_tile(minefield, minefield->visibleField[i*width + j] == FLAGGED ? flagged : mine, i, j);
                }
            } else if (minefield->visibleField[i*width + j] == FLAGGED) {
                minefield_draw_tile(minefield, misflagged, i, j);
            }
        }
    }
}

void minefield_draw_win_field(Minefield* minefield) {
    uint8_t i, j;
    uint8_t width = minefield->fieldWidth;
    for (i = 0; i < minefield->fieldWidth; i++) {
        for (j = 0; j < minefield->fieldHeight; j++) {
            minefield_draw_visible_tile(minefield, i, j);
            if (minefield->mines[i*width + j]) {
                minefield_draw_tile(minefield, win, i, j);
            }
        }
    }
}

void minefield_draw_visible_tile(Minefield* minefield, uint8_t i, uint8_t j) {
    minefield_draw_tile(minefield, minefield_get_visible_type(minefield, i, j), i, j);
}

gfx_sprite_t* minefield_get_tile(int8_t tileNum) {
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

const char* MineMagicString = "MINESAVE_1";
#define     MineMagicStringLength 11

typedef struct SerializedMinefield {
    uint8_t fieldWidth;
    uint8_t fieldHeight;
    uint8_t numMines;

    uint16_t gameTime;
} SerializedMinefield;

bool minefield_is_valid_save(const char* appVarName) {
    ti_var_t file = ti_Open(appVarName, "r+");
    if (!file) { return false; }
    uint8_t* buffer = NULL;
    
    char magicStringBuffer[MineMagicStringLength];
    if (ti_Read(&magicStringBuffer, MineMagicStringLength, 1, file) != 1)
        goto load_error;

    if (strncmp(magicStringBuffer, MineMagicString, MineMagicStringLength) != 0)
        goto load_error;

    SerializedMinefield serializedMinefield;
    if (ti_Read(&serializedMinefield, sizeof(SerializedMinefield), 1, file) != 1)
        goto load_error;
    
    uint16_t boardsize = serializedMinefield.fieldWidth * serializedMinefield.fieldHeight;
    buffer = malloc(boardsize * sizeof(uint8_t));
    if (ti_Read(buffer, sizeof(uint8_t), boardsize, file) != boardsize)
        goto load_error;

    if (ti_Read(buffer, sizeof(int8_t), boardsize, file) != boardsize)
        goto load_error;

    free(buffer);
    ti_Close(file);

    return true;
    
load_error:
    if (buffer != NULL)
        free(buffer);
    ti_Close(file);
    ti_Delete(appVarName);
    return false;
}

void minefield_save(Minefield* minefield, const char* appVarName) {
    ti_var_t file = ti_Open(appVarName, "w");
    if (!file) { return; }

    ti_Write(MineMagicString, MineMagicStringLength, 1, file);
    ti_Write(minefield, sizeof(SerializedMinefield), 1, file);

    uint16_t boardsize = minefield->fieldWidth * minefield->fieldHeight;
    ti_Write(minefield->mines, sizeof(uint8_t), boardsize, file);
    ti_Write(minefield->visibleField, sizeof(int8_t), boardsize, file);

    if (!ti_IsArchived(file)) {
        ti_SetArchiveStatus(true, file);
    }

    ti_Close(file);
}

Minefield* minefield_load(const char* appVarName) {
    ti_var_t file = ti_Open(appVarName, "r+");
    if (!file) { return NULL; }
    
    Minefield* minefield = NULL;

    char magicStringBuffer[MineMagicStringLength];
    if (ti_Read(&magicStringBuffer, MineMagicStringLength, 1, file) != 1)
        goto load_error;

    if (strncmp(magicStringBuffer, MineMagicString, MineMagicStringLength) != 0)
        goto load_error;

    SerializedMinefield serializedMinefield;
    if (ti_Read(&serializedMinefield, sizeof(SerializedMinefield), 1, file) != 1)
        goto load_error;

    minefield = minefield_create(serializedMinefield.fieldWidth, serializedMinefield.fieldHeight, serializedMinefield.numMines);
    minefield->gameTime = serializedMinefield.gameTime;
    minefield->fieldsGenerated = true;

    uint16_t boardsize = minefield->fieldWidth * minefield->fieldHeight;
    ti_Read(minefield->mines, sizeof(uint8_t), boardsize, file);
    ti_Read(minefield->visibleField, sizeof(int8_t), boardsize, file);

    for (int i = 0; i < boardsize; i++) {
        if (minefield->visibleField[i] > 0) {
            minefield->totalVisible++;
        } else if (minefield->visibleField[i] == FLAGGED) {
            minefield->numFlags++;
        }
    }
    
load_error:
    if (!ti_IsArchived(file)) {
        ti_SetArchiveStatus(false, file);
    }

    ti_Close(file);
    ti_Delete(appVarName);
    return minefield;
}