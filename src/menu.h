#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

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
    uint8_t TextDisabledForegroundColor;
    const char* Title;

    uint8_t NumItems;
    MenuItem* Items;
    
    void* ExtraFunction;
    
    MenuSelectionType SelectionType;
    
    kb_lkey_t BackKey;
    kb_lkey_t AltBackKey;
    char CursorChar;
    
    void* Tag;
} Menu;

typedef struct MenuEventArgs {
    Menu* Menu;
    uint8_t Index;
    uint16_t FrameNumber;
    bool Back;
    bool Faded;
} MenuEventArgs;

Menu* menu_create(uint8_t numItems, const char* title);
void menu_delete(Menu* menu);
int menu_display(Menu* menu, bool doFadeIn);

#endif