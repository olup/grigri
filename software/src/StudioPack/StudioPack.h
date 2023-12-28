#pragma once

#include <ArduinoJson.h>

#include "../utils/utils.h"

// create a class StudioPack thatbextends the Pack class
class StudioPack {
 private:
  StaticJsonDocument<50 * 1000> doc;
  JsonArray stageNodes;
  JsonArray actionNodes;

  JsonObject currentNode;
  JsonArray currentMenu;
  u_int currentMenuIndex;

  JsonObject findNode(const char *uuid, JsonArray nodes) {
    log("Finding node with uuid:", uuid);
    for (JsonObject node : nodes) {
      // get the uuid from the node
      const char *nodeUuid = node["uuid"].as<const char *>();
      if (nodeUuid != nullptr && strcmp(nodeUuid, uuid) == 0) {
        return node;
      }

      const char *nodeId = node["id"].as<const char *>();
      if (nodeId != nullptr && strcmp(nodeId, uuid) == 0) {
        return node;
      }
    }
  }

 public:
  const char *uuid;

  // override the init method
  void init(const char *json) {
    Serial.println("init studio pack");

    // deserialize the json
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.f_str());
    }

    // get the stageNodes array from the doc
    stageNodes = doc["stageNodes"].as<JsonArray>();
    // get the actionNodes array from the doc
    actionNodes = doc["actionNodes"].as<JsonArray>();

    Serial.println("Action nodes loaded length:");
    Serial.println(actionNodes.size());

    uuid = stageNodes[0]["uuid"].as<const char *>();
    log("STUDIO PACK UUID:", uuid);

    currentNode = stageNodes[0];
  }

  const char *getAudio() { return currentNode["audio"].as<const char *>(); };

  const char *getImage() { return currentNode["image"].as<const char *>(); };

  const char *getCurrentNodeUuid() {
    return currentNode["uuid"].as<const char *>();
  };

  const bool isCoverNode() {
    const char *currentNodeUuid = currentNode["uuid"].as<const char *>();
    const char *firstNodeUuid = stageNodes[0]["uuid"].as<const char *>();
    return strcmp(currentNodeUuid, firstNodeUuid) == 0;
  };
  const bool isStoryNode() {
    return currentNode["controlSettings"]["pause"].as<const bool>();
  };
  const bool isAutoPlay() {
    return currentNode["controlSettings"]["autoplay"].as<const bool>();
  }

  void goToSelect() {
    const bool canSelect = currentNode["controlSettings"]["ok"];
    if (!canSelect && !isAutoPlay()) {
      Serial.println("No select action available");
      return;
    }

    const char *menuUuid = currentNode["okTransition"]["actionNode"];
    const long menuIndex = currentNode["okTransition"]["optionIndex"];
    const JsonObject menuNode = findNode(menuUuid, actionNodes);

    currentMenu = menuNode["options"].as<JsonArray>();
    currentMenuIndex = menuIndex;

    currentNode = findNode(currentMenu[currentMenuIndex], stageNodes);
  };

  void goToBack() {
    const bool canBack =
        !isCoverNode() &&
        currentNode["controlSettings"]["home"].as<const bool>();
    if (!canBack) {
      return;
    }

    // check if backTransition is null, if so, return to the cover node
    if (currentNode["homeTransition"].isNull()) {
      currentNode = stageNodes[0];
      return;
    }

    const char *menuUuid = currentNode["homeTransition"]["actionNode"];
    const long menuIndex = currentNode["homeTransition"]["optionIndex"];

    const JsonObject menuNode = findNode(menuUuid, actionNodes);

    currentMenu = menuNode["options"].as<JsonArray>();
    currentMenuIndex = menuIndex;

    Serial.println(currentMenuIndex);

    currentNode = findNode(currentMenu[currentMenuIndex], stageNodes);
  };

  void goToPrevious() {
    const bool canPrevious =
        !isCoverNode() && currentNode["controlSettings"]["wheel"];
    if (!canPrevious) {
      return;
    }

    int newPos = currentMenuIndex - 1;
    if (newPos < 0) {
      newPos = currentMenu.size() - 1;
    }

    const char *currentNodeUuid = currentMenu[newPos].as<const char *>();

    currentMenuIndex = newPos;
    currentNode = findNode(currentNodeUuid, stageNodes);
  };

  void goToNext() {
    const bool canNext =
        !isCoverNode() && currentNode["controlSettings"]["wheel"];
    if (!canNext) {
      return;
    }

    int newPos = currentMenuIndex + 1;
    if (newPos >= currentMenu.size()) {
      newPos = 0;
    }
    Serial.println(newPos);

    currentMenuIndex = newPos;
    const char *currentNodeUuid = currentMenu[newPos].as<const char *>();
    Serial.println(currentNodeUuid);
    currentNode = findNode(currentNodeUuid, stageNodes);
  };
};