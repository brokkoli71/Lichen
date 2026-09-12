#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h>

// =====================
// Power Switch
// =====================
#define SWITCH_PIN 25   // INPUT_PULLUP -> reads LOW when switch is ON (closed to GND)

// =====================
// Display (SH1106 128x64, I2C)
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

// =====================
// Timing
// =====================
#define SENSOR_INTERVAL 5000UL   // how often we sample the sensors
#define GRAPH_INTERVAL  5000UL   // how long each graph mode is shown before cycling
#define LOOP_TICK       100UL    // main loop tick, keeps everything responsive
#define WIFI_TIMEOUT    15000UL  // 15 seconds before we give up on a connection attempt

const char* ssid = "WIFI name here";
const char* password = "WIFI password here";

AsyncWebServer server(80);
DHT dht(DHT_PIN, DHT_TYPE);

// =====================
// Queues (history for the graph)
// =====================
#define NUM_RECORDED_VALUES 128
int   m135queue[NUM_RECORDED_VALUES] = {0};
float temperatureQueue[NUM_RECORDED_VALUES] = {0};
float humidityQueue[NUM_RECORDED_VALUES] = {0};
int   sampleCount = 0; // how many of the slots above currently hold real data (0..NUM_RECORDED_VALUES)

// =====================
// Wifi state machine
// =====================
enum WifiState { WIFI_STATE_OFF, WIFI_STATE_CONNECTING, WIFI_STATE_CONNECTED, WIFI_STATE_FAILED };
WifiState wifiState = WIFI_STATE_OFF;
unsigned long wifiConnectStart = 0;

// =====================
// Switch edge detection (debounced)
// =====================
bool switchOn = false;
bool lastSwitchOn = false;
bool rawSwitchOnLast = false;
unsigned long lastSwitchChangeTime = 0;
#define DEBOUNCE_MS 50UL

// =====================
// Graph cycling
// =====================
enum GraphMode { GRAPH_TEMP = 0, GRAPH_PPM = 1, GRAPH_HUM = 2 };
GraphMode graphMode = GRAPH_TEMP;
unsigned long lastGraphSwitch = 0;

// =====================
// Latest readings (kept even between successful DHT reads, so the queues stay aligned)
// =====================
int   currentAdc = 0;
float currentTemp = 0;
float currentHum  = 0;
bool  haveValidReading = false;

unsigned long lastSensorRead = 0;

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
// Wifi handling (non-blocking)
// =====================
void startWifiConnect() {
  Serial.println("Starting Wi-Fi connection attempt...");
  // Note: no WiFi.mode() call here. Mode is set exactly once in setup() and never
  // toggled again - repeatedly tearing down/recreating the WiFi driver via mode()
  // is what was causing: assert failed: xQueueSemaphoreTake queue.c:1709 (( pxQueue ))
  WiFi.begin(ssid, password);
  wifiState = WIFI_STATE_CONNECTING;
  wifiConnectStart = millis();
}

void stopWifi() {
  Serial.println("Switch OFF -> stopping Wi-Fi.");
  WiFi.disconnect(); // stop trying to connect / drop the current connection, but leave the driver initialized
  wifiState = WIFI_STATE_OFF;
}

void updateWifiState() {
  if (wifiState == WIFI_STATE_CONNECTING) {
    if (WiFi.status() == WL_CONNECTED) {
      wifiState = WIFI_STATE_CONNECTED;
      Serial.print("Wi-Fi connected! IP address: ");
      Serial.println(WiFi.localIP());
    } else if (millis() - wifiConnectStart > WIFI_TIMEOUT) {
      wifiState = WIFI_STATE_FAILED;
      Serial.println("Wi-Fi connection attempt timed out.");
    }
  } else if (wifiState == WIFI_STATE_CONNECTED) {
    // Detect an unexpected drop so the icon reflects reality
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi connection lost, retrying...");
      startWifiConnect();
    }
  }
  // Note: WIFI_STATE_FAILED just sits there until the switch is cycled off/on again,
  // per the requirement that a retry happens when the switch is turned back on.
}

// =====================
// Display: small Wi-Fi status icon, fixed bottom-right corner
// =====================
#define WIFI_ICON_CX 121       // icon "dot" x position - chosen so the r=6 arc's right edge lands on column 127
#define WIFI_ICON_CY 63        // icon "dot" y position - the very last row, so nothing sits below it
#define WIFI_ICON_CLEAR_X 106  // clears a box behind the icon so graph lines never clutter it
#define WIFI_ICON_CLEAR_Y 44
#define WIFI_ICON_CLEAR_W 22
#define WIFI_ICON_CLEAR_H 20

void clearWifiIconArea() {
  u8g2.setDrawColor(0); // 0 = erase, regardless of the display's normal draw color
  u8g2.drawBox(WIFI_ICON_CLEAR_X, WIFI_ICON_CLEAR_Y, WIFI_ICON_CLEAR_W, WIFI_ICON_CLEAR_H);
  u8g2.setDrawColor(1); // back to normal
}

void drawWifiIcon(int x, int y) {
  // x,y = the "dot" position at the base of the icon (bottom point of the arcs)
  switch (wifiState) {
    case WIFI_STATE_CONNECTED:
      u8g2.drawDisc(x, y, 1);
      u8g2.drawCircle(x, y, 3, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
      u8g2.drawCircle(x, y, 6, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
      break;
    case WIFI_STATE_CONNECTING: {
      // animate: number of arcs shown cycles 1 -> 2 -> 3 every 400ms
      int bars = 1 + ((millis() / 400) % 3);
      u8g2.drawDisc(x, y, 1);
      if (bars >= 2) u8g2.drawCircle(x, y, 3, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
      if (bars >= 3) u8g2.drawCircle(x, y, 6, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
      break;
    }
    case WIFI_STATE_FAILED:
      // Standard "no connection" convention: the full wifi symbol with a diagonal
      // slash through it, rather than a plain X. Same bounding box as CONNECTED
      // (radius 6, so it's not any bigger), just with a line drawn across it.
      u8g2.drawDisc(x, y, 1);
      u8g2.drawCircle(x, y, 3, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
      u8g2.drawCircle(x, y, 6, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
      u8g2.drawLine(x - 6, y - 6, x + 6, y); // corner-to-corner slash across the icon's own bounding box
      break;
    case WIFI_STATE_OFF:
    default:
      // just the dot, no arcs = radio off
      u8g2.drawDisc(x, y, 1);
      break;
  }
}

// =====================
// Display: graph of the last `sampleCount` values for the current mode
// =====================
void drawGraph(int graphTop, int graphBottom) {
  if (sampleCount < 2) {
    u8g2.setCursor(0, (graphTop + graphBottom) / 2);
    u8g2.print("Collecting data...");
    return;
  }

  float vals[NUM_RECORDED_VALUES];
  float minV = 1e9f, maxV = -1e9f;

  for (int i = 0; i < sampleCount; i++) {
    float v;
    switch (graphMode) {
      case GRAPH_TEMP: v = temperatureQueue[i]; break;
      case GRAPH_HUM:  v = humidityQueue[i];    break;
      case GRAPH_PPM:  v = adcToPpm(m135queue[i]); break;
      default:         v = 0;
    }
    vals[i] = v;
    if (v < minV) minV = v;
    if (v > maxV) maxV = v;
  }

  if (maxV - minV < 1.0f) { // avoid a divide-by-zero on a flat line
    maxV += 1.0f;
    minV -= 1.0f;
  }

  int graphHeight = graphBottom - graphTop;
  int prevX = -1, prevY = -1;

  // vals[0] is the newest sample; walk from oldest to newest so the graph reads left -> right
  for (int i = sampleCount - 1; i >= 0; i--) {
    int xPos = map(sampleCount - 1 - i, 0, sampleCount - 1, 0, 127);
    int yPos = graphBottom - (int)((vals[i] - minV) / (maxV - minV) * graphHeight);
    if (prevX >= 0) {
      u8g2.drawLine(prevX, prevY, xPos, yPos);
    }
    prevX = xPos;
    prevY = yPos;
  }
}

// =====================
// Display: main draw
// =====================
void showToDisplay() {
  u8g2.clearBuffer();

  if (!switchOn) {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 20);
    u8g2.print("Switch OFF");
  } else {
    // Current values, always shown
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(0, 8);
    char buf[40];
    snprintf(buf, sizeof(buf), "T:%.1fC  H:%.1f%% PPM:%.0f",
             currentTemp, currentHum, adcToPpm(currentAdc));
    u8g2.print(buf);

    // Graph label
    const char* label = (graphMode == GRAPH_TEMP) ? "Temperature (C)" :
                         (graphMode == GRAPH_PPM)  ? "PPM" :
                                                      "Humidity (%)";
    u8g2.setCursor(0, 18);
    u8g2.print(label);

    drawGraph(22, 63);

    if (wifiState == WIFI_STATE_CONNECTING) {
      // Bottom of the screen, per request - it's fine if this overlaps the graph,
      // and it's fine if it runs into the Wi-Fi icon area too: that area gets
      // cleared and redrawn on top right after this.
      u8g2.setFont(u8g2_font_5x7_tf);
      u8g2.setCursor(0, 62);
      u8g2.print("Connecting to Wi-Fi...");
    }
  }

  // Wi-Fi icon: always drawn last, fixed bottom-right corner, on a freshly
  // cleared patch so nothing else on screen can clutter it.
  clearWifiIconArea();
  drawWifiIcon(WIFI_ICON_CX, WIFI_ICON_CY);

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

  Wire.begin(LCD_SDA, LCD_SCL);
  u8g2.begin();

  // Wi-Fi mode is set exactly ONCE, here, and never changed again (no WIFI_OFF/STA
  // toggling at runtime). Repeatedly tearing down and recreating the WiFi driver via
  // WiFi.mode() is what caused the earlier boot-loop crash. Connecting/disconnecting
  // from here on is handled purely via WiFi.begin()/WiFi.disconnect() in loop().
  WiFi.mode(WIFI_STA);

  // HTTP endpoint
  server.on("/mq135", HTTP_GET, [](AsyncWebServerRequest *request){
    int latestMQ135 = m135queue[0];
    float ppm = adcToPpm(latestMQ135);

    String json = "{\"current_adc\": " + String(latestMQ135) +
                  ", \"current_ppm\": " + String(ppm) +
                  ", \"current_temperature\": " + String(temperatureQueue[0]) +
                  ", \"current_humidity\": " + String(humidityQueue[0]) +
                  ", \"wifi_state\": " + String((int)wifiState) +
                  ", \"queue\":[";

    for (int i = 0; i < sampleCount; i++) {
      json += "{\"adc\": " + String(m135queue[i]) +
              ", \"ppm\": " + String(adcToPpm(m135queue[i])) +
              ", \"temperature\": " + String(temperatureQueue[i]) +
              ", \"humidity\": " + String(humidityQueue[i]) + "}";

      if (i < sampleCount - 1) {
        json += ", ";
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
  unsigned long now = millis();

  // ---- Read switch (debounced) & detect edges ----
  bool rawSwitchOn = digitalRead(SWITCH_PIN) == LOW; // LOW = pressed/on, per INPUT_PULLUP wiring

  if (rawSwitchOn != rawSwitchOnLast) {
    lastSwitchChangeTime = now;      // raw signal just changed, restart the debounce timer
    rawSwitchOnLast = rawSwitchOn;
  }
  if (now - lastSwitchChangeTime >= DEBOUNCE_MS) {
    switchOn = rawSwitchOn;          // raw signal has been stable long enough, accept it
  }

  bool turnedOn  = switchOn && !lastSwitchOn;
  bool turnedOff = !switchOn && lastSwitchOn;
  lastSwitchOn = switchOn;

  if (turnedOn) {
    // Switch just went ON (including the very first loop iteration if it starts ON):
    // (re)start the Wi-Fi connection attempt.
    startWifiConnect();
    // Reset graph cycling so it starts fresh each time the device is turned on.
    graphMode = GRAPH_TEMP;
    lastGraphSwitch = now;
  }

  if (turnedOff) {
    stopWifi();
  }

  if (switchOn) {
    // ---- Wi-Fi state machine only runs while the switch is on ----
    updateWifiState();

    // ---- Sample sensors on their own schedule (non-blocking) ----
    if (now - lastSensorRead >= SENSOR_INTERVAL) {
      lastSensorRead = now;

      int latestAdc = analogRead(MQ135_AO_PIN);
      float latestTemp = dht.readTemperature();
      float latestHum  = dht.readHumidity();

      if (isnan(latestTemp) || isnan(latestHum)) {
        Serial.println("Failed to read from DHT11, carrying last known value forward.");
        latestTemp = currentTemp;
        latestHum  = currentHum;
      } else {
        haveValidReading = true;
      }

      currentAdc  = latestAdc;
      currentTemp = latestTemp;
      currentHum  = latestHum;

      // Push all three in lockstep so the queues never drift out of alignment.
      pushToIntQueue(latestAdc, m135queue);
      pushToFloatQueue(latestTemp, temperatureQueue);
      pushToFloatQueue(latestHum, humidityQueue);
      if (sampleCount < NUM_RECORDED_VALUES) sampleCount++;

      Serial.print("ADC: ");
      Serial.print(latestAdc);
      Serial.print(" | PPM: ");
      Serial.print(adcToPpm(latestAdc));
      Serial.print(" | Temp: ");
      Serial.print(latestTemp);
      Serial.print(" | Hum: ");
      Serial.println(latestHum);
    }

    // ---- Cycle the graph every 5s ----
    if (now - lastGraphSwitch >= GRAPH_INTERVAL) {
      lastGraphSwitch = now;
      graphMode = (GraphMode)((graphMode + 1) % 3);
    }
  }

  // ---- Redraw the display ----
  showToDisplay();

  delay(LOOP_TICK);
}
