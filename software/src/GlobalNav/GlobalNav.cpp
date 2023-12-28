#include "GlobalNav.h"

#include "../StudioPack/StudioPack.h"
#include "../file/file.h"
#include "../utils/utils.h"

void GlobalNav::initCurrentPack() {
  const char *packUuid = packUuids[currentPackIndex];
  String currentPackJsonPath =
      mergeSegments(packsPaths, "/", packUuid, "/", "story.json", nullptr);
  String json;
  int result = fs_read_file(currentPackJsonPath.c_str(), json);
  if (result != 0) {
    Serial.println("Error reading file");
    return;
  }
  currentPack.init(json.c_str());
  log("Current pack uuid is: ", currentPack.uuid);
  log("Free ram", "");
}

GlobalNav::~GlobalNav() {
  if (packsPaths != nullptr) {
    free(const_cast<char *>(packsPaths));
  }
}

void GlobalNav::init(const char *packIndexJson,
                     const char *packsPathsArgument) {
  Serial.println("Init global nav");
  packsPaths = strdup(packsPathsArgument);
  log("Packs path: ", packsPaths);

  // deserialize the json
  DeserializationError err = deserializeJson(doc, packIndexJson);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.f_str());
  }

  // get the stageNodes array from the doc
  packUuids = doc.as<JsonArray>();

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
  return packUuids[currentPackIndex].as<const char *>();
}
const char *GlobalNav::getCurrentNodeUuid() {
  return currentPack.getCurrentNodeUuid();
}

const bool GlobalNav::isAutoPlay() { return currentPack.isAutoPlay(); }
const bool GlobalNav::isStoryNode() { return currentPack.isStoryNode(); }
const bool GlobalNav::isCoverNode() { return currentPack.isCoverNode(); }
