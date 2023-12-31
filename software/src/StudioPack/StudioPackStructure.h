#pragma once

#include <ArduinoJson.h>

#include <memory>
#include <vector>

struct Position {
  int x;
  int y;
};

struct Transition {
  bool isNull;
  String actionNode;
  int optionIndex;
};

struct ControlSettings {
  bool wheel;
  bool ok;
  bool home;
  bool pause;
  bool autoplay;
};

struct StoryNode {
  String uuid;
  String type;
  String name;
  Position position;
  String image;
  String audio;
  Transition okTransition;
  Transition homeTransition;
  ControlSettings controlSettings;
  bool squareOne;
};

struct StoryActionNode {
  String id;
  String name;
  Position position;
  std::vector<String> options;
};

struct StoryData {
  String format;
  String title;
  int version;
  String description;
  bool nightModeAvailable;
  std::vector<StoryNode> stageNodes;
  std::vector<StoryActionNode> actionNodes;
};

// External function to initialize StoryNode from JSON
inline StoryNode initializeStoryNodeFromJson(const JsonObject& json) {
  StoryNode node;
  node.uuid = json["uuid"].as<String>();
  node.type = json["type"].as<String>();
  node.name = json["name"].as<String>();
  node.position.x = json["position"]["x"].as<int>();
  node.position.y = json["position"]["y"].as<int>();
  node.image = json["image"].as<String>();
  node.audio = json["audio"].as<String>();

  if (json["okTransition"].isNull()) {
    node.okTransition.isNull = true;
  } else {
    node.okTransition.isNull = false;
    node.okTransition.actionNode =
        json["okTransition"]["actionNode"].as<String>();
    node.okTransition.optionIndex =
        json["okTransition"]["optionIndex"].as<int>();
  }

  if (json["homeTransition"].isNull()) {
    node.homeTransition.isNull = true;
  } else {
    node.homeTransition.isNull = false;
    node.homeTransition.actionNode =
        json["homeTransition"]["actionNode"].as<String>();
    node.homeTransition.optionIndex =
        json["homeTransition"]["optionIndex"].as<int>();
  }

  node.controlSettings.wheel = json["controlSettings"]["wheel"].as<bool>();
  node.controlSettings.ok = json["controlSettings"]["ok"].as<bool>();
  node.controlSettings.home = json["controlSettings"]["home"].as<bool>();
  node.controlSettings.pause = json["controlSettings"]["pause"].as<bool>();
  node.controlSettings.autoplay =
      json["controlSettings"]["autoplay"].as<bool>();
  node.squareOne = json["squareOne"].as<bool>();
  return node;
}

// External function to initialize StoryActionNode from JSON
inline StoryActionNode initializeStoryActionNodeFromJson(const JsonObject& json) {
  StoryActionNode actionNode;
  actionNode.id = json["id"].as<String>();
  actionNode.name = json["name"].as<String>();
  actionNode.position.x = json["position"]["x"].as<double>();
  actionNode.position.y = json["position"]["y"].as<double>();
  JsonArray optionsArray = json["options"].as<JsonArray>();
  for (JsonVariant option : optionsArray) {
    actionNode.options.push_back(option.as<String>());
  }
  return actionNode;
}

// External function to initialize StoryData from JSON
inline StoryData initializeStoryDataFromJson(const JsonObject json) {
  StoryData data;
  data.format = json["format"].as<String>();
  data.title = json["title"].as<String>();
  data.version = json["version"].as<int>();
  data.description = json["description"].as<String>();
  data.nightModeAvailable = json["nightModeAvailable"].as<bool>();

  JsonArray stageNodesArray = json["stageNodes"].as<JsonArray>();
  for (JsonVariant stageNode : stageNodesArray) {
    data.stageNodes.push_back(
        initializeStoryNodeFromJson(stageNode.as<JsonObject>()));
  }

  JsonArray actionNodesArray = json["actionNodes"].as<JsonArray>();
  for (JsonVariant actionNode : actionNodesArray) {
    data.actionNodes.push_back(
        initializeStoryActionNodeFromJson(actionNode.as<JsonObject>()));
  }

  return data;
}
