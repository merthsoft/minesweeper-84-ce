#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdlib.h>

#include "key_helper.h"

#define FUNCTION_BACK (void*)1
#define FUNCTION_NONE (void*)0

typedef struct MenuItem {
    char* Name;
    void* Function;
    bool Selected;
    void* Tag;
} MenuItem;

typedef enum MenuSelectionType {
    MenuSelectionType_None,
    MenuSelectionType_Single,
    MenuSelectionType_Multiple
} MenuSelectionType;

typedef struct Menu {
    char* Title;
    MenuItem* Items;
    uint8_t NumItems;
    void* ExtraFunction;
    MenuSelectionType SelectionType;
    key_t BackKey;
    char CursorChar;
    void* Tag;
} Menu;

typedef struct MenuEventArgs {
    Menu* Menu;
    uint8_t Index;
    uint32_t FrameNumber;
    bool Back;
} MenuEventArgs;

char* BackString = "Back";

Menu* Menu_create(uint8_t numItems, const char* title);
void Menu_delete(Menu* menu);
int Menu_display(Menu* menu);

#endif MENU_H