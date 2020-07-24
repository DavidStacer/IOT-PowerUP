#include <Wire.h>

#include "SHTSensor.h"

// https://randomnerdtutorials.com/esp32-timer-wake-up-deep-sleep/ great resource - very good tutorials
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */


SHTSensor sht;
// To use a specific sensor instead of probing the bus use this command:
// SHTSensor sht(SHTSensor::SHT3X);

RTC_DATA_ATTR int bootCount = 0;   // the RTC_DATA_ATTR stored the variable in the Real Time Clock so it is saved when main processor is sleeping.


void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  Wire.setClock(50000);

  Serial.begin(115200);
  //delay(1000); // let serial console settle

  if (sht.init()) {
    Serial.print("init(): success\n");
  } else {
    Serial.print("init(): failed\n");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
  sht.readSample();    // seems like the first reading ends in an error so take a reading now to clear error.
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Boot number:" + String(bootCount++));
}

void loop() {

  //    unsigned long start;
  //    start = millis();
  //  sht.readSample();
  //  Serial.print("Time to take temp ");
  //  Serial.println(millis() - start);

  if (sht.readSample()) {
    Serial.print("RH: ");
    Serial.print(sht.getHumidity(), 2);
    Serial.print(" C: ");
    Serial.print(sht.getTemperature(), 2);
    Serial.println("     Going to sleep now for " + String(TIME_TO_SLEEP) + " Seconds");
    //delay(100);
    Serial.flush();
    esp_deep_sleep_start();
  } else {
    Serial.print("Error in readSample()\n");
  }

}
