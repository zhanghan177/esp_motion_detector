#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "WiFi.h"

#include "WiFiConfig.h"
#include "global_state.h"

const int PIR_PIN = 12;  // pin number for PIR control
const int MAIN_LOOP_INTERVAL = 5000;  // sleep interval for main event loop. in ms

// Vairables will change
volatile SemaphoreHandle_t mutex;
volatile global_state_t global_state = STATE_EMPTY;
volatile bool state_changed = false;
unsigned long people_left_at;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  connectToWiFiPSK();
  reset_PIR();

  mutex = xSemaphoreCreateMutex();
  lock(mutex);

  // Intialize the PIR pin as an input
  pinMode(PIR_PIN, INPUT);
  // Attach interrupt service routine to PIR sensor
  attachInterrupt(digitalPinToInterrupt(PIR_PIN),
                  motion_sensor_ISR, CHANGE);

  unlock(mutex);

  Serial.println("Setup complete.");
}

void reset_PIR() {
  global_state = STATE_EMPTY;  // reset global state
  state_changed = false;
}

void loop() {
  // put your main code here, to run repeatedly:
  bool people_enter = false;
  bool people_left = false;;
  lock(mutex);
  Serial.printf("Values: %d, %d\n", global_state, state_changed);
  if (state_changed) {
    switch (global_state) {
      case STATE_EMPTY:
        people_left = true;
        break;
      case STATE_OCCUPIED:
        people_enter = true;
        break;
    }
  }

  if (people_left) {
    people_left_at = millis();
  }

  reset_PIR();
  unlock(mutex);

  delay(MAIN_LOOP_INTERVAL);
}

void lock(SemaphoreHandle_t l) {
  while (xSemaphoreTake(l, (TickType_t) 10) != pdTRUE) {}
  return;
}

void unlock(SemaphoreHandle_t l) {
  xSemaphoreGive(l);
}

/*
   Motion sensor interrupt service routine
*/
void motion_sensor_ISR() {
  long value = digitalRead(PIR_PIN);
  lock(mutex);
  state_changed = true;
  if (value == HIGH) {
    // motion detected
    global_state = STATE_OCCUPIED;
  }
  unlock(mutex);
}

void connectToWiFiPSK()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFi_SSID, WiFi_password);
  Serial.print("Connecting to ");
  Serial.println(WiFi_SSID);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);

    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }

  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
}
