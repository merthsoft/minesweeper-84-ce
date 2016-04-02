#include "settings.h"
#include "menu.h"

void set_difficulty(MenuEventArgs* menu_event_args) {
    Settings* settings = (Settings*)menu_event_args->Menu->Tag;
    int8_t widths[3] = { 8, 11, 16 };
    int8_t heights[3] = { 8, 11, 11};
    int8_t num_mines[3] = { 10, 20, 36 };

    settings->width = widths[menu_event_args->Index];
    settings->height = heights[menu_event_args->Index];
    settings->num_mines = num_mines[menu_event_args->Index];
}

void settings_setup(MenuEventArgs* menu_event_args) {
    Menu* menu;
    menu = menu_create(3, "Difficulty");
    menu->TextBackgroundColor = 0;
    menu->TextForegroundColor = 7;
    menu->ClearColor = 0;
    menu->BackKey = Key_Del;
    menu->Tag = menu_event_args->Menu->Tag;

    menu->SelectionType = MenuSelectionType_Single;

    menu->Items[0].Name = "Beginner";
    menu->Items[0].Function = set_difficulty;

    menu->Items[1].Name = "Intermediate";
    menu->Items[1].Function = set_difficulty;

    menu->Items[2].Name = "Expert";
    menu->Items[2].Function = set_difficulty;

    menu->Items[3].Name = "Back";
    menu->Items[3].Function = MENU_FUNCTION_BACK;
}