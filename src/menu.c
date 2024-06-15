#include <graphx.h>
#include <fileioc.h>
#include <keypadc.h>
#include <time.h>

#include "menu.h"
#include "menu_sprites.h"
#include "gfx/gfx.h"
#include "effects.h"

Menu* menu_create(uint8_t numItems, const char* title) {
    uint8_t i;

    Menu* menu = malloc(numItems * sizeof(Menu));
    menu->Items = malloc(numItems * sizeof(MenuItem));
    for (i = 0; i < numItems; i++) {
        menu->Items[i].Function = MENU_FUNCTION_NONE;
        menu->Items[i].Name = "";
        menu->Items[i].Selected = false;
        menu->Items[i].Tag = NULL;
    }
    menu->NumItems = numItems;

    menu->Title = title;
    menu->ExtraFunction = MENU_FUNCTION_NONE;
    menu->BackKey = 0;
    menu->CursorChar = 0x10;
    menu->Tag = NULL;

    menu->XLocation = 0;
    menu->YLocation = 0;
    menu->ClearScreen = true;
    menu->ClearColor = 255;
    menu->TextBackgroundColor = 255;
    menu->TextForegroundColor = 0;

    return menu;
}

void menu_delete(Menu* menu) {
    free(menu->Items);
    free(menu);
}

int menu_display(Menu* menu, bool doFadeIn) {
    uint8_t i;
    uint8_t y = 1;
    uint8_t old_y = 1;
    uint8_t linePadding = 10;
    uint8_t textPadding = 10;
    uint8_t extraTextPadding = 0;
    uint8_t previouslySelectedIndex = 0;
    bool selected = false;
    uint32_t frameNumber = 0;
    MenuEventArgs* eventArgs;
    bool back = false;
    bool faded = doFadeIn;
    void(*func)(MenuEventArgs*);

    if (menu->SelectionType != MenuSelectionType_None) {
        extraTextPadding = linePadding;
    }

    eventArgs = malloc(sizeof(MenuEventArgs));

    if (menu->ClearScreen) {
        gfx_FillScreen(menu->ClearColor);
    }

    while(kb_AnyKey());
    bool keyPressed = false;
    clock_t clockOffset = 0;
    while (!(kb_IsDown(kb_KeyClear) || back)) {
        gfx_SetTextFGColor(menu->TextForegroundColor | menu->TextBackgroundColor << 8);

        if (menu->Title != NULL) {
            gfx_PrintStringXY(menu->Title, menu->XLocation + 2, menu->YLocation + 1);
            gfx_SetColor(menu->TextForegroundColor);
            gfx_HorizLine_NoClip(menu->XLocation + 1, menu->YLocation + 10, gfx_GetStringWidth(menu->Title) + 5);
        }

        for (i = 0; i < menu->NumItems; i++) {
            gfx_PrintStringXY(menu->Items[i].Name, menu->XLocation + textPadding + extraTextPadding, menu->YLocation + 3 + linePadding + linePadding * i);

            if (menu->SelectionType != MenuSelectionType_None && menu->Items[i].Function != MENU_FUNCTION_BACK) {
                if (menu->Items[i].Selected) {
                    previouslySelectedIndex = i;
                    selected = true;
                } else {
                    selected = false;
                }

                switch (menu->SelectionType) {
                    case MenuSelectionType_Single:
                        gfx_TransparentSprite((gfx_sprite_t*)(selected ? radiobutton_filled : radiobutton_empty), menu->XLocation + textPadding, menu->YLocation + 3 + linePadding + linePadding * i - 1);
                        break;
                    case MenuSelectionType_Multiple:
                        gfx_TransparentSprite((gfx_sprite_t*)(selected ? checkbox_checked : checkbox_empty), menu->XLocation + textPadding, menu->YLocation + 3 + linePadding + linePadding * i - 1);
                        break;
                    case MenuSelectionType_None:
                        break;
                }
            }
        }

        gfx_SetTextXY(menu->XLocation + 2, menu->YLocation + 3 + linePadding * y);
        gfx_PrintChar(menu->CursorChar);
        
        
        if (menu->ExtraFunction != MENU_FUNCTION_NONE) {
            func = menu->ExtraFunction;
            eventArgs->FrameNumber = frameNumber;
            eventArgs->Faded = faded;
            eventArgs->Menu = menu;
            eventArgs->Index = y - 1;
            eventArgs->Back = false;

            func(eventArgs);

            y = eventArgs->Index + 1;
            frameNumber = eventArgs->FrameNumber;
            menu = eventArgs->Menu;
            back = eventArgs->Back;
            faded = eventArgs->Faded;
        }

        if (faded) {
            fadein();
            faded = false;
        }

        // If no key is pressed, reset timer and variable for keeping track of if a key is held down.
        if (clockOffset == 0 || (!kb_AnyKey() && keyPressed)) {
            keyPressed = false;
            clockOffset = clock();
        }
        
        // If no key is being held, allow an input
        // If a key is being held, introduce a small delay between inputs. In this example the delay is
        // CLOCKS_PER_SEC / 32, but it can be changed to what feels most natural.
        if (kb_AnyKey() && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            clockOffset = clock();

            old_y = y;
            if (kb_IsDown(kb_KeyUp)) { y = y == 1 ? menu->NumItems : y - 1; }
            else if (kb_IsDown(kb_KeyDown)) { y = y == menu->NumItems ? 1 : y + 1; }
            else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                uint8_t index = y - 1;
                func = menu->Items[index].Function;

                if (menu->SelectionType != MenuSelectionType_None && menu->Items[y - 1].Function != MENU_FUNCTION_BACK) {
                    switch (menu->SelectionType) {
                        case MenuSelectionType_Single:
                            if (index != previouslySelectedIndex) {
                                menu->Items[previouslySelectedIndex].Selected = false;
                                menu->Items[index].Selected = true;
                            }
                            break;
                        case MenuSelectionType_Multiple:
                            menu->Items[index].Selected = !menu->Items[index].Selected;
                            break;
                        case MenuSelectionType_None:
                            break;
                    }
                } 
                
                if (func == MENU_FUNCTION_BACK) { back = true; }
                else if (func != MENU_FUNCTION_NONE) { 
                    eventArgs->FrameNumber = frameNumber;
                    eventArgs->Menu = menu;
                    eventArgs->Index = index;
                    eventArgs->Back = false;

                    func(eventArgs);
                    while(kb_AnyKey());

                    y = eventArgs->Index + 1;
                    frameNumber = eventArgs->FrameNumber;
                    menu = eventArgs->Menu;
                    back = eventArgs->Back;
                    faded = eventArgs->Faded;
                }
                gfx_FillScreen(menu->ClearColor);
            } else if (kb_IsDown(menu->BackKey) || kb_IsDown(menu->AltBackKey)) {
                y = 0;
                back = true;
            }

            if (old_y != y) {
                gfx_SetColor(menu->ClearColor);
                gfx_FillRectangle_NoClip(menu->XLocation + 0, menu->YLocation + 3 + linePadding * old_y, 10, 8);
            }

            // If this is the first instance of an input after a release (the key has not already been held
            // down for some time), wait for a longer delay to ensure the user wants to continue holding down
            // the key and creating more inputs. In this example the delay is CLOCKS_PER_SEC / 4, but it can
            // be changed to what feels most natural.
            if (!keyPressed) {
                while (clock() - clockOffset < CLOCKS_PER_SEC / 4 && kb_AnyKey()) {
                    kb_Scan();
                }
            }

            // Now we know that the user is holding down a key (If not, we'll reset keyPressed back to false
            // at the beginning of the loop.
            keyPressed = true;
        }
        frameNumber++;
    }

    free(eventArgs);
    return y-1;
}