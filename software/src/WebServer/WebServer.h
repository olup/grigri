#pragma once

#include "../file/file.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "HTTPClient.h"

void download(String download_url, String path) {
  Serial.println("Downloading file from url: " + download_url);
  Serial.println("Saving file to path: " + path);

  // Init SD card and create a new file
  File file = SD_MMC.open(path, FILE_WRITE);

  // Get file stream from internet
  HTTPClient download_handler;
  download_handler.begin(download_url);
  int httpCode = download_handler.GET();
  WiFiClient* stream = download_handler.getStreamPtr();

  // Download data and write into SD card
  size_t downloaded_data_size = 0;
  const size_t SIZE = download_handler.getSize();

  Serial.print("Downloading file of size: ");
  Serial.println(SIZE);

  while (downloaded_data_size < SIZE) {
    size_t available_data_size = stream->available();
    if (available_data_size > 0) {
      uint8_t* file_data = (uint8_t*)malloc(available_data_size);
      Serial.print("1");

      stream->readBytes(file_data, available_data_size);
      Serial.print("2");
      file.write(file_data, available_data_size);

      Serial.print("3");
      downloaded_data_size += available_data_size;
      Serial.print("4");
      free(file_data);

      Serial.print(".");
    }
  }

  file.close();
}

AsyncWebServer server(8000);

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

  server.on("/install", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("Handling read request");
    Serial.println(request->url());

    // get the file name from the request
    String fileUrl = request->arg("url");

    // download file to sd
    String packUuid = request->arg("uuid");
    String packPath = "/tmp/" + packUuid + ".tar";
    Serial.println("Downloading file to path: " + packPath);

    request->send(500, "text/plain", "Starting download");

    download(fileUrl, packPath);
  });

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
