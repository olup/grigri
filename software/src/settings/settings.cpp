
#include "./settings.h"

Preferences preferences;

int settings_volume = 50;
int settings_brightness = 50;
int settings_auto_off = true;
int settings_persist_nav = true;

String state_nav_pack = "";
String state_nav_node = "";
int state_nav_pos = 0;

bool state_is_paused = false;
bool state_is_connect_mode = false;

void settings_init() {
  Serial.println("settings_init");
  preferences.begin("settings", false);

  // load values
  settings_volume = preferences.getUChar("volume", 50);
  settings_brightness = preferences.getUChar("brightness", 50);
  settings_auto_off = preferences.getBool("autoOff", true);
  settings_persist_nav = preferences.getBool("persistNav", true);

  state_nav_pack = preferences.getString("navPack", "");
  state_nav_node = preferences.getString("navNode", "");
  state_nav_pos = preferences.getInt("navPos", 0);
}

void settings_persist() {
  Serial.println("Saving settings");

  preferences.putUChar("volume", settings_volume);
  preferences.putUChar("brightness", settings_brightness);
  preferences.putBool("autoOff", settings_auto_off);
  preferences.putBool("persistNav", settings_persist_nav);

  preferences.putString("navPack", state_nav_pack);
  preferences.putString("navNode", state_nav_node);
  preferences.putInt("navPos", state_nav_pos);
}
