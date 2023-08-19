#include <Arduino.h>
#include <WiFi.h>

#include "tflite_handler.h"
#include "camera_handler.h"
#include "server_handler.h"

#include "WifiCreds.h" // WifiCreds.h is not included in the repo, you need to create it yourself
//const char *ssid = "xxx";
//const char *password = "xxx";

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  init_camera();
  init_TFLite();
  init_server();

  Serial.printf("Total heap: %d\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());

  pinMode(4, OUTPUT);
  ledcSetup(10, 5000, 8);
  ledcAttachPin(4, 10);
  ledcWrite(10, 6);
}

void loop(){}