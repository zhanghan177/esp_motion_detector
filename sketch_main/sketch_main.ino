#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "global_state.h"
#include "event.h"

const int PIR_PIN = 13;  // pin number for PIR control
const int MAIN_LOOP_INTERVAL = 1000;  // sleep interval for main event loop. in ms
const unsigned long ROOM_IDLE_THRESHOLD = 15 * 60 * 1000;  // in ms
const bool ENABLE_TURN_ON_SWITCH = false;  // Currently, we only want to use ESP to automatically turn off switch
const bool ENABLE_TURN_OFF_SWITCH = true;  //

// Vairables will change
SemaphoreHandle_t mutex;
global_state_t global_state;
unsigned long room_empty_duration;
long PIR_reading;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  connectToWiFiPSK();

  time_management_setup();

  // Initialize global states
  global_state = STATE_EMPTY_LONG_TIME;

  mutex = xSemaphoreCreateMutex();

  lock(mutex);
  reset_PIR();

  // Intialize the PIR pin as an input
  pinMode(PIR_PIN, INPUT_PULLUP);
  // Attach interrupt service routine to PIR sensor3
  attachInterrupt(digitalPinToInterrupt(PIR_PIN),
                  motion_sensor_ISR, CHANGE);
  unlock(mutex);

  Serial.println("Setup complete.");
}

void loop() {
  event_t event = EVENT_NO_EVENT;
  unsigned long now = millis();

  lock(mutex);
  Serial.printf("Values: %ld, now: %lu\n", PIR_reading, now);

  switch (PIR_reading) {
    case LOW:
      switch (global_state) {
        case STATE_OCCUPIED:
          event = EVENT_PEOPLE_JUST_LEFT;
          break;
        case STATE_EMPTY:
          if (time_difference_meet_threshold(room_empty_duration, ROOM_IDLE_THRESHOLD)) {
            event = EVENT_ROOM_EMPTY_LONG_TIME;
          }
          break;
        case STATE_EMPTY_LONG_TIME:
          break;
      }
      break;
    case HIGH:
      event = EVENT_PEOPLE_ENTER;
      break;
  }
  reset_PIR();
  unlock(mutex);

  switch (event) {
    case EVENT_PEOPLE_ENTER:
      switch_turn_on();
      break;
    case EVENT_PEOPLE_JUST_LEFT:
      room_empty_duration = 0;
      break;
    case EVENT_ROOM_EMPTY_LONG_TIME:
      switch_turn_off();
      break;
  }

  if (global_state == STATE_EMPTY) room_empty_duration += MAIN_LOOP_INTERVAL;

  delay(MAIN_LOOP_INTERVAL);
}

/*
   Motion sensor interrupt service routine
*/
void motion_sensor_ISR() {
  long value = digitalRead(PIR_PIN);

  Serial.printf("ISR Interrupt! %ld\n", value);

  lock(mutex);
  if (value == HIGH) {
    // motion detected
    PIR_reading = HIGH;
  }
  unlock(mutex);
}
