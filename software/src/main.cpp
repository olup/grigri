#include <Arduino.h>
#include <ArduinoJson.h>
#include <Button2.h>

#include "./BLE/manager.h"
#include "./GlobalNav/GlobalNav.h"
#include "./define.h"
#include "./display/display.h"
#include "./file/file.h"
#include "./player/player.h"
#include "./power.h"
#include "./settings/settings.h"
#include "./utils/utils.h"
#include "./wifi/wifi.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"

#define SOURCE_DIR "/grigri"
#define PACKS_DIR "packs/"

GlobalNav globalNav;
BLEManager bleManager;
WiFiModule wifiModule;

Button2 btn1;
Button2 btn2;
Button2 btn3;
Button2 btn4;
Button2 btn5;
Button2 btn6;
Button2 btn7;

void play_selection() {
  state_is_paused = false;
  if (globalNav.isStoryNode()) {
    display_set_bl(0);
    cancelShutoff();
  } else {
    display_set_bl(settings_brightness);
    scheduleShutOff();
  }

  player_play(globalNav.getAudioPath().c_str());
  display_draw_image(globalNav.getImagePath().c_str());
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
  delaySwitchOff();

  if (state_is_paused) {
    player_togglePause();
    display_set_bl(0);
    state_is_paused = false;
    return;
  }

  if (player_isPlaying() && globalNav.isStoryNode()) {
    player_togglePause();
    display_pause();
    display_set_bl(settings_brightness);
    state_is_paused = true;
    return;
  }

  Serial.println("Action: Select");
  globalNav.goToSelect();

  play_selection();
}

void handleTapBack(Button2& b) {
  delaySwitchOff();

  Serial.println("Action: Back");

  if (globalNav.isCoverNode()) {
    return;
  }

  globalNav.goToBack();
  play_selection();
}

void handleTapNext(Button2& b) {
  delaySwitchOff();

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
  delaySwitchOff();

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
  delaySwitchOff();
  settings_volume = settings_volume + 10;
  if (settings_volume > 100) settings_volume = 100;
  player_setVolume(settings_volume);
}
void handleVolumeDown(Button2& b) {
  delaySwitchOff();
  settings_volume = settings_volume - 10;
  if (settings_volume < 0) settings_volume = 0;
  player_setVolume(settings_volume);
}

void handlePreSwitchOff(Button2& b) { display_set_bl(0); }

void handleSwitchOff(Button2& b) {
  delay(100);
  Serial.println("Switch off");

  state_nav_pack = globalNav.getCurrentPackUuid();
  state_nav_node = globalNav.getCurrentNodeUuid();
  state_nav_pos = player_get_position();

  settings_persist();

  switchOff();
}

void handleStartConfigMode(Button2& b) {
  Serial.println("Start config mode");
  state_is_connect_mode = true;
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
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout detector

  Serial.begin(115200);

  printWakeupReason();

  wifiModule.init();

  // disable radio for power saving
  wifiModule.turnOffWiFi();
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
  btn5.setLongClickHandler(handleSwitchOff);
  btn5.setLongClickDetectedHandler(handlePreSwitchOff);

  // switch on config mode (enable bluetooth)
  btn5.setDoubleClickHandler(handleStartConfigMode);

  btn6.begin(BTN_6);
  btn6.setTapHandler(handleVolumeUp);

  btn7.begin(BTN_7);
  btn7.setTapHandler(handleVolumeDown);

  // initialize components
  fs_init();
  settings_init();

  display_set_bl(0);
  display_init();

  player_init();

  // initialize navigation
  String jsonPath = mergeSegments(SOURCE_DIR, "/", "packsIndex.json", nullptr);
  String packsDirectoryPath = mergeSegments(SOURCE_DIR, "/", "packs", nullptr);

  globalNav.init(jsonPath.c_str(), packsDirectoryPath.c_str());

  // setup volume and backlight
  display_set_bl(settings_brightness);
  player_setVolume(settings_volume);

  // load navigation position
  if (settings_persist_nav && state_nav_pack.length() > 0 &&
      state_nav_node.length() > 0) {
    globalNav.goTo(state_nav_pack.c_str(), state_nav_node.c_str());

    play_selection();

    if (globalNav.isStoryNode()) {
      player_set_position(state_nav_pos);

      player_togglePause();
      state_is_paused = true;

      // display pause screen
      display_set_bl(settings_brightness);
      display_pause();
    }
  } else {
    play_selection();
  }
}

void loop() {
  player_loop();
  buttons_loop();
}
