#pragma once

#include <vector>

// Function to list available SSIDs
std::vector<String> listSsids();

// Function to connect to a WiFi network
bool connectToWiFi(const char* ssid, const char* password);

// Function to disable WiFi
void disableWiFi();
void enableWifi();
