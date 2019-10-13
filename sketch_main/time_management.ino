#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long  estOffset_sec = -5 * 3600;
const int   daylightOffset_sec = 3600;

bool is_set = false;

void time_management_setup() {
  configTime(estOffset_sec, daylightOffset_sec, ntpServer);
  is_set = true;
}

int currentTime(struct tm* timeinfo) {
  if (!is_set) return -1;

  if (!getLocalTime(timeinfo)) {
    Serial.println("Failed to obtain time");
    return -1;
  }
  Serial.println(timeinfo, "%A, %B %d %Y %H:%M:%S");
  return 0;
}

bool time_difference_meet_threshold(unsigned long duration, unsigned long thresh) {
  return duration > thresh;
}
