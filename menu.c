#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <lib/ce/graphc.h>
#include <lib/ce/fileioc.h>

#include "key_helper.h"
#include "menu.h"
#include "menu_sprites.h"

Menu* Menu_create(uint8_t numItems, const char* title) {
    uint8_t i;

    Menu* menu = malloc(numItems * sizeof(Menu));
    menu->Items = malloc(numItems * sizeof(MenuItem));
    for (i = 0; i < numItems; i++) {
        menu->Items[i].Function = FUNCTION_NONE;
        menu->Items[i].Name = "";
        menu->Items[i].Selected = false;
        menu->Items[i].Tag = NULL;
    }
    menu->NumItems = numItems;

    menu->Title = title;
    menu->ExtraFunction = FUNCTION_NONE;
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

void Menu_delete(Menu* menu) {
    free(menu->Items);
    free(menu);
}

int Menu_display(Menu* menu) {
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

    if (menu->SelectionType != MenuSelectionType_None) {
        extraTextPadding = linePadding;
    }

    eventArgs = malloc(sizeof(MenuEventArgs));

    if (menu->ClearScreen) {
        gc_FillScrn(menu->ClearColor);
    }

    while (!back) {
        gc_SetTextColor(menu->TextForegroundColor | menu->TextBackgroundColor << 8);

        if (menu->Title != NULL) {
            gc_PrintStringXY(menu->Title, menu->XLocation + 2, menu->YLocation + 1);
            gc_SetColorIndex(menu->TextForegroundColor);
            gc_NoClipHorizLine(menu->XLocation + 1, menu->YLocation + 10, gc_StringWidth(menu->Title) + 5);
        }

        for (i = 0; i < menu->NumItems; i++) {
            gc_PrintStringXY(menu->Items[i].Name, menu->XLocation + textPadding + extraTextPadding, menu->YLocation + 3 + linePadding + linePadding * i);

            if (menu->SelectionType != MenuSelectionType_None && menu->Items[i].Function != FUNCTION_BACK) {
                if (menu->Items[i].Selected) {
                    previouslySelectedIndex = i;
                    selected = true;
                } else {
                    selected = false;
                }

                switch (menu->SelectionType) {
                    case MenuSelectionType_Single:
                        gc_NoClipDrawSprite(selected ? radiobutton_filled : radiobutton_empty, menu->XLocation + textPadding, menu->YLocation + 3 + linePadding + linePadding * i - 1, 9, 9);
                        break;
                    case MenuSelectionType_Multiple:
                        gc_NoClipDrawSprite(selected ? checkbox_checked : checkbox_empty, menu->XLocation + textPadding, menu->YLocation + 3 + linePadding + linePadding * i - 1, 9, 9);
                        break;
                }
            }
        }

        gc_SetTextXY(menu->XLocation + 2, menu->YLocation + 3 + linePadding * y);
        gc_PrintChar(menu->CursorChar);
        Key_scanKeys(0);
        old_y = y;
        
        if (menu->ExtraFunction != FUNCTION_NONE) {
            void(*func)(MenuEventArgs*) = menu->ExtraFunction;
            eventArgs->FrameNumber = frameNumber;
            eventArgs->Menu = menu;
            eventArgs->Index = y - 1;
            eventArgs->Back = false;

            func(eventArgs);
            y = eventArgs->Index + 1;
            frameNumber = eventArgs->FrameNumber;
            menu = eventArgs->Menu;
            back = eventArgs->Back;
        }

        if (Key_justPressed(Key_Up)) { y = y == 1 ? menu->NumItems : y - 1; }
        else if (Key_justPressed(Key_Down)) { y = y == menu->NumItems ? 1 : y + 1; }
        else if (Key_justPressed(Key_2nd) || Key_justPressed(Key_Enter)) {
            uint8_t index = y - 1;
            void(*func)(MenuEventArgs*) = menu->Items[index].Function;

            if (menu->SelectionType != MenuSelectionType_None && menu->Items[y - 1].Function != FUNCTION_BACK) {
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
                }
            } 
            
            if (func == FUNCTION_BACK) { back = true; }
            else if (func != FUNCTION_NONE) { 
                eventArgs->FrameNumber = frameNumber;
                eventArgs->Menu = menu;
                eventArgs->Index = index;
                eventArgs->Back = false;

                func(eventArgs);

                y = eventArgs->Index + 1;
                frameNumber = eventArgs->FrameNumber;
                menu = eventArgs->Menu;
                back = eventArgs->Back;
            }
            gc_FillScrn(menu->ClearColor);
        } else if (Key_justPressed(menu->BackKey)) {
            y = 0;
            back = true;
        }

        if (old_y != y) {
            gc_SetColorIndex(menu->ClearColor);
            gc_NoClipRectangle(menu->XLocation + 0, menu->YLocation + 3 + linePadding * old_y, 10, 8);
        }

        frameNumber++;
    }

    free(eventArgs);
    return y-1;
}