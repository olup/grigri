
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

// navigationPosition
void settings_set_navigation_position(NavigationPosition navigationPosition) {
  preferences.putString("navPack", navigationPosition.packUuid);
  preferences.putString("navNode", navigationPosition.nodeUuid);
  preferences.putInt("navPos", navigationPosition.playbackPosition);

  Serial.print("settings_get_navigation_position: ");
  Serial.println(navigationPosition.playbackPosition);
}

NavigationPosition settings_get_navigation_position() {
  String navigationPackUuidStr = preferences.getString("navPack", "");
  String navigationNodeUuidStr = preferences.getString("navNode", "");
  int navigationPlaybackPosition = preferences.getInt("navPos", 0);

  Serial.print("settings_get_navigation_position: ");
  Serial.println(navigationPlaybackPosition);

  NavigationPosition navigationPosition = {
      navigationPackUuidStr, navigationNodeUuidStr, navigationPlaybackPosition};

  return navigationPosition;
}
