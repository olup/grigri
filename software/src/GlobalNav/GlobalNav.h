#pragma once

#include <ArduinoJson.h>

#include "../StudioPack/StudioPack.h"

class GlobalNav {
 private:
  StaticJsonDocument<10 * 1000> doc;
  JsonArray packUuids;
  int currentPackIndex;
  StudioPack currentPack;

  const char *packsPaths;

  void initCurrentPack();

 public:
  ~GlobalNav();

  void init(const char *packIndexJson, const char *packsPathsArgument);

  const char *getAudio();
  const char *getImage();

  String getAudioPath();
  String getImagePath();

  void goToSelect();
  void goToBack();
  void goToPrevious();
  void goToNext();

  const char *getCurrentPackUuid();
  const char *getCurrentNodeUuid();

  const bool isAutoPlay();
  const bool isStoryNode();
};