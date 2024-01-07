#pragma once

#include <ArduinoJson.h>

#include <vector>

#include "../StudioPack/StudioPack.h"

class GlobalNav {
 private:
  // array of uuid string

  std::vector<String> packUuids;
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

  void goTo(const char *packUuid, const char *nodeUuid);

  const char *getCurrentPackUuid();
  const char *getCurrentNodeUuid();

  const bool isAutoPlay();
  const bool isStoryNode();
  const bool isCoverNode();
};