#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "secrets.h"

// =====================
// Power Switch
// =====================
#define SWITCH_PIN 25

// =====================
// Display (ST7920)
// =====================
#define LCD_SCL 18
#define LCD_SDA 23

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(
    U8G2_R0,
    U8X8_PIN_NONE
);

// =====================
// Sensors
// =====================
#define MQ135_AO_PIN 34
#define DHT_PIN 27
#define DHT_TYPE DHT11

#define INTERVAL 5000

#define WIFI_TIMEOUT 15000 // 15 seconds
// Wi-Fi credentials live in secrets.h, which is gitignored.
// Copy secrets.h.example to secrets.h and fill it in.

AsyncWebServer server(80);
DHT dht(DHT_PIN, DHT_TYPE);

// =====================
// Queues
// =====================
#define NUM_RECORDED_VALUES 128
int m135queue[NUM_RECORDED_VALUES] = {0};
float temperatureQueue[NUM_RECORDED_VALUES] = {0};
float humidityQueue[NUM_RECORDED_VALUES] = {0};

// =====================
// Queue Helpers
// =====================
void pushToIntQueue(int value, int* queue) {
  for (int i = NUM_RECORDED_VALUES - 1; i > 0; i--) {
    queue[i] = queue[i - 1];
  }
  queue[0] = value;
}

void pushToFloatQueue(float value, float* queue) {
  for (int i = NUM_RECORDED_VALUES - 1; i > 0; i--) {
    queue[i] = queue[i - 1];
  }
  queue[0] = value;
}

// =====================
// Conversion
// =====================
float adcToPpm(int adc) {
  return map(adc, 0, 4095, 400, 5000);
}

// =====================
// Display Function
// =====================
void showToDisplay(int adc, float ppm, float temp, float hum) {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 12);
  u8g2.print("ADC: ");
  u8g2.print(adc);

  u8g2.setCursor(0, 24);
  u8g2.print("PPM: ");
  u8g2.print(ppm, 0);

  u8g2.setCursor(0, 42);
  u8g2.print("Temp: ");
  u8g2.print(temp, 1);
  u8g2.print(" C");

  u8g2.setCursor(0, 54);
  u8g2.print("Hum: ");
  u8g2.print(hum, 1);
  u8g2.print(" %");

  u8g2.sendBuffer();
}

// =====================
// Setup
// =====================
void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  dht.begin();

  Wire.begin(23, 18);
  u8g2.begin();

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  unsigned long wifiStartTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - wifiStartTime < WIFI_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi connected! IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("No Wi-Fi connection possible.");
    Serial.println("Continuing without Wi-Fi...");
  }

  // HTTP endpoint
  server.on("/mq135", HTTP_GET, [](AsyncWebServerRequest *request){
    int latestMQ135 = m135queue[0];
    float ppm = adcToPpm(latestMQ135); 

    String json = "{\"current_adc\": " + String(latestMQ135) + 
                  ", \"current_ppm\": " + String(ppm) + 
                  ", \"current_temperature\": " + String(temperatureQueue[0]) + 
                  ", \"current_humidity\": " + String(humidityQueue[0]) + 
                  ", \"queue\":[";
                  
    for (int i = 0; i < NUM_RECORDED_VALUES; i++) {
      json += "{\"adc\": " + String(m135queue[i]) + 
              ", \"ppm\": " + String(adcToPpm(m135queue[i])) + 
              ", \"temperature\": " + String(temperatureQueue[i]) + 
              ", \"humidity\": " + String(humidityQueue[i]) + "}";

      if (i < NUM_RECORDED_VALUES - 1 && m135queue[i+1] != 0) { 
        json += ", ";
      } else {
        break;
      }
    }
    json += "]}";

    request->send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Web server started.");
}

// =====================
// Loop
// =====================
void loop() {
  // Read switch
  bool switchOn = digitalRead(SWITCH_PIN) == LOW;

  if(!switchOn) {
    Serial.println("Power Switch off.");
    u8g2.clearBuffer();
    u8g2.sendBuffer();
  } else {
    int latestMQ135 = analogRead(MQ135_AO_PIN);
    pushToIntQueue(latestMQ135, m135queue);

    float latestTemperature = dht.readTemperature();
    float latestHumidity = dht.readHumidity();

    if (isnan(latestTemperature) || isnan(latestHumidity)) {
      Serial.println("Failed to read from DHT11");
    } else {
      pushToFloatQueue(latestTemperature, temperatureQueue);
      pushToFloatQueue(latestHumidity, humidityQueue);
    }

    float ppm = adcToPpm(latestMQ135);

    Serial.print("ADC: ");
    Serial.print(latestMQ135);
    Serial.print(" | PPM: ");
    Serial.print(ppm);
    Serial.print(" | Temp: ");
    Serial.print(latestTemperature);
    Serial.print(" | Hum: ");
    Serial.println(latestHumidity);

    // Display output
    showToDisplay(latestMQ135, ppm, latestTemperature, latestHumidity);
  }

  

  delay(INTERVAL);
}