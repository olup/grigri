#include <WiFi.h>

#include <vector>

std::vector<String> listSsids() {
  // WiFi.begin();

  int numNetworks = WiFi.scanNetworks();
  std::vector<String> ssids;
  ssids.reserve(5);  // we only want to return 5 networks

  if (numNetworks == 0) {
    Serial.println("No WiFi networks found");
    return ssids;
  }

  Serial.print("Number of WiFi networks found: ");
  Serial.println(numNetworks);

  for (int i = 0; i < numNetworks && i < 5; i++) {
    String ssid = WiFi.SSID(i);
    ssids.push_back(ssid);

    Serial.print("SSID: ");
    Serial.println(ssid.c_str());
  }

  return ssids;
}

void enableWifi() { WiFi.mode(WIFI_STA); }

bool connectToWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 50) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
    return false;
  }

  Serial.println("Connected to WiFi");
  return true;
}

void disableWiFi() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
