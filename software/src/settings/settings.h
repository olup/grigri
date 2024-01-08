#pragma once

#include <Preferences.h>

extern int settings_volume;
extern int settings_brightness;
extern int settings_auto_off;
extern int settings_persist_nav;

extern String state_nav_pack;
extern String state_nav_node;
extern int state_nav_pos;

extern bool state_is_paused;
extern bool state_is_connect_mode;

void settings_init();

void settings_persist();
