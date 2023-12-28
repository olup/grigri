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

void log(const char* arg1, const char* arg2) {
  Serial.print(arg1);
  Serial.print(" ");
  Serial.println(arg2);
}
