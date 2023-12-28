#include "./File.h"

#include "../define.h"

void fs_init() {
  SD_MMC.setPins(CLK, CMD, D0, D1, D2, D3);
  if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No card attached");
    return;
  }

  Serial.println("Card Initialized");
}

int fs_read_file(const char* fileName, String& fileContent) {
  Serial.println("Reading file: " + String(fileName));
  // Open the file
  File file = SD_MMC.open(fileName);
  // Check if the file opened successfully
  if (!file) {
    return -1;  // Return -1 to indicate an error opening the file
  }
  // Read the file content into a String
  while (file.available()) {
    fileContent += char(file.read());
  }

  // Close the file
  file.close();

  return 0;  // Return 0 to indicate success
}

int fs_write_file(const char* fileName, char* fileContent) {
  // Open the file
  File file = SD_MMC.open(fileName, FILE_WRITE);

  // Check if the file opened successfully
  if (!file) {
    return -1;  // Return -1 to indicate an error opening the file
  }

  // Write the file content
  file.print(fileContent);

  // Close the file
  file.close();

  return 0;  // Return 0 to indicate success
}

uint64_t fs_sd_card_total_space() { return SD_MMC.totalBytes(); }
uint64_t fs_sd_card_used_space() { return SD_MMC.usedBytes(); }
