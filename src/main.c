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
#include <graphx.h>

#include "main.h"
#include "gfx/gfx.h"
#include "key_helper.h"
#include "menu.h"
#include "minefield.h"
#include "settings.h"

int main(void) {
    Menu* main_menu;
    Settings settings;
    settings.height = 8;
    settings.width = 8;
    settings.num_mines = 10;
    
    Key_init();
    gfx_Begin();
    
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetTransparentColor(0);
    gfx_FillScreen(BACKGROUND_COLOR);

    main_menu = menu_create(4, NULL);
    
    main_menu->ExtraFunction = draw_demo_board;
    
    main_menu->XLocation = 130;
    main_menu->YLocation = 188;
    
    main_menu->TextBackgroundColor = BACKGROUND_COLOR;
    main_menu->TextForegroundColor = FOREGROUND_COLOR;
    main_menu->ClearColor = BACKGROUND_COLOR;
    main_menu->BackKey = Key_Clear;
    main_menu->Tag = &settings;

    main_menu->ClearScreen = false;
    
    main_menu->Items[0].Name = "Play";
    main_menu->Items[0].Function = main_game_loop;
    main_menu->Items[1].Name = "Difficulty";
    main_menu->Items[1].Function = settings_setup;
    main_menu->Items[2].Name = "Help";
    main_menu->Items[2].Function = print_help_text;
    main_menu->Items[3].Name = "Quit";
    main_menu->Items[3].Function = MENU_FUNCTION_BACK;

    menu_display(main_menu);
    menu_delete(main_menu);

    Key_reset();
    gfx_End();
    pgrm_CleanUp();
}

void draw_demo_board(MenuEventArgs* menuEventArgs) {
    if (menuEventArgs->FrameNumber % 5 == 0) {
        Minefield* minefield;
        minefield = minefield_create(8, 8, 10);
        minefield_draw_demo_field(minefield);
        minefield_delete(minefield);
    }

    gfx_TransparentSprite(logo1, 3, 9);
    gfx_TransparentSprite(logo2, 221, 9);

    gfx_PrintStringXY("Merthsoft  '24", 222, 230);
    gfx_PrintStringXY("v1.3", 290, 32);
}

void main_game_loop(MenuEventArgs* menuEventArgs) {
    int8_t x = 0;
    int8_t y = 0;
    int8_t old_x = 0;
    int8_t old_y = 0;
    bool redraw = true;
    bool flag = false;
    Minefield* minefield;
    Settings* settings;
    bool quit = false;
    
    settings = (Settings*)menuEventArgs->Menu->Tag;
    minefield = minefield_create(settings->width, settings->height, settings->num_mines);
    gfx_FillScreen(BACKGROUND_COLOR);

    int8_t prevFlags = -1;
    while (!quit) {
        if (prevFlags != minefield->numFlags) {
            gfx_SetColor(BACKGROUND_COLOR);
            gfx_FillRectangle_NoClip(0, 0, 320, 8);
            gfx_SetTextXY(1, 1);
            gfx_PrintString("Mines:  ");
            gfx_PrintInt(minefield->numFlags, 2);
            gfx_PrintString(" / ");
            gfx_PrintInt(minefield->numMines, 2);
            prevFlags = minefield->numFlags;
        }
        if (redraw) {
            minefield_draw_in_game_field(minefield);
            redraw = false;
        }
        minefield_draw_tile(minefield, cursor, x, y);

        Key_scanKeys(0);

        old_x = x;
        old_y = y;

        if (Key_justPressed(Key_Up)) { y = y == 0 ? minefield->fieldHeight - 1 : y - 1; }
        else if (Key_justPressed(Key_Down)) { y = y == minefield->fieldHeight - 1 ? 0 : y + 1; }
        else if (Key_justPressed(Key_Left)) { x = x == 0 ? minefield->fieldWidth - 1 : x - 1; }
        else if (Key_justPressed(Key_Right)) { x = x == minefield->fieldWidth - 1 ? 0 : x + 1; }
        else if (Key_justPressed(Key_2nd)) {
            if (minefield->visibleField[x][y] != FLAGGED && minefield->visibleField[x][y] != 0) {
                minefield_cascade(minefield, x, y);

                if (minefield->gameState == GameState_Lost) {
                    die(minefield, x, y);
                    quit = true;
                } else if (minefield->gameState == GameState_Won) {
                    win_game(minefield);
                    quit = true;
                } else {
                    redraw = true;
                }
            }
        } else if (Key_justPressed(Key_Alpha)) {
            if (minefield->visibleField[x][y] == FILLED) {
                minefield->visibleField[x][y] = FLAGGED;
                minefield->numFlags++;
            } else if (minefield->visibleField[x][y] == FLAGGED) {
                minefield->visibleField[x][y] = QUESTION;
                minefield->numFlags--;
            } else if (minefield->visibleField[x][y] == QUESTION) {
                minefield->visibleField[x][y] = FILLED;
            }
            flag = true;
        } else if (Key_justPressed(Key_Clear) || Key_justPressed(Key_Mode)) {
            quit = true;
        }

        if (old_x != x || old_y != y || flag) {
            minefield_draw_visible_tile(minefield, old_x, old_y);
            flag = false;
        }
    }

    minefield_delete(minefield);
}

void print_string(char* string, uint16_t x, uint8_t* y, uint16_t indent) {
    char* string_copy;
    char* sub_string;
    size_t string_length;
    uint16_t drawX = x;
    uint16_t space_width;
    space_width = 8;//gfx_StringWidth(" ");

    string_length = strlen(string);
    string_copy = malloc(string_length);
    strcpy(string_copy, string);

    sub_string = strtok(string_copy, " ");
    while (sub_string != NULL) {
        unsigned int width;
        width = gfx_GetStringWidth(sub_string);
        if (drawX + width > LCD_WIDTH_PX - 5) {
            *y += 10;
            drawX = x + indent;
        }
        gfx_PrintStringXY(sub_string, drawX, *y);
        drawX += width + space_width;
        
        sub_string = strtok(NULL, " ");
    }
    *y += 10;

    free(string_copy);
}

void print_help_text(MenuEventArgs* _menuEventArgs) {
    uint8_t y = 1;
    gfx_FillScreen(BACKGROUND_COLOR);
    
    print_string("How to play:", 1, &y, 0);
    gfx_PrintStringXY("1.", 1, y);
    print_string("Uncover a mine, and the game ends.", 15, &y, 0);
    gfx_PrintStringXY("2.", 1, y);
    print_string("Uncover an empty square, and you keep playing.", 15, &y, 0);
    gfx_PrintStringXY("3.", 1, y);
    print_string("Uncover a number, and it tells you how many mines lay hidden in the eight surrounding squares--information you use to deduce which nearby squares are safe to click.", 15, &y, 0);
    
    print_string("", 1, &y, 0);
    print_string("Controls:", 1, &y, 0);
    print_string("Arrows - Move the cursor", 1, &y, 0);
    print_string("2nd - Uncover the spot at the cursor location", 1, &y, gfx_GetStringWidth("2nd-") + 16);
    print_string("Alpha - Flag the spot at the cursor location", 1, &y, gfx_GetStringWidth("Alpha-") + 16);
    print_string("Clear - Return to the title screen", 1, &y, 0);

    print_string("", 1, &y, 0);
    print_string("Advanced:", 1, &y, 0);
    print_string("If you press 2nd on an already uncovered spot that has a number of mines around it, and you have flagged the correct number of mines, all non-flagged spots around it will be uncovered.", 1, &y, 0);

    while (!Key_isDown(Key_Clear)) {
        Key_scanKeys(0);
    }

    return;
}

void win_game(Minefield* minefield) {
    minefield_draw_win_field(minefield);

    gfx_SetColor(BACKGROUND_COLOR);
    gfx_FillRectangle_NoClip(0, 0, 320, 8);
    gfx_SetTextXY(1, 1);
    gfx_PrintString("You win! :D Press enter.");

    do {
        Key_scanKeys(0);
    } while (!(Key_justPressed(Key_Del) || Key_justPressed(Key_Enter) || Key_justPressed(Key_Clear) || Key_justPressed(Key_2nd)));
}

void die(Minefield* minefield, int8_t cursorX, int8_t cursorY) {
    minefield_draw_die_field(minefield, cursorX, cursorY);
    
    gfx_SetColor(BACKGROUND_COLOR);
    gfx_FillRectangle_NoClip(0, 0, 320, 8);
    gfx_SetTextXY(1, 1);
    gfx_PrintString("You lose! D: Press enter.");

    do {
        Key_scanKeys(0);
    } while (!(Key_justPressed(Key_Del) || Key_justPressed(Key_Enter) || Key_justPressed(Key_Clear) || Key_justPressed(Key_2nd)));
}