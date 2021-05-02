#include <HTTPClient.h>

const char* ssid = ""; // wifi ssid
const char* password = ""; // wifi password
const String apiBody = "https://api.cryptonator.com/api/ticker/";
const String doge = "doge";
const String usd = "usd";
const String uah = "uah";

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float getPrice(String base, String target) {
  String url = apiBody + base + "-" + target;
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      JsonObject obj = doc.as<JsonObject>();

      String base = obj["ticker"]["base"];
      float price = obj["ticker"]["price"];
      String target = obj["ticker"]["target"];
      http.end();

      return price;
    } else {
      Serial.println("Error on HTTP request");
      http.end();

      return 0.0;

    }
  }
  Serial.println("Wifi disconnected...");

  return 0.0;
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi..");
  }
  Serial.println("Connected to the Wi-Fi network");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D -> 128x64, 0x3C -> 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    float dogeToUsd = getPrice(doge, usd);
    float usdToDoge = getPrice(usd, doge);
    float dogeToUah = getPrice(doge, uah);

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.printf("%s -> %.4f %s ", "Doge", dogeToUsd, "$\n");

    display.setCursor(0, 10);
    display.printf("%s -> %.4f %s", "Usd", usdToDoge, "Doge\n");   

    display.setCursor(0, 20);
    display.printf("%s -> %.4f %s ", "Doge", dogeToUah, "UAH\n");

    display.display();
  }

  delay(10000); 
}
