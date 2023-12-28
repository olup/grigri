#pragma once

#include <ArduinoJson.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

void handleRequest(const char* message);
void handleRequestTask(void* parameter);

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic);
};

class BLEManager {
 public:
  void switchOff();
  void init();                           // Initialization function
  void sendUpdate(const char* message);  // Function to send updates
 private:
  BLEServer* pServer = nullptr;
  BLECharacteristic* pInputCharacteristic = nullptr;
  BLECharacteristic* pOutputCharacteristic = nullptr;
  const char* serviceUUID = "c80c5dc9-297a-4fdd-9276-4a8c6ac103f6";
  const char* inputCharacteristicUUID = "9284a635-d211-4bb9-8058-50024029f95a";
  const char* outputCharacteristicUUID = "c61652a7-4f2d-4ad6-8ba1-50ae92d83576";
};

extern BLEManager bleManager;  // Declare the bleManager object