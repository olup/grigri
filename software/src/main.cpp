#include <Arduino.h>
#include <ArduinoJson.h>
#include <Button2.h>

#include "./BLE/manager.h"
#include "./GlobalNav/GlobalNav.h"
#include "./define.h"
#include "./display/display.h"
#include "./file/file.h"
#include "./player/player.h"
#include "./settings/settings.h"
#include "./utils/utils.h"
#include "./wifi/wifi.h"

#define SOURCE_DIR "/grigri"
#define PACKS_DIR "packs/"

GlobalNav globalNav;
BLEManager bleManager;

Button2 btn1;
Button2 btn2;
Button2 btn3;
Button2 btn4;
Button2 btn5;
Button2 btn6;
Button2 btn7;

bool isPaused = false;

void play_selection() {
  if (globalNav.isStoryNode()) {
    display_set_bl(0);
  } else {
    display_set_bl(settings_get_brightness());
  }
  player_play(globalNav.getAudioPath().c_str());
  display_draw_bmp(globalNav.getImagePath().c_str());
}

void audio_eof_mp3(const char* info) {  // end of file
  Serial.println("End of playback");

  if (globalNav.isAutoPlay()) {
    Serial.println("Autoplay detected");
    globalNav.goToSelect();

    play_selection();
  };
}

void handleTapSelect(Button2& b) {
  if (isPaused) {
    player_togglePause();
    isPaused = false;
    return;
  }

  if (player_isPlaying() && globalNav.isStoryNode()) {
    player_togglePause();
    isPaused = true;
    return;
  }

  Serial.println("Action: Select");
  globalNav.goToSelect();

  play_selection();
}

void handleTapBack(Button2& b) {
  Serial.println("Action: Back");
  globalNav.goToBack();

  play_selection();
}

void handleTapNext(Button2& b) {
  Serial.println("Action: Next");

  if (globalNav.isStoryNode() && player_isPlaying()) {
    Serial.println("Playback: +10s");
    player_moveRelative(10);
    return;
  }

  globalNav.goToNext();
  play_selection();
}

void handleTapPrevious(Button2& b) {
  Serial.println("Action: Previous");

  if (globalNav.isStoryNode() && player_isPlaying()) {
    Serial.println("Playback: -10s");
    player_moveRelative(-10);
    return;
  }

  globalNav.goToPrevious();

  play_selection();
}

void handleVolumeUp(Button2& b) {
  player_setVolume(player_get_volume() + 1);
  settings_set_volume(player_get_volume());
}
void handleVolumeDown(Button2& b) {
  player_setVolume(player_get_volume() - 1);
  settings_set_volume(player_get_volume());
}

void handleSwitchOff(Button2& b) {
  // todo: do we need to power things down ? (amp, sd card, lcd, etc.)
  Serial.println("Switch off");
  display_set_bl(0);
  delay(5000);

  esp_sleep_enable_ext0_wakeup(WAKE_UP_BTN, 0);
  esp_deep_sleep_start();
}

void handleStartConfigMode(Button2& b) {
  Serial.println("Start config mode");
  bleManager.init();
}

void buttons_loop() {
  btn1.loop();
  btn2.loop();
  btn3.loop();
  btn4.loop();
  btn5.loop();
  btn6.loop();
  btn7.loop();
}

void setup() {
  Serial.begin(115200);

  // disable radio for power saving
  disableWiFi();
  bleManager.switchOff();

  // setup buttons
  btn1.begin(BTN_1);
  btn1.setTapHandler(handleTapPrevious);

  btn2.begin(BTN_2);
  btn2.setTapHandler(handleTapNext);

  btn3.begin(BTN_3);
  btn3.setTapHandler(handleTapBack);

  btn4.begin(BTN_4);
  btn4.setTapHandler(handleTapSelect);

  btn5.begin(BTN_5);
  btn5.setLongClickTime(3000);
  btn5.setLongClickDetectedHandler(handleSwitchOff);

  // switch on config mode (enable bluetooth)
  btn5.setDoubleClickHandler(handleStartConfigMode);

  btn6.begin(BTN_6);
  btn6.setTapHandler(handleVolumeUp);

  btn7.begin(BTN_7);
  btn7.setTapHandler(handleVolumeDown);

  // initialize components
  fs_init();
  settings_init();
  display_init();
  player_init();

  // setup volume and backlight
  display_set_bl(settings_get_brightness());
  player_setVolume(settings_get_volume());

  // initialize navigation
  String jsonPath = mergeSegments(SOURCE_DIR, "/", "packsIndex.json", nullptr);
  String json;
  int result = fs_read_file(jsonPath.c_str(), json);

  if (result != 0) {
    Serial.println("Error reading file");
    return;
  }

  String packsDirectoryPath = mergeSegments(SOURCE_DIR, "/", "packs", nullptr);

  globalNav.init(json.c_str(), packsDirectoryPath.c_str());

  play_selection();
}

void loop() {
  player_loop();
  buttons_loop();
}
