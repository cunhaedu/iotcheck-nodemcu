#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "config.h"

float getCurrentTemperature();
float getCurrentHumidity();
void httpRequest();
String makeRequest();

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("\nConnecting to: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  IPAddress ip = WiFi.localIP();
  Serial.println("\nwi-fi connected");
  Serial.print("IP address: ");
  Serial.println(ip);
}

void loop() {
  httpRequest();
  delay(10000);
}

void httpRequest() {
  String payload = makeRequest();

  if (!payload) {
    return;
  }

  Serial.println("RESPONSE: " + payload);
}

String makeRequest() {
  http.begin(client, String(BASE_URL) + "temperatures");
  http.addHeader("content-type", "application/x-www-form-urlencoded");

  float currentTemperature = getCurrentTemperature();
  float currentHumidity = getCurrentHumidity();

  String body = "temperature=" + String(currentTemperature) + 
    "&humidity=" + String(currentHumidity) + 
    "&sensor=" + SENSOR_ID + "&token=" + DEVICE_TOKEN;

  int httpCode = http.POST(body);

  if (httpCode < 0) {
    Serial.println("request error - " + httpCode);
    return "";
  }

  if (httpCode != HTTP_CODE_OK) {
    return "";
  }

  String response =  http.getString();
  http.end();

  return response;
}

float getCurrentTemperature() {
  int temperatureValue = analogRead(A0);
  float millivolts = (temperatureValue/1024.0) * 3300; 
  float celsius = millivolts/10;

  return celsius;
}

float getCurrentHumidity() {
  float initialValue = getCurrentTemperature();
  float currentHumidity = initialValue + 12;

  return currentHumidity;
}
