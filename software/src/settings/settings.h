#include <Preferences.h>

struct NavigationPosition {
  String packUuid;
  String nodeUuid;
  uint32_t playbackPosition;
};

void settings_init(void);

void settings_set_volume(int volume);
int settings_get_volume();

void settings_set_brightness(int brightness);
int settings_get_brightness();

void settings_set_navigation_position(NavigationPosition navigationPosition);

NavigationPosition settings_get_navigation_position();