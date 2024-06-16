#ifndef MINEFIELD_H_
#define MINEFIELD_H_

#define minefield_get_visible_type(minefield, i, j) minefield_get_tile(minefield->visibleField[i*minefield->fieldWidth + j])
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
    uint8_t fieldWidth;
    uint8_t fieldHeight;
    uint8_t numMines;
    
    uint16_t gameTime;

    uint8_t xOff;
    uint8_t yOff;

    uint16_t totalVisible;
    uint16_t totalNonMineTiles;
    uint8_t numFlags;

    bool fieldsGenerated;
    
    GameState gameState;

    int8_t* visibleField;
    uint8_t* mines;
} Minefield;

Minefield* minefield_create(uint8_t width, uint8_t height, uint8_t numMines);
void minefield_delete(Minefield* minefield);

void minefield_randomize(Minefield* minefield, uint8_t x, uint8_t y);

void minefield_cascade(Minefield* minefield, uint8_t x, uint8_t y);

void minefield_draw_tile(Minefield* minefield, gfx_sprite_t* tile, uint8_t i, uint8_t j);
gfx_sprite_t* minefield_get_tile(int8_t tileNum);
void minefield_draw_visible_tile(Minefield* minefield, uint8_t i, uint8_t j);

void minefield_draw_in_game_field(Minefield* minefield);
void minefield_draw_demo_field(Minefield* minefield);
void minefield_draw_win_field(Minefield* minefield);
void minefield_draw_die_field(Minefield* minefield, uint8_t x, uint8_t y);

bool minefield_is_valid_save(const char* appVarName);
void minefield_save(Minefield* minefield, const char* appVarName);
Minefield* minefield_load(const char* appVarName);

#endif