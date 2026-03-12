#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// ─────────────────────────────────────────────
// Wi-Fi Credentials — UPDATE THESE
// ─────────────────────────────────────────────
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ─────────────────────────────────────────────
// LED GPIO Pin Mapping
// ─────────────────────────────────────────────
#define LEFT_RED    12
#define LEFT_GREEN  13
#define RIGHT_RED   14
#define RIGHT_GREEN 15

AsyncWebServer server(80);

// ─────────────────────────────────────────────
// Helper: set all LEDs to GREEN (default/safe state)
// ─────────────────────────────────────────────
void setDefaultGreen() {
  digitalWrite(LEFT_RED,    LOW);
  digitalWrite(LEFT_GREEN,  HIGH);
  digitalWrite(RIGHT_RED,   LOW);
  digitalWrite(RIGHT_GREEN, HIGH);
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  // Configure LED pins
  pinMode(LEFT_RED,    OUTPUT);
  pinMode(LEFT_GREEN,  OUTPUT);
  pinMode(RIGHT_RED,   OUTPUT);
  pinMode(RIGHT_GREEN, OUTPUT);

  // Default state: both sides GREEN
  setDefaultGreen();

  // ── Camera Configuration ──────────────────
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size   = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count     = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

  // ── Wi-Fi Connection ──────────────────────
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32-CAM IP: http://");
  Serial.println(WiFi.localIP());

  // ── HTTP Routes ───────────────────────────

  // Serve JPEG frame
  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      request->send(500, "text/plain", "Camera capture failed");
      return;
    }
    request->send_P(200, "image/jpeg", fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });

  // Vehicle detected on LEFT → Left=GREEN, Right=RED
  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LEFT_RED,    LOW);
    digitalWrite(LEFT_GREEN,  HIGH);
    digitalWrite(RIGHT_RED,   HIGH);
    digitalWrite(RIGHT_GREEN, LOW);
    Serial.println("LEFT detected → Left:GREEN  Right:RED");
    request->send(200, "text/plain", "Left detected");
  });

  // Vehicle detected on RIGHT → Right=GREEN, Left=RED
  server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LEFT_RED,    HIGH);
    digitalWrite(LEFT_GREEN,  LOW);
    digitalWrite(RIGHT_RED,   LOW);
    digitalWrite(RIGHT_GREEN, HIGH);
    Serial.println("RIGHT detected → Left:RED  Right:GREEN");
    request->send(200, "text/plain", "Right detected");
  });

  // Vehicles on BOTH sides → Both=RED
  server.on("/both", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LEFT_RED,    HIGH);
    digitalWrite(LEFT_GREEN,  LOW);
    digitalWrite(RIGHT_RED,   HIGH);
    digitalWrite(RIGHT_GREEN, LOW);
    Serial.println("BOTH detected → Left:RED  Right:RED");
    request->send(200, "text/plain", "Both detected");
  });

  // No vehicle → Both=GREEN
  server.on("/none", HTTP_GET, [](AsyncWebServerRequest *request) {
    setDefaultGreen();
    Serial.println("NO vehicle → Left:GREEN  Right:GREEN");
    request->send(200, "text/plain", "No detection");
  });

  server.begin();
  Serial.println("Server started!");
}

void loop() {
  // All logic handled via HTTP callbacks — nothing needed here
}
