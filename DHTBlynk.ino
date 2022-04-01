// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLI3qKFgy8"
#define BLYNK_DEVICE_NAME "Automatic Incubator"
#define BLYNK_AUTH_TOKEN "qCpnWRLDvEaPZ0EMw7Y-ZCqrGOg68C2m"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "hazwan";
char pass[] = "maiseringgit";

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  dht.begin();
  lcd.begin();
  lcd.backlight();
  //timer.setInterval(1000L, sendSensor);
}

void loop() {

  Blynk.run();
  timer.run();
  timer.setInterval(1000L, sendSensor);

}


////////////////// DHT SENSOR ///////////////////////////////
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V8, h);
  Blynk.virtualWrite(V7, t);
  
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(t);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("RH:");
  lcd.print(h);
  lcd.print("%");

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
}
