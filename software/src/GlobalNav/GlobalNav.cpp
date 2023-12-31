#include "GlobalNav.h"

#include "../file/file.h"
#include "../utils/utils.h"

void GlobalNav::initCurrentPack() {
  const char *packUuid = packUuids.at(currentPackIndex).c_str();
  String currentPackPath = mergeSegments(packsPaths, "/", packUuid, nullptr);
  currentPack.init(currentPackPath.c_str());
}

GlobalNav::~GlobalNav() {
  if (packsPaths != nullptr) {
    free(const_cast<char *>(packsPaths));
  }
}

void GlobalNav::init(const char *packIndexPath,
                     const char *packsPathsArgument) {
  Serial.println("Init global nav");

  // read packindex and deserialize json
  String packIndexJson;
  int result = fs_read_file(packIndexPath, packIndexJson);
  if (result != 0) {
    Serial.println("Error reading file");
    return;
  }

  packsPaths = strdup(packsPathsArgument);

  // deserialize the json
  DynamicJsonDocument doc = DynamicJsonDocument(1000);

  DeserializationError err = deserializeJson(doc, packIndexJson);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.f_str());
  }

  // get the stageNodes array from the doc
  JsonArray packUuidsJson = doc.as<JsonArray>();
  for (JsonVariant packUuidJson : packUuidsJson) {
    packUuids.push_back(packUuidJson.as<String>());
  }

  currentPackIndex = 0;
  initCurrentPack();
}

const char *GlobalNav::getAudio() { return currentPack.getAudio(); }
const char *GlobalNav::getImage() { return currentPack.getImage(); }

String GlobalNav::getAudioPath() {
  String audioPath = mergeSegments(packsPaths, "/", currentPack.uuid, "/",
                                   "assets", "/", getAudio(), nullptr);
  return audioPath;
}

String GlobalNav::getImagePath() {
  String imagePath = mergeSegments(packsPaths, "/", currentPack.uuid, "/",
                                   "assets", "/", getImage(), nullptr);
  return imagePath;
}

void GlobalNav::goToSelect() { currentPack.goToSelect(); }

void GlobalNav::goToBack() { currentPack.goToBack(); }

void GlobalNav::goToPrevious() {
  if (!currentPack.isCoverNode()) {
    currentPack.goToPrevious();
    return;
  }
  int nextPackIndex = currentPackIndex - 1;
  if (nextPackIndex < 0) {
    nextPackIndex = packUuids.size() - 1;
  }

  currentPackIndex = nextPackIndex;
  initCurrentPack();
}

void GlobalNav::goToNext() {
  if (!currentPack.isCoverNode()) {
    currentPack.goToNext();
    return;
  }

  u_int nextPackIndex = currentPackIndex + 1;

  if (nextPackIndex >= packUuids.size()) {
    nextPackIndex = 0;
  }

  currentPackIndex = nextPackIndex;

  initCurrentPack();
}

const char *GlobalNav::getCurrentPackUuid() {
  return packUuids.at(currentPackIndex).c_str();
}
const char *GlobalNav::getCurrentNodeUuid() {
  return currentPack.getCurrentNodeUuid();
}

const bool GlobalNav::isAutoPlay() { return currentPack.isAutoPlay(); }
const bool GlobalNav::isStoryNode() { return currentPack.isStoryNode(); }
const bool GlobalNav::isCoverNode() { return currentPack.isCoverNode(); }
