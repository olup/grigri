#pragma once

#include <ArduinoJson.h>

#include <vector>

#include "../file/file.h"
#include "../utils/utils.h"
#include "./StudioPackStructure.h"

// create a class StudioPack thatbextends the Pack class
class StudioPack {
 private:
  StoryData pack;

  StoryNode currentNode;
  StoryActionNode currentMenu;
  u_int currentMenuIndex;

  StoryNode findStageNode(const char *uuid) {
    Serial.print("Finding stage node with uuid: ");
    Serial.println(uuid);

    for (StoryNode node : pack.stageNodes) {
      const char *nodeUuid = node.uuid.c_str();
      if (nodeUuid != nullptr && strcmp(nodeUuid, uuid) == 0) {
        return node;
      }
    }
  }

  StoryActionNode findActionNode(const char *uuid) {
    Serial.print("Finding action node with uuid:");
    Serial.println(uuid);

    for (StoryActionNode node : pack.actionNodes) {
      Serial.println(node.id.c_str());

      const char *nodeUuid = node.id.c_str();  // todo check node uuid
      if (nodeUuid != nullptr && strcmp(nodeUuid, uuid) == 0) {
        return node;
      }
    }
  }

 public:
  const char *uuid;

  // override the init method
  void init(const char *packPath) {
    Serial.println("init studio pack");

    // read packindex and deserialize json
    String json;
    String packJsonFilePath =
        mergeSegments(packPath, "/", "story.json", nullptr);
    int result = fs_read_file(packJsonFilePath.c_str(), json);

    if (result != 0) {
      Serial.println("Error reading file");
      return;
    }
    // deserialize the json
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.f_str());
    }

    pack = initializeStoryDataFromJson(doc.as<JsonObject>());

    uuid = pack.stageNodes[0].uuid.c_str();

    Serial.print("STUDIO PACK UUID:");
    Serial.println(uuid);

    currentNode = pack.stageNodes[0];
  }

  const char *getAudio() { return currentNode.audio.c_str(); };

  const char *getImage() { return currentNode.image.c_str(); };

  const char *getCurrentNodeUuid() { return currentNode.uuid.c_str(); };

  const bool isCoverNode() {
    const char *currentNodeUuid = currentNode.uuid.c_str();
    Serial.println(currentNodeUuid);

    const char *firstNodeUuid = pack.stageNodes[0].uuid.c_str();
    Serial.println(firstNodeUuid);

    return strcmp(currentNodeUuid, firstNodeUuid) == 0;
  };

  const bool isStoryNode() { return currentNode.controlSettings.pause; };
  const bool isAutoPlay() { return currentNode.controlSettings.autoplay; }

  void goToSelect() {
    const bool canSelect = currentNode.controlSettings.ok;

    if (!canSelect && !isAutoPlay()) {
      Serial.println("No select action available");
      return;
    }

    const char *menuUuid = currentNode.okTransition.actionNode.c_str();
    const long menuIndex = currentNode.okTransition.optionIndex;

    currentMenu = findActionNode(menuUuid);
    currentMenuIndex = menuIndex;
    currentNode = findStageNode(currentMenu.options[currentMenuIndex].c_str());
  };

  void goToBack() {
    const bool canBack = !isCoverNode() && currentNode.controlSettings.home;
    if (!canBack) {
      return;
    }

    // check if backTransition is null, if so, return to the cover node
    if (currentNode.homeTransition.isNull) {
      currentNode = pack.stageNodes[0];
      return;
    }

    const char *menuUuid = currentNode.homeTransition.actionNode.c_str();
    const long menuIndex = currentNode.homeTransition.optionIndex;

    currentMenu = findActionNode(menuUuid);
    currentMenuIndex = menuIndex;
    currentNode = findStageNode(currentMenu.options[currentMenuIndex].c_str());
  };

  void goToPrevious() {
    const bool canPrevious =
        !isCoverNode() && currentNode.controlSettings.wheel;
    if (!canPrevious) {
      return;
    }

    int newPos = currentMenuIndex - 1;
    if (newPos < 0) {
      newPos = currentMenu.options.size() - 1;
    }

    const char *currentNodeUuid = currentMenu.options[newPos].c_str();

    currentMenuIndex = newPos;
    currentNode = findStageNode(currentNodeUuid);
  };

  void goToNext() {
    const bool canNext = !isCoverNode() && currentNode.controlSettings.wheel;
    if (!canNext) {
      return;
    }
    int newPos = currentMenuIndex + 1;
    if (newPos >= currentMenu.options.size()) {
      newPos = 0;
    }
    currentMenuIndex = newPos;
    const char *currentNodeUuid = currentMenu.options[newPos].c_str();
    currentNode = findStageNode(currentNodeUuid);
  };

  void goTo(const char *nodeUuid) { currentNode = findStageNode(nodeUuid); };
};