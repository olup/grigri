// wifi_module.cpp

#include "./wifi.h"

bool WiFiModule::init() { return preferences.begin("wifiModule", false); }

std::vector<std::pair<String, bool>> WiFiModule::listAvailableNetworks() {
  std::vector<std::pair<String, bool>> availableNetworks;
  int numNetworks = WiFi.scanNetworks();

  for (int i = 0; i < numNetworks; i++) {
    String ssid = WiFi.SSID(i);
    bool passwordStored = preferences.isKey(ssid.c_str());

    availableNetworks.push_back(std::make_pair(ssid, passwordStored));
  }

  return availableNetworks;
}

bool WiFiModule::connectToWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 50) {
    delay(500);
    attempts++;
    Serial.print(".");
  }

  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");

    // Save the successful WiFi password to preferences
    String passwordString(password);
    size_t success = preferences.putString(ssid, passwordString);
    Serial.print("Password stored: ");
    Serial.println(success);

    return true;
  } else {
    return false;
  }
}

bool WiFiModule::connectToKnownWiFi(const char* ssid) {
  Serial.print("Connecting to known wifi :");
  Serial.println(ssid);

  String storedPassword = preferences.getString(ssid, "");

  if (storedPassword.length() > 0) {
    return connectToWiFi(ssid, storedPassword.c_str());
  } else {
    return false;  // No stored password for the provided SSID
  }
}

bool WiFiModule::tryConnectToAnyKnownWiFi() {
  std::vector<std::pair<String, bool>> knownNetworks = listAvailableNetworks();

  for (const auto& network : knownNetworks) {
    if (connectToKnownWiFi(network.first.c_str())) {
      return true;  // Connected to a known network
    }
  }

  return false;  // Failed to connect to any known network
}

void WiFiModule::forgetNetwork(const char* ssid) { preferences.remove(ssid); }

void WiFiModule::turnOnWiFi() {
  WiFi.mode(WIFI_STA);
  delay(100);
}

void WiFiModule::turnOffWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(100);
}
