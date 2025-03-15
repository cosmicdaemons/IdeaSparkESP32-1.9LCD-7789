#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "weather_images.h"

const char* ssid = "***";
const char* password = "***";
String latitude = "50";
String longitude = "30";
// Enter your location
String location = "Kyiv";
// Type the timezone you want to get the time for
String timezone = "Europe/Kyiv";

// Store date and time
String current_date;
String last_weather_update;
String temperature;
String humidity;
int is_day;
int weather_code = 0;
String weather_description;
#define TEMP_CELSIUS 1
#if TEMP_CELSIUS
  String temperature_unit = "";
  const char degree_symbol[] = "\u00B0";
#else
  String temperature_unit = "&temperature_unit=fahrenheit";
  const char degree_symbol[] = "\u00B0F";
#endif


#define LCD_MOSI 23
#define LCD_SCLK 18
#define LCD_CS 15
#define LCD_DC 2
#define LCD_RST 4
#define LCD_BLK 32
Adafruit_ST7789 lcd=Adafruit_ST7789(LCD_CS,LCD_DC,LCD_RST);
uint16_t textColor;

#define maxString 21
char target[maxString + 1] = "";
String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}

void get_weather_data() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Construct the API endpoint
    String url = String("http://api.open-meteo.com/v1/forecast?latitude=" + latitude + "&longitude=" + longitude + "&current=temperature_2m,relative_humidity_2m,is_day,precipitation,rain,weather_code" + temperature_unit + "&timezone=" + timezone + "&forecast_days=1");
    http.begin(url);
    int httpCode = http.GET(); // Make the GET request

    if (httpCode > 0) {
      // Check for the response
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        //Serial.println("Request information:");
        //Serial.println(payload);
        // Parse the JSON to extract the time
        DynamicJsonDocument doc(800);
        //JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          const char* datetime = doc["current"]["time"];
          String datetime_str = String(datetime);
          int splitIndex = datetime_str.indexOf('T');
          current_date = datetime_str.substring(0, splitIndex);
          last_weather_update = datetime_str.substring(splitIndex + 1, splitIndex + 9);
          String temperature = doc["current"]["temperature_2m"];
          String humidity = doc["current"]["relative_humidity_2m"];
          String is_day = doc["current"]["is_day"];
          String weather_code = doc["current"]["weather_code"];
          if (is_day == "1"){lcd.fillScreen(ST77XX_WHITE);lcd.setTextColor(ST77XX_BLACK);}else{lcd.fillScreen(ST77XX_BLACK);lcd.setTextColor(ST77XX_WHITE);}
          get_weather_description(weather_code.toInt());
          lcd.setTextSize(2);
          lcd.setCursor(10, 140);
          lcd.println(utf8rus(weather_description));
          lcd.setTextSize(2);
          lcd.setCursor(170, 20);
          lcd.println(String(current_date));
          lcd.setTextSize(2);
          lcd.setCursor(200, 60);
          lcd.println(String(temperature + "\xB0" + "C"));
          lcd.setTextSize(2);
          lcd.setCursor(210, 100);
          lcd.println(String(humidity+"%"));
          lcd.setTextSize(1);
          lcd.setCursor(10, 160);
          lcd.println(String(utf8rus("Оновлено в ")+last_weather_update));
          if (is_day == "1"){
            if (weather_code.toInt() == 0 || weather_code.toInt() == 1 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_clearsky_day, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() == 2 || weather_code.toInt() == 3 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_partlycloudy_day, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() == 45 || weather_code.toInt() == 48 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_fog_day, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() > 50 && weather_code.toInt() < 60 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_partlycloudy_day, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() > 60 && weather_code.toInt() < 68 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_rainshowers_day, 100, 100, ST77XX_ORANGE);
            }

             if (weather_code.toInt() > 70 && weather_code.toInt() < 78 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_snowshowers_day, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() > 79 && weather_code.toInt() < 87 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_rainshowers_day, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() > 95 && weather_code.toInt() < 100 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_rainshowersandthunder_day, 100, 100, ST77XX_ORANGE);
            }
          }
          if (is_day == "0"){
            if (weather_code.toInt() == 0 || weather_code.toInt() == 1 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_clearsky_night, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() == 2 || weather_code.toInt() == 3 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_partlycloudy_night, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() == 45 || weather_code.toInt() == 48 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_fog_night, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() > 50 && weather_code.toInt() < 60 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_partlycloudy_night, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() > 60 && weather_code.toInt() < 68 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_rainshowers_night, 100, 100, ST77XX_ORANGE);
            }

             if (weather_code.toInt() > 70 && weather_code.toInt() < 78 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_snowshowers_night, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() > 79 && weather_code.toInt() < 87 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_rainshowers_night, 100, 100, ST77XX_ORANGE);
            }
             if (weather_code.toInt() > 95 && weather_code.toInt() < 100 ){
             lcd.drawBitmap(30, 20, met_bitmap_white_100x100_rainshowersandthunder_night, 100, 100, ST77XX_ORANGE);
            }
          }
          Serial.println(temperature);
          Serial.println(humidity);
          Serial.println(is_day);
          Serial.println(weather_code);
          Serial.println(String(timezone));
          // Split the datetime into date and time
//          String datetime_str = String(datetime);
//          int splitIndex = datetime_str.indexOf('T');
//          current_date = datetime_str.substring(0, splitIndex);
          last_weather_update = datetime_str.substring(splitIndex + 1, splitIndex + 9); // Extract time portion
        } else {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
        }
      }
      else {
        Serial.println("Failed");
      }
    } else {
      Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end(); // Close connection
  } else {
    Serial.println("Not connected to Wi-Fi");
  }
}

void setup() {
  lcd.cp437(true);
  lcd.init(170, 320);
  
  lcd.setRotation(3);
  Serial.begin(115200);

//  textColor = ST77XX_WHITE;
//  lcd.setTextColor(textColor);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to Wi-Fi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void get_weather_description(int code) {
  switch (code) {
    case 0:
      weather_description = "ЯСНО";
      break;
    case 1: 
      weather_description = "ЯСНО";
      break;
    case 2: 
      weather_description = "МIНЛИВА ХМАРНIСТЬ";
      break;
    case 3:
      weather_description = "ХМАРНО";
      break;
    case 45:
      weather_description = "ТУМАН";
      break;
    case 48:
      weather_description = "ТУМАН";
      break;
    case 51:
      weather_description = "ХМАРНО МОЖЛИВИЙ ДОЩ";
      break;
    case 53:
      weather_description = "ХМАРНО МОЖЛИВИЙ ДОЩ";
      break;
    case 55:
      weather_description = "ХМАРНО МОЖЛИВИЙ ДОЩ";
      break;
    case 56:
      weather_description = "ХМАРНО МОЖЛИВИЙ ДОЩ";
      break;
    case 57:
      weather_description = "ХМАРНО МОЖЛИВИЙ ДОЩ";
      break;
    case 61:
      weather_description = "ДОЩ";
      break;
    case 63:
      weather_description = "ДОЩ";
      break;
    case 65:
      weather_description = "ДОЩ";
      break;
    case 66:
      weather_description = "ДОЩ";
      break;
    case 67:
      weather_description = "ДОЩ";
      break;
    case 71:
      weather_description = "МОЖЛИВИЙ СНIГ";
      break;
    case 73:
      weather_description = "МОЖЛИВИЙ СНIГ";
      break;
    case 75:
      weather_description = "МОЖЛИВИЙ СНIГ";
      break;
    case 77:
      weather_description = "СНIГ";
      break;
    case 80:
      weather_description = "ДОЩ";
      break;
    case 81:
      weather_description = "ДОЩ";
      break;
    case 82:
      weather_description = "ДОЩ";
      break;
    case 85:
      weather_description = "ДОЩ";
      break;
    case 86:
      weather_description = "СИЛЬНИЙ ДОЩ";
      break;
    case 95:
      weather_description = "ГРОЗА";
      break;
    case 96:
      weather_description = "ГРОЗА";
      break;
    case 99:
      weather_description = "ГРОЗА";
      break;
    default: 
      weather_description = "НЕВIДОМО";
      break;
  }
}

void loop() {
  get_weather_data();
  delay(600000);
}
