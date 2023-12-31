#include "manager.h"

#include "../WebServer/WebServer.h"
#include "../battery/battery.h"
#include "../display/display.h"
#include "../file/file.h"
#include "../player/player.h"
#include "../settings/settings.h"
#include "../wifi/wifi.h"

void handleRequest(const char* message) {
  Serial.println("Handling request");
  Serial.println(message);

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, message);

  // Check for parsing errors
  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* commandName = doc["name"].as<const char*>();
  const char* commandId = doc["id"].as<const char*>();

  StaticJsonDocument<512> responseDoc;

  // Handle the command based on the "n" key value
  if (commandName) {
    // switch depending on the command name
    if (strcmp(commandName, "get_wifi_list") == 0) {
      Serial.println("get_wifi_list");

      enableWifi();
      std::vector<String> ssids = listSsids();

      responseDoc["type"] = "response";
      responseDoc["id"] = commandId;
      responseDoc["name"] = "wifi_list";

      int index = 0;
      for (const auto& ssid : ssids) {
        responseDoc["payload"]["ssids"][index] = ssid.c_str();
        index++;
      }

      std::string jsonString;
      serializeJson(responseDoc, jsonString);

      bleManager.sendUpdate(jsonString.c_str());
      disableWiFi();
    }

    if (strcmp(commandName, "do_wifi_connect") == 0) {
      Serial.println("do_wifi_connect");

      // get ssid and key from payload
      const char* ssid = doc["payload"]["ssid"].as<const char*>();
      const char* key = doc["payload"]["key"].as<const char*>();

      Serial.print("Connecting to ");
      Serial.println(ssid);

      enableWifi();
      connectToWiFi(ssid, key);

      int retries = 0;
      while (WiFi.status() != WL_CONNECTED && retries < 50) {
        delay(500);
        Serial.print(".");
        retries++;
      }

      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to WiFi");
      } else {
        Serial.println("Connected to WiFi");

        web_server_init();

        responseDoc["type"] = "response";
        responseDoc["id"] = commandId;
        responseDoc["payload"]["success"] = true;

        std::string jsonString;
        serializeJson(responseDoc, jsonString);

        bleManager.sendUpdate(jsonString.c_str());
      }
    }

    if (strcmp(commandName, "do_wifi_disconnect") == 0) {
      Serial.println("do_wifi_disconnect");
      disableWiFi();

      // reponse
      responseDoc["id"] = commandId;
      responseDoc["payload"]["success"] = "true";
      std::string jsonString;
      serializeJson(responseDoc, jsonString);
      bleManager.sendUpdate(jsonString.c_str());
    }

    if (strcmp(commandName, "get_status") == 0) {
      Serial.println("get_status");

      uint32_t ram = ESP.getFreeHeap();
      uint16_t battery = get_battery_level_percent();

      responseDoc["id"] = commandId;
      responseDoc["payload"]["battery"]["levelPercent"] = battery;
      responseDoc["payload"]["ram"] = ram;
      responseDoc["payload"]["sd"]["total"] = fs_sd_card_total_space();
      responseDoc["payload"]["sd"]["used"] = fs_sd_card_used_space();
      responseDoc["payload"]["volume"] = player_get_volume();
      responseDoc["payload"]["backlight"] = settings_get_brightness();

      bool isWifiConnected = WiFi.status() == WL_CONNECTED;
      responseDoc["payload"]["wifi"]["connected"] = isWifiConnected;
      if (isWifiConnected) {
        responseDoc["payload"]["wifi"]["ssid"] = WiFi.SSID();

        IPAddress ipAddress = WiFi.localIP();
        String ip = String(ipAddress[0]) + String(".") + String(ipAddress[1]) +
                    String(".") + String(ipAddress[2]) + String(".") +
                    String(ipAddress[3]);

        responseDoc["payload"]["wifi"]["ip"] = WiFi.localIP().toString();
      }

      std::string jsonString;
      serializeJson(responseDoc, jsonString);
      bleManager.sendUpdate(jsonString.c_str());
    }
  }
}

void handleRequestTask(void* pvParameters) {
  Serial.println("Handling request task");
  const char* message = (const char*)pvParameters;
  Serial.println(message);

  handleRequest(message);
  free((void*)message);
  vTaskDelete(NULL);
}

void MyCallbacks::onWrite(BLECharacteristic* pCharacteristic) {
  std::string value = pCharacteristic->getValue();

  char* copiedValue = new char[value.length() + 1];
  strcpy(copiedValue, value.c_str());
  Serial.println("Received message");

  Serial.println(copiedValue);

  xTaskCreatePinnedToCore(handleRequestTask,   // Function to implement the task
                          "handle_request",    // Name of the task
                          4096,                // Stack size (words, not bytes)
                          (void*)copiedValue,  // Task input parameter
                          1,                   // Priority of the task
                          NULL,                // Task handle
                          0  // Core to run the task on (0 or 1)
  );
};

void BLEManager::switchOff() { btStop(); }
void BLEManager::init() {
  btStart();

  BLEDevice::init("GRIGRI");
  BLEDevice::setMTU(512);
  pServer = BLEDevice::createServer();
  BLEService* pService = pServer->createService(serviceUUID);

  pInputCharacteristic = pService->createCharacteristic(
      inputCharacteristicUUID, BLECharacteristic::PROPERTY_READ |
                                   BLECharacteristic::PROPERTY_WRITE |
                                   BLECharacteristic::PROPERTY_NOTIFY |
                                   BLECharacteristic::PROPERTY_INDICATE);

  pInputCharacteristic->setCallbacks(new MyCallbacks());

  pOutputCharacteristic = pService->createCharacteristic(
      outputCharacteristicUUID, BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_NOTIFY |
                                    BLECharacteristic::PROPERTY_INDICATE);

  pInputCharacteristic->addDescriptor(new BLE2902());
  pOutputCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pServer->getAdvertising()->start();
}

void BLEManager::sendUpdate(const char* message) {
  Serial.println("Sending update");
  Serial.println(message);

  pOutputCharacteristic->setValue(message);
  pOutputCharacteristic->notify();
}