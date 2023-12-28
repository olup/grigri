#pragma once

#include "../file/file.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

AsyncWebServer server(8000);

void onPackUpload(AsyncWebServerRequest* request, String filename, size_t index,
                  uint8_t* data, size_t len, bool final) {
  static File fsUploadFile;

  if (!index) {
    // Create a new file to save the uploaded data
    request->_tempFile = SD_MMC.open("/tmp-pack.zip", "w");

    if (!request->_tempFile) {
      Serial.println("Failed to open file for writing");
      return request->send(500, "text/plain", "Internal Server Error");
    }
  }

  // Write the data to the file
  if (len) {
    request->_tempFile.write(data, len);
  }

  // If this is the last part of the data, close the file
  if (final) {
    request->_tempFile.close();
    Serial.println("File uploaded successfully");
  }
}

void web_server_init() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Hello, world");
  });

  // request to read a file from sd
  server.on("/read-file", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("Handling read request");
    Serial.println(request->url());

    // get the file name from the request
    String filePath = request->arg("p");
    // if file does not exist, return 404
    if (!SD_MMC.exists(filePath)) {
      request->send(404, "text/plain", "File not found");
      return;
    }

    AsyncWebServerResponse* response =
        request->beginResponse(SD_MMC, filePath, "text/plain");
    request->send(response);
  });

  // request to list all files in a directory
  server.on("/read-dir", HTTP_POST, [](AsyncWebServerRequest* request) {
    Serial.println("Handling list request");
    Serial.println(request->url());

    // get the directory path from the request
    String directoryPath = request->arg("p");

    // open the directory
    File directory = SD_MMC.open(directoryPath);

    // check if the directory opened successfully
    if (!directory) {
      request->send(404, "text/plain", "Directory not found");
      return;
    }

    // create a json array to store the file names
    StaticJsonDocument<512> doc;
    int index = 0;
    while (File file = directory.openNextFile()) {
      doc["files"][index] = file.name();
      index++;
    }

    // close the directory
    directory.close();

    // send the response
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on(
      "/packs/install", HTTP_POST,
      [](AsyncWebServerRequest* request) { request->send(200); }, onPackUpload);

  server.onNotFound([](AsyncWebServerRequest* request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404);
    }
  });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  server.begin();
}
