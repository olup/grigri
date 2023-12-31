#include "utils.h"

String mergeSegments(const char* segment, ...) {
  String mergedResult;

  // Concatenate the first segment
  mergedResult += segment;

  // Access the variable arguments
  va_list args;
  va_start(args, segment);

  // Concatenate each subsequent segment into the result String
  while ((segment = va_arg(args, const char*)) != nullptr) {
    mergedResult += segment;
  }

  // Clean up the variable arguments
  va_end(args);

  return mergedResult;
}

void printWakeupReason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }
}