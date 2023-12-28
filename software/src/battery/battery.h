#pragma once
#include <Arduino.h>

void battery_init();
uint16_t get_battery_level_analog();
uint16_t get_battery_level_percent();
bool is_battery_charging();