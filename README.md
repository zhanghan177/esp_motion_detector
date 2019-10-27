# ESP32 Motion Detector to Switch on/off Lamps

This project uses ESP32 board and a PIR motion sensor to detect whether the room is occupied. If the room is empty for a long time, the ESP32 board will issue switch-off the outlet command via IFTTT webhook.

## Setup

You need to update configurations in two files inside the `sketch_main` directory

```bash
cd sketch_main
cp IFTTT_config.h.example IFTTT_config.h
cp WiFiConfig.h.example WiFiConfig.h
```

Inside `IFTTT_config.h` file, you need to put in your credentials for IFTTT webhook.

Inside `WiFiConfig.h` file, you need to update your local WiFi credentials.


## Setup IFTTT Webhook

Follow Step 3 in this tutorial: https://www.instructables.com/id/ESP32RC522IFTTTHome-Security/

## Install

You need to install Arduino IDE and set it up to program with ESP32 board. You can find many tutorials online on how to do that.
