#include "WiFi.h"
#include "WiFiConfig.h"

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
