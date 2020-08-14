/*
  WriteMultipleFields

  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.

  Hardware: ESP32 based boards

  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!

  Note:
  - Requires installation of EPS32 core. See https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md for details.
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.

  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.

  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.

  For licensing information, see the accompanying license file.

  Copyright 2018, The MathWorks, Inc.
*/

#include "ThingSpeak.h"
#include "secrets.h"
#include <WiFi.h>
#include <Wire.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "SHTSensor.h"          // lib from https://github.com/Sensirion/arduino-sht
// https://randomnerdtutorials.com/esp32-timer-wake-up-deep-sleep/ great resource - very good tutorials
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60   //300       // <<< change this one for the interval you want in seconds

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

SHTSensor sht(SHTSensor::SHT3X_ALT);

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int sensorErrorCount = 0;

bool sensorError = false;

float humidity, celsius, fahrenheit;      // global variables - its only a demo

void setup() {
  Serial.begin(115200);  //Initialize serial
  Serial.println("in setup");
  // brown out is when an internal voltage sensor detects too low of voltage.
  // I was having problems with brown out then found that if you slow down the processor speed that can cure the problem.
  // I'm not sure if my board was unique of if other will have this problem too.
  // to change the processor speed tools->frequency and set to 80Mhz
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  uint32_t brown_reg_temp = READ_PERI_REG(RTC_CNTL_BROWN_OUT_REG); //save WatchDog register
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 // WiFi.mode(WIFI_MODE_STA); // turn on WiFi
  WiFi.mode(WIFI_STA);      // turn on WiFi
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  Wire.begin();
  Wire.setClock(50000);    // helps with some of the slower temp sensors
  if (sht.init()) {
    Serial.print("init(): success\n");
  } else {
    Serial.print("init(): failed\n");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
//  delay(500);
//  Serial.println("Enabled Brown out Detection");
//  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, brown_reg_temp); //enable brownout detector
  bootCount++;
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Boot number:" + String(bootCount) + " Sensor Errors:" + sensorErrorCount);
}

bool read_temp_sensor() {
  if (sht.readSample()) {
    celsius = sht.getTemperature();
    fahrenheit = (sht.getTemperature() * 9 / 5) + 32;
    humidity = sht.getHumidity();

    Serial.print("RH: ");
    Serial.print(humidity);
    Serial.print(" C: ");
    Serial.print(celsius);
    Serial.print(" F: ");
    Serial.println(fahrenheit);

    delay(100);
    Serial.flush();
    return true;

  } else {
    Serial.print("Error in readSample()\n");
    sensorErrorCount++;
    return false;
  }
}

void loop() {


  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(1000);
    }
    Serial.println("\nConnected.");
  }


  if ( ! read_temp_sensor()) {
    Serial.println("error reading sensor");
    read_temp_sensor();
  }

  // set the fields with the values
  ThingSpeak.setField(1, fahrenheit);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, celsius);
  ThingSpeak.setField(4, 0);
  ThingSpeak.setField(5, 0);
  ThingSpeak.setField(6, 0);
  ThingSpeak.setField(7, 0);
  ThingSpeak.setField(8, bootCount++);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {

    Serial.println("   Channel update successful.");
    WiFi.disconnect();
    Serial.println("   Going to sleep now for " + String(TIME_TO_SLEEP) + " Seconds");

    delay(500);
    esp_deep_sleep_start();
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
    delay(500);
    esp_deep_sleep_start();
  }


}
