// mqtt-atoms3lite-neomatrix.ino

#include <M5Unified.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "config.h"
#include "time.h"

WiFiClient wifi_client;
void mqtt_sub_callback(char* topic, byte* payload, unsigned int length);
PubSubClient mqtt_client(mqtt_host2, mqtt_port, mqtt_sub_callback, wifi_client);

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define BTNLED_PIN 35 // PIN for Button LED
#define MATRIX_PIN 2  // AtomS3Lite's Grove connector.

// initial time (possibly given by an external RTC)
#define RTC_UTC_TEST 1735092915  // 1735092915 = 2024-12-25 02:15:15 UTC

// for testing purpose:
extern "C" int clock_gettime(clockid_t unused, struct timespec* tp);

////////////////////////////////////////////////////////
static timeval tv;
static timespec tp;
static time_t now;

int BLED_R = 0;
int BLED_G = 0;
int BLED_B = 0;
int MLED_R = 0;
int MLED_G = 0;
int MLED_B = 0;

String the_message;
String the_color;
int  pixelPerChar = 6;
int  maxDisplacement;

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (RGB+W NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.

Adafruit_NeoPixel  button_led(1, BTNLED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 8, 
  MATRIX_PIN,
  NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void matrix_setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);
//  the_color = "#777777";
//  set_message_color(the_color);
}

void BLED_set(byte idx, byte red, byte green, byte blue) {
  BLED_R = red;
  BLED_G = green;
  BLED_B = blue;
  Serial.print(" R="); Serial.print(BLED_R);
  Serial.print(" G="); Serial.print(BLED_G);
  Serial.print(" B="); Serial.print(BLED_B);
  Serial.println();
  button_led.setPixelColor(idx, button_led.Color(BLED_R, BLED_G, BLED_B));
  button_led.show();
}

#define PTM(w) \
  Serial.print(" " #w "="); \
  Serial.print(tm->tm_##w);

void printTm(const char* what, const tm* tm) {
  Serial.print(what);
  PTM(isdst);
  PTM(yday);
  PTM(wday);
  PTM(year);
  PTM(mon);
  PTM(mday);
  PTM(hour);
  PTM(min);
  PTM(sec);
}

void showTime_old() {
  gettimeofday(&tv, nullptr);
  clock_gettime(0, &tp);
  now = time(nullptr);
  Serial.println();
  printTm("localtime:", localtime(&now));
  Serial.println();
  printTm("gmtime:   ", gmtime(&now));
  Serial.println();
}

void print_GMT_Time() {
  time_t timeNow;
  struct tm * timeElements;
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    timeNow = time(NULL);
    timeElements = gmtime(&timeNow);
    //Serial.println(isotime(timeElements));
    Serial.print("GMT: ");
    Serial.println(timeElements, "%Y-%m-%d %H:%M:%S");  // Serial port output date and time.  
  }
}

void print_LOC_Time() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.print("LOC: ");
    Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");  // Serial port output date and time.  
  }
}

void showTime() {
  gettimeofday(&tv, nullptr);
  clock_gettime(0, &tp);
  now = time(nullptr);
  Serial.println();
  print_GMT_Time();
  print_LOC_Time();
  Serial.println();
}

void every_minute() {
  unsigned int OLD_BLED_R, OLD_BLED_G, OLD_BLED_B;
  now = time(nullptr);
  if (localtime(&now) -> tm_sec == 0) {
    OLD_BLED_R = BLED_R;
    OLD_BLED_G = BLED_G;
    OLD_BLED_B = BLED_B;
    BLED_set(0, 10, 0, 0);
    showTime();
    delay(1000);
    BLED_set(0, OLD_BLED_R, OLD_BLED_G, OLD_BLED_B);
  }
}

void setup() {
  auto cfg = M5.config();
  cfg.serial_baudrate = 19200;
  M5.begin(cfg);

  // Wifi
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  //WiFi.begin(wifi_ssid, wifi_password, 0, wifi_bssid);
  // WiFi.BSSIDstr(i).c_str()
  WiFi.setSleep(false);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    switch (count % 4) {
      case 0:
        Serial.print("|");
        BLED_set(0, 10, 10, 0);
        break;
      case 1:
        Serial.print("/");
        break;
      case 2:
        BLED_set(0, 0, 0, 0);
        Serial.print("-");
        break;
      case 3:
        Serial.println("\\");
        break;
    }
    count++;
    if (count >= 240) reboot();  //was delay 250 240 / 4 = 60sec
  }
  BLED_set(0, 10, 10, 0);
  Serial.println("WiFi connected!");
  delay(1000);

  // MQTT
  bool rv = false;
  if (mqtt_use_auth == true) {
    rv = mqtt_client.connect(mqtt_client_id, mqtt_username, mqtt_password);
  } else {
    rv = mqtt_client.connect(mqtt_client_id);
  }
  if (rv == false) {
    Serial.println("mqtt connecting failed...");
    reboot();
  }
  Serial.println("MQTT connected!");

  BLED_set(0, 0, 0, 10);

  Serial.print("Subscribe : topic=");
  Serial.println(mqtt_subscribe_topic);
  mqtt_client.subscribe(mqtt_subscribe_topic);

  delay(1000);

  // setup RTC time
  // it will be used until NTP server will send us real current time
  Serial.println("Manually setting some time from some RTC:");
  time_t rtc = RTC_UTC_TEST;
  timeval tv = { rtc, 0 };
  settimeofday(&tv, nullptr);

  // configTime
//  configTime(config_time_hours * 3600, 0, config_time_server1);
//  configTime(MYTZ, config_time_server1);
  configTime(gmtOffset_sec, daylightOffset_sec, config_time_server1);
  delay(1000);
  struct tm t;
  count = 0;
  if (!getLocalTime(&t)) {
    Serial.println("getLocalTime() failed...");
    delay(1000);
//    count++;
//    if (count > 5) break;
    //reboot();
  }
  showTime();
  delay(1000);
  showTime();
  Serial.println("configTime() success!");

  BLED_set(0, 0, 10, 0);

  delay(1000);

  the_message = "Hello!";
  maxDisplacement = the_message.length() * pixelPerChar + matrix.width();

  matrix_setup();
}

void reboot() {
  Serial.println("REBOOT!!!!!");
  for (int i = 0; i < 30; ++i) {
    button_led.setPixelColor(0, button_led.Color(255, 0, 255));
    delay(100);
    button_led.setPixelColor(0, button_led.Color(0, 0, 0));
    delay(100);
  }

  ESP.restart();
}

int x    = matrix.width();
int pass = 0;

void matrix_loop() {
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(the_message);
  if (--x < -maxDisplacement) {
    x = matrix.width();
  }
//  if(--x < -64) {  
//    x = matrix.width();
//    matrix.setTextColor(matrix.Color(MLED_R, MLED_G, MLED_B));
//  }
  matrix.show();
  delay(100);
}

void matrix_loop_old() {
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(the_message);
  if(--x < -36) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(100);
}

void loop() {
  mqtt_client.loop();
  if (!mqtt_client.connected()) {
    Serial.println("MQTT disconnected...");
//    reboot();
  }
 // showTime();
  matrix_loop();
  M5.update();
}

#define MAX_BUF_LEN 256
void mqtt_sub_callback(char* topic, byte* payload, unsigned int length) {
  char buf[MAX_BUF_LEN];
  String payload_str;

  memset(buf, 0, MAX_BUF_LEN);
  int min_len;
  min_len = MAX_BUF_LEN;
  if (length < min_len) {
    min_len = length;
  }
  strncpy(buf, (char*)payload, min_len);
  payload_str = String(buf);

  int k;
  k = payload_str.indexOf(',');
  the_message = payload_str.substring(0, k);
  the_color   = payload_str.substring(k+1, min_len);
  set_message_color(the_color);
  
  maxDisplacement = the_message.length() * pixelPerChar + matrix.width();
  
  showTime();
  Serial.print("str     :"); Serial.println(payload_str);
  Serial.print("length  :"); Serial.println(length);
  Serial.print("min_len :"); Serial.println(min_len);
  Serial.print("k       :"); Serial.println(k);
  Serial.print("msg     :"); Serial.println(the_message);
  Serial.print("clr     :"); Serial.println(the_color);

  matrix.show();
}


void set_message_color(String &color_str) {
  if (color_str.length() != 7) return;

  // check header
  if (color_str.substring(0, 1) != "#") return;

  String str_r = color_str.substring(1, 3);
  String str_g = color_str.substring(3, 5);
  String str_b = color_str.substring(5, 7);

  char *pos;
  uint8_t r = (uint8_t)strtol(str_r.c_str(), &pos, 16);
  uint8_t g = (uint8_t)strtol(str_g.c_str(), &pos, 16);
  uint8_t b = (uint8_t)strtol(str_b.c_str(), &pos, 16);

  MLED_R = r;
  MLED_G = g;
  MLED_B = b;

  matrix.setTextColor(matrix.Color(MLED_R, MLED_G, MLED_B));
}
