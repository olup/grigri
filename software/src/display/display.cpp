#include "./display.h"

#include "../define.h"
#include "../utils/utils.h"

TFT_eSPI tft = TFT_eSPI();

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();  // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();  // MSB
  return result;
}

void display_fill_black() { tft.fillScreen(TFT_BLACK); }

void display_init() {
  // setup pwm for backlight
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LCD_BL, 0);

  // setup tft
  tft.begin();
  tft.setRotation(1);  // 0 & 2 Portrait. 1 & 3 landscape
  display_fill_black();
}

void display_set_bl(int power) {
  // map power to 0-255
  power = map(power, 0, 100, 255, 0);
  // set backlight power
  ledcWrite(0, power);
}
int display_get_bl() {
  int power = ledcRead(0);
  // map power to 0 - 100
  power = map(power, 0, 255, 100, 0);
  return power;
}

void display_draw_bmp(const char *filename) {
  display_fill_black();

  Serial.print("Displaying BMP at path: ");
  Serial.println(filename);

  // is the filename a bmp?
  if ((filename[strlen(filename) - 1] != 'p') ||
      (filename[strlen(filename) - 2] != 'm') ||
      (filename[strlen(filename) - 3] != 'b')) {
    Serial.println("Not a bmp file");
    return;
  }

  int16_t x, y;
  x = y = 0;

  fs::File bmpFS;

  // Open requested file on SD card
  bmpFS = SD_MMC.open(filename);

  if (!bmpFS) {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h;
  uint8_t r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
      // Allocate memory for the entire BMP image
      uint32_t dataSize = w * h * 3;
      uint8_t *bmpData = (uint8_t *)malloc(dataSize);

      if (!bmpData) {
        Serial.println("Memory allocation failed");
        bmpFS.close();
        return;
      }
      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);

      // Read the entire BMP file into the array
      bmpFS.seek(seekOffset);
      bmpFS.read(bmpData, dataSize);

      // Convert 24 to 16 bit colors
      uint16_t *pixelData = (uint16_t *)malloc(w * h * sizeof(uint16_t));

      for (uint32_t i = 0; i < dataSize; i += 3) {
        b = bmpData[i];
        g = bmpData[i + 1];
        r = bmpData[i + 2];
        uint32_t pixelIndex = (i / 3) % w + ((h - 1) - (i / 3) / w) * w;
        pixelData[pixelIndex] =
            ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
      }

      // Display the BMP image in one go
      tft.pushImage(0, 0, w, h, pixelData);

      // Free the allocated memory
      free(bmpData);
      free(pixelData);

      tft.setSwapBytes(oldSwapBytes);
    } else
      Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}
