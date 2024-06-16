#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "menu.h"

typedef struct Settings {
    uint8_t width;
    uint8_t height;
    uint8_t num_mines;
    bool new_game;
} Settings;

void settings_setup(MenuEventArgs* menu_event_args);

#endif // !SETTINGS_H_