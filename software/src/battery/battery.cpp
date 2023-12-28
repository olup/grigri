#include "./battery.h"

#include "./define.h"

void battery_init() {
  // set battery pin as input
  pinMode(BATTERY_READ, INPUT);
  // setup pin battery charging stat
  pinMode(BATTERY_CHARGING_STAT, INPUT_PULLUP);
}

// rea battery level from adc
uint16_t get_battery_level_analog() {
  // read battery level from adc
  uint16_t battery_level = analogRead(BATTERY_READ);
  // set battery level
  return battery_level;
}

float voltage_minimum = 3.2 / 2;
float voltage_maximum = 4.2 / 2;
float analog_minimum = map(voltage_minimum, 0, 3.3, 0, 4095);
float analog_maximum = map(voltage_maximum, 0, 3.3, 0, 4095);

// with a voltage divider, value can go from 3.3 to 4.2
uint16_t get_battery_level_percent() {
  // read battery level from adc
  uint16_t battery_level = get_battery_level_analog();
  // convert battery level to percentage
  battery_level = map(battery_level, analog_minimum, analog_maximum, 0, 100);
  // set battery level
  return battery_level;
}

bool is_battery_charging() { return !!digitalRead(BATTERY_CHARGING_STAT); }