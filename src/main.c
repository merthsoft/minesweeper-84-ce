#include <graphx.h>
#include <keypadc.h>
#include <time.h>
#include <string.h>

#include "main.h"
#include "gfx/gfx.h"
#include "menu.h"
#include "minefield.h"
#include "settings.h"
#include "effects.h"

int copyright_shimmer = 0;
const char copyright[] = "Merthsoft  '24";

#define copyright_length 14

const char appvar[] = "MNFLD";

int main(void) {
    Menu* main_menu;
    Settings settings;
    settings.height = 8;
    settings.width = 8;
    settings.num_mines = 10;
    
    gfx_Begin();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_FillScreen(1);
    fadeout();
    gfx_FillScreen(BACKGROUND_COLOR);
    gfx_SetTransparentColor(0);
    
    main_menu = menu_create(5, NULL);
    
    main_menu->Items[0].Name = "New Game";
    main_menu->Items[0].Function = new_game;

    if (minefield_is_valid_save(appvar)) 
    {
        main_menu->Items[1].Name = "Continue";
        main_menu->Items[1].Function = continue_game;
    } else {
        main_menu->Items[1].Name = "<No saved game found>";
        main_menu->Items[1].Function = NULL;
    }
    main_menu->Items[2].Name = "Difficulty";
    main_menu->Items[2].Function = settings_setup;
    main_menu->Items[3].Name = "Help";
    main_menu->Items[3].Function = print_help_text;
    main_menu->Items[4].Name = "Quit";
    main_menu->Items[4].Function = MENU_FUNCTION_BACK;
    
    main_menu->ExtraFunction = draw_demo_board;
    
    main_menu->XLocation = 130;
    main_menu->YLocation = 178;
    
    main_menu->TextBackgroundColor = BACKGROUND_COLOR;
    main_menu->TextForegroundColor = FOREGROUND_COLOR;
    main_menu->TextDisabledForegroundColor = DISABLED_COLOR;
    main_menu->ClearColor = BACKGROUND_COLOR;
    main_menu->BackKey = kb_KeyClear;
    main_menu->AltBackKey = kb_KeyDel;
    main_menu->Tag = &settings;

    main_menu->ClearScreen = false;

    menu_display(main_menu, true);
    menu_delete(main_menu);

    gfx_End();

    while(kb_AnyKey());

    return 0;
}

#define RAINBOW_START 57
void draw_demo_board(MenuEventArgs* menuEventArgs) {
    if (menuEventArgs->FrameNumber % 10 == 0 || menuEventArgs->Faded) {
        Minefield* minefield;
        minefield = minefield_create(9, 7, 15);
        minefield->yOff -= 7;
        minefield_randomize(minefield, 0, 0);
        minefield_draw_demo_field(minefield);
        minefield_delete(minefield);
    } 
    
    if (menuEventArgs->FrameNumber % 125 == 0 || menuEventArgs->Faded) {
        copyright_shimmer = 0;
    }

    gfx_TransparentSprite(logo1, 3, 9);
    gfx_TransparentSprite(logo2, 221, 9);

    gfx_SetTextScale(1, 1);
    int w = gfx_GetStringWidth(copyright);

    for (int i = 0; i < copyright_length; i++)
    {
        int palette_index = (i + copyright_shimmer) % copyright_length;
        gfx_SetTextFGColor(palette_index + RAINBOW_START);
        gfx_SetTextXY(GFX_LCD_WIDTH / 2 - w / 2 + 8 * i, 2);
        gfx_PrintChar(copyright[i]);
    }

    if (copyright_shimmer <= copyright_length)
        copyright_shimmer++;

    gfx_SetTextFGColor(FOREGROUND_COLOR);
    gfx_PrintStringXY("v1.4b", 285, 32);
}

void run(MenuEventArgs* menuEventArgs, Minefield* minefield)
{
    fadeout();
    main_game_loop(minefield);
    if (minefield->gameState == GameState_Running && minefield->fieldsGenerated)
    {
        minefield_save(minefield, appvar);
        menuEventArgs->Menu->Items[1].Name = "Continue";
        menuEventArgs->Menu->Items[1].Function = continue_game;
    } else {
        menuEventArgs->Menu->Items[1].Name = "<No saved game found>";
        menuEventArgs->Menu->Items[1].Function = NULL;
    }
    minefield_delete(minefield);
    menuEventArgs->Faded = true;
}

void new_game(MenuEventArgs* menuEventArgs)
{
    Settings* settings = (Settings*)menuEventArgs->Menu->Tag;
    Minefield* minefield = minefield_create(settings->width, settings->height, settings->num_mines);
    
    run(menuEventArgs, minefield);
}

void continue_game(MenuEventArgs* menuEventArgs)
{
    Minefield* minefield = minefield_load(appvar);
    if (minefield == NULL)
    {
        Settings* settings = (Settings*)menuEventArgs->Menu->Tag;
        minefield = minefield_create(settings->width, settings->height, settings->num_mines);
    }

    run(menuEventArgs, minefield);
}

void main_game_loop(Minefield* minefield) {
    int8_t x = 0;
    int8_t y = 0;
    int8_t old_x = 0;
    int8_t old_y = 0;
    bool redraw = true;
    bool flag = false;
    bool quit = false;

    uint8_t width = minefield->fieldWidth;

    gfx_FillScreen(BACKGROUND_COLOR);

    int8_t prevFlags = -1;
    clock_t clockOffset = 0;
    
    bool fade = true;
    while(kb_AnyKey());
    bool keyPressed = false;
    uint32_t lastClock = clock() / CLOCKS_PER_SEC;
    while (!quit) {
        uint32_t clockSeconds = clock() / CLOCKS_PER_SEC;
        if (clockSeconds > lastClock || minefield->gameTime == 0)
        {
            if (minefield->gameTime < 65535) {
                minefield->gameTime += clockSeconds - lastClock;
            }
            lastClock = clockSeconds;

            gfx_SetColor(BACKGROUND_COLOR);
            gfx_FillRectangle_NoClip(160, 0, 160, 8);
            gfx_SetTextXY(240, 1);
            gfx_SetTextFGColor(FOREGROUND_COLOR);
            gfx_PrintString("Time:  ");
            gfx_PrintInt(minefield->gameTime, 5);
        }

        if (prevFlags != minefield->numFlags) {
            gfx_SetColor(BACKGROUND_COLOR);
            gfx_FillRectangle_NoClip(0, 0, 160, 8);
            gfx_SetTextXY(1, 1);
            gfx_SetTextFGColor(FOREGROUND_COLOR);
            gfx_PrintString("Mines:  ");
            gfx_PrintInt(minefield->numFlags, 2);
            gfx_PrintString(" / ");
            gfx_PrintInt(minefield->numMines, 2);
            prevFlags = minefield->numFlags;
        }
        if (redraw) {
            minefield_draw_in_game_field(minefield);
            redraw = false;
            if (fade) {
                fadein();
                fade = false;
            }
        }
        minefield_draw_tile(minefield, cursor, x, y);

        if (clockOffset == 0 || (!kb_AnyKey() && keyPressed)) {
            keyPressed = false;
            clockOffset = clock();
        }

        if (kb_AnyKey() && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC/ 32)) {
            clockOffset = clock();
            old_x = x;
            old_y = y;

            if (kb_IsDown(kb_KeyUp)) { y = y == 0 ? minefield->fieldHeight - 1 : y - 1; }
            else if (kb_IsDown(kb_KeyDown)) { y = y == minefield->fieldHeight - 1 ? 0 : y + 1; }
            else if (kb_IsDown(kb_KeyLeft)) { x = x == 0 ? width - 1 : x - 1; }
            else if (kb_IsDown(kb_KeyRight)) { x = x == width - 1 ? 0 : x + 1; }
            else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                if (minefield->visibleField[x*width +y] != FLAGGED && minefield->visibleField[x*width +y] != 0) {
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
            } else if (kb_IsDown(kb_KeyAlpha)) {
                if (minefield->visibleField[x*width +y] == FILLED) {
                    minefield->visibleField[x*width +y] = FLAGGED;
                    minefield->numFlags++;
                } else if (minefield->visibleField[x*width +y] == FLAGGED) {
                    minefield->visibleField[x*width +y] = QUESTION;
                    minefield->numFlags--;
                } else if (minefield->visibleField[x*width +y] == QUESTION) {
                    minefield->visibleField[x*width +y] = FILLED;
                }
                flag = true;
            } else if (kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyMode) || kb_IsDown(kb_KeyDel)) {
                quit = true;
            }

            if (old_x != x || old_y != y || flag) {
                minefield_draw_visible_tile(minefield, old_x, old_y);
                flag = false;
            }

            if (!keyPressed) {
                while (clock() - clockOffset < CLOCKS_PER_SEC / 4 && kb_AnyKey()) {
                    kb_Scan();
                }
            }

            keyPressed = true;
        }
    }

    fadeout();
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

void print_help_text(MenuEventArgs* menuEventArgs) {
    uint8_t y = 1;
    fadeout();
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

    fadein();

    kb_Scan();
    while (!kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }

    fadeout();
    menuEventArgs->Faded = true;

    return;
}

void waitForKeys() {
    while(kb_AnyKey());
    do {
        kb_Scan();
    } while (!(kb_IsDown(kb_KeyDel) || kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_KeyClear)));
}

void end_game(Minefield* minefield, int8_t cursorX, int8_t cursorY, bool isWin)
{
    if (isWin)
        minefield_draw_win_field(minefield);
    else
        minefield_draw_die_field(minefield, cursorX, cursorY);

    gfx_SetColor(BACKGROUND_COLOR);
    gfx_FillRectangle_NoClip(0, 0, 320, 8);
    gfx_SetTextXY(1, 1);

    if (isWin)
        gfx_PrintString("You win! :D Press enter.");
    else
        gfx_PrintString("You lose! D: Press enter.");

    gfx_SetColor(BACKGROUND_COLOR);
    gfx_FillRectangle_NoClip(160, 0, 160, 8);
    gfx_SetTextXY(240, 1);
    gfx_PrintString("Time:  ");
    gfx_PrintInt(minefield->gameTime, 5);

    waitForKeys();
}

void win_game(Minefield* minefield) {
    end_game(minefield, 0, 0, true);
}

void die(Minefield* minefield, int8_t cursorX, int8_t cursorY) {
    end_game(minefield, cursorX, cursorY, false);
}
