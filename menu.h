#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdlib.h>

#include "key_helper.h"

#define MENU_FUNCTION_BACK (void*)1
#define MENU_FUNCTION_NONE (void*)0

typedef struct MenuItem {
    char* Name;
    void *Function;
    bool Selected;
    void* Tag;
} MenuItem;

typedef enum MenuSelectionType {
    MenuSelectionType_None,
    MenuSelectionType_Single,
    MenuSelectionType_Multiple
} MenuSelectionType;

typedef struct Menu {
    uint16_t XLocation;
    uint8_t YLocation;
        
    uint8_t ClearColor;
    bool ClearScreen;

    uint8_t TextForegroundColor;
    uint8_t TextBackgroundColor;
    char* Title;

    uint8_t NumItems;
    MenuItem* Items;
    
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

char* menu_back_string = "Back";

Menu* menu_create(uint8_t numItems, const char* title);
void menu_delete(Menu* menu);
int menu_display(Menu* menu);

#endif MENU_H