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
#include "minefield.h"
#include "settings.h"

void main(void) {
    Menu* main_menu;
    Settings settings;
    settings.height = 8;
    settings.width = 8;
    settings.num_mines = 10;
    
    Key_init();
    gc_InitGraph();
    
    gc_SetPalette(sprites_pal, sizeof(sprites_pal));
    gc_SetTransparentColor(sprites_transpcolor_index);
    gc_FillScrn(0);

    main_menu = menu_create(4, NULL);
    
    main_menu->ExtraFunction = draw_demo_board;
    
    main_menu->XLocation = 130;
    main_menu->YLocation = 188;
    
    main_menu->TextBackgroundColor = 0;
    main_menu->TextForegroundColor = 7;
    main_menu->ClearColor = 0;
    main_menu->BackKey = Key_Del;
    main_menu->Tag = &settings;

    main_menu->ClearScreen = 0;
    
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
    gc_CloseGraph();
    pgrm_CleanUp();
}

void draw_demo_board(MenuEventArgs* menuEventArgs) {
    if (menuEventArgs->FrameNumber % 5 == 0) {
        Minefield* minefield;
        minefield = minefield_create(8, 8, 10);
        minefield_draw_demo_field(minefield);
        minefield_delete(minefield);
    }

    gc_ClipDrawTransparentSprite(logo1, 3, 9, logo1_width, logo1_height);
    gc_ClipDrawTransparentSprite(logo2, 221, 9, logo2_width, logo2_height);

    gc_SetTextColor(7);
    gc_PrintStringXY("Merthsoft  '16", 222, 230);
    gc_PrintStringXY("v0.9", 290, 32);
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
    
    settings = (Settings*)menuEventArgs->Menu->Tag;
    minefield = minefield_create(settings->width, settings->height, settings->num_mines);
    gc_FillScrn(0);

    while (!Key_isDown(Key_Del)) {
        gc_SetTextColor(7);
        gc_SetTextXY(1, 1);
        gc_PrintString("Mines:  ");
        gc_PrintInt(minefield->numFlags, 2);
        gc_PrintString(" / ");
        gc_PrintInt(minefield->numMines, 2);
        if (redraw) {
            minefield_draw_in_game_field(minefield);
            redraw = false;
        }
        minefield_draw_tile(minefield, cursor, x, y);

        Key_scanKeys(200);

        old_x = x;
        old_y = y;

        if (Key_isDown(Key_Up)) { y = y == 0 ? minefield->fieldHeight - 1 : y - 1; } 
        else if (Key_isDown(Key_Down)) { y = y == minefield->fieldHeight - 1 ? 0 : y + 1; } 
        else if (Key_isDown(Key_Left)) { x = x == 0 ? minefield->fieldWidth - 1 : x - 1; }
        else if (Key_isDown(Key_Right)) { x = x == minefield->fieldWidth - 1 ? 0 : x + 1; } 
        else if (Key_justPressed(Key_2nd)) {
            if (minefield->visibleField[x][y] != FLAGGED && minefield->visibleField[x][y] != 0) {
                minefield_cascade(minefield, x, y);

                if (minefield->gameState == GameState_Lost) {
                    die(minefield, x, y);
                    return;
                }
                
                if (minefield->gameState == GameState_Won) {
                    win_game(minefield);
                    return;
                }
                redraw = true;
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
    space_width = 8;//gc_StringWidth(" ");

    string_length = strlen(string);
    string_copy = malloc(string_length);
    strcpy(string_copy, string);

    sub_string = strtok(string_copy, " ");
    while (sub_string != NULL) {
        unsigned int width;
        width = gc_StringWidth(sub_string);
        if (drawX + width > LCD_WIDTH_PX - 5) {
            *y += 10;
            drawX = x + indent;
        }
        gc_PrintStringXY(sub_string, drawX, *y);
        drawX += width + space_width;
        
        sub_string = strtok(NULL, " ");
    }
    *y += 10;

    free(string_copy);
}

void print_help_text(MenuEventArgs* menuEventArgs) {
    uint8_t y = 1;
    gc_FillScrn(0);
    gc_SetTextColor(7);
    
    print_string("How to play:", 1, &y, 0);
    gc_PrintStringXY("1.", 1, y);
    print_string("Uncover a mine, and the game ends.", 15, &y, 0);
    gc_PrintStringXY("2.", 1, y);
    print_string("Uncover an empty square, and you keep playing.", 15, &y, 0);
    gc_PrintStringXY("3.", 1, y);
    print_string("Uncover a number, and it tells you how many mines lay hidden in the eight surrounding squares--information you use to deduce which nearby squares are safe to click.", 15, &y, 0);
    
    print_string("", 1, &y, 0);
    print_string("Controls:", 1, &y, 0);
    print_string("Arrows - Move the cursor", 1, &y, 0);
    print_string("2nd - Uncover the spot at the cursor location", 1, &y, gc_StringWidth("2nd-") + 16);
    print_string("Alpha - Flag the spot at the cursor location", 1, &y, gc_StringWidth("Alpha-") + 16);
    print_string("Del - Return to the title screen", 1, &y, 0);

    print_string("", 1, &y, 0);
    print_string("Advanced:", 1, &y, 0);
    print_string("If you press 2nd on an already uncovered spot that has a number of mines around it, and you have flagged the correct number of mines, all non-flagged spots around it will be uncovered.", 1, &y, 0);

    while (!Key_isDown(Key_Del)) {
        Key_scanKeys(0);
    }

    return;
}

void win_game(Minefield* minefield) {
    minefield_draw_win_field(minefield);

    gc_SetTextColor(7);
    gc_SetTextXY(1, 1);
    gc_PrintString("You win! :D Press enter.");

    do {
        Key_scanKeys(0);
    } while (!(Key_justPressed(Key_Del) || Key_justPressed(Key_Enter) || Key_justPressed(Key_Clear) || Key_justPressed(Key_2nd)));
}

void die(Minefield* minefield, int8_t cursorX, int8_t cursorY) {
    minefield_draw_die_field(minefield, cursorX, cursorY);

    gc_SetTextColor(7);
    gc_SetTextXY(1, 1);
    gc_PrintString("You lose! D: Press enter.");

    do {
        Key_scanKeys(0);
    } while (!(Key_justPressed(Key_Del) || Key_justPressed(Key_Enter) || Key_justPressed(Key_Clear) || Key_justPressed(Key_2nd)));
}