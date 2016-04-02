#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "menu.h"

typedef struct Settings {
    int8_t width;
    int8_t height;
    int8_t num_mines;
} Settings;

void settings_setup(MenuEventArgs* menu_event_args);

#endif // !SETTINGS_H_