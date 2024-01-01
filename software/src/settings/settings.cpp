
#include "./settings.h"

Preferences preferences;

void settings_init(void) {
  Serial.println("settings_init");
  preferences.begin("settings", false);

  if (!preferences.getUChar("volume")) {
    preferences.putUChar("volume", 10);
  }
  if (!preferences.getUChar("brightness")) {
    preferences.putUChar("brightness", 50);
  }
}

// volume
void settings_set_volume(int volume) { preferences.putUChar("volume", volume); }

int settings_get_volume() { return preferences.getUChar("volume"); }

// brightness
void settings_set_brightness(int brightness) {
  preferences.putUChar("brightness", brightness);
}

int settings_get_brightness() { return preferences.getUChar("brightness"); }