// wifi_module.h

#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

#include <vector>

class WiFiModule {
 public:
  bool init();
  std::vector<std::pair<String, bool>> listAvailableNetworks();
  bool connectToWiFi(const char* ssid, const char* password);
  bool connectToKnownWiFi(const char* ssid);
  bool tryConnectToAnyKnownWiFi();
  void forgetNetwork(const char* ssid);
  void turnOnWiFi();
  void turnOffWiFi();

 private:
  Preferences preferences;
};

extern WiFiModule wifiModule;

#endif  // WIFI_MODULE_H
