#include <Arduino.h>

#include "./display/display.h"
#include "define.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TickType_t xDelay5min = pdMS_TO_TICKS(1000 * 60 * 3);

void switchOff() {
  // todo: do we need to power things down ? (amp, sd card, lcd, etc.)
  Serial.println("Switch off");
  esp_sleep_enable_ext0_wakeup(WAKE_UP_BTN, 0);
  esp_deep_sleep_start();
}

TaskHandle_t autoOffTaskHandle = NULL;

void autoOffTask(void* parameter) {
  // Wait for the auto-off delay
  vTaskDelay(xDelay5min);

  // stop the device
  switchOff();

  // Delete this task
  vTaskDelete(NULL);
}

void scheduleShutOff() {
  // If the auto-off task is already running, delete it
  if (autoOffTaskHandle != NULL) {
    vTaskDelete(autoOffTaskHandle);
    autoOffTaskHandle = NULL;
  }

  // Start the auto-off task
  xTaskCreate(autoOffTask, "AutoOffTask", 10000, NULL, 1, &autoOffTaskHandle);
}

void delaySwitchOff() {
  // If no auto-off task is running, don't schedule a new one
  if (autoOffTaskHandle == NULL) return;

  // Delete the auto-off task, so we can schedule a new one
  vTaskDelete(autoOffTaskHandle);
  autoOffTaskHandle = NULL;

  // Start the auto-off task
  xTaskCreate(autoOffTask, "AutoOffTask", 10000, NULL, 1, &autoOffTaskHandle);
}

void cancelShutoff() {
  // If the auto-off task is running, delete it
  if (autoOffTaskHandle != NULL) {
    vTaskDelete(autoOffTaskHandle);
    autoOffTaskHandle = NULL;
  }
}