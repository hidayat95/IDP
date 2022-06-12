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
//char ssid[] = "hazwan";
//char pass[] = "maiseringgit";
char ssid[] = "Redmi Note 9S";
char pass[] = "12345678";

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
int h;
float t;

// incubator param //
double App_Temp_Set;
int App_Humid_Set;
int App_Days_Set;
int pinValue;

float inc_temp;
int inc_humid;
int inc_days;

float new_temp;
int new_humid;
int new_days;


// pin define //
#define heater 13
#define fan 12
#define w_pump 14
#define roller 5

int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
long interval;           // interval at which to blink (milliseconds)

const int Chick_Btn = 27;
const int Quail_Btn = 26;
const int Duck_Btn = 25;
const int Goose_Btn = 33;
int Chick_BtnState = 0;
int Quail_BtnState = 0;
int Duck_BtnState = 0;
int Goose_BtnState = 0;



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

  pinMode(heater, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(w_pump, OUTPUT);
  pinMode(roller, OUTPUT);

  pinMode(Chick_Btn, INPUT);
  pinMode(Quail_Btn, INPUT);
  pinMode(Duck_Btn, INPUT);
  pinMode(Goose_Btn, INPUT);
  
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
  LCD_DISP();
  Inc_Button();
  param_setting();
  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, sendUptime); 
 // temp_ctrl();
  //egg_roll();
  inc_seq();
  

  Serial.print(new_temp);
  Serial.print("  ");
  Serial.print(new_humid);
  Serial.print("  ");
  Serial.println(new_days);

}


// ================================================================
// ===                      DHT SENSOR                          ===
// ================================================================
void sendSensor()
{
  h = dht.readHumidity();
  t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V8, h);
  Blynk.virtualWrite(V7, t);
  
//  Serial.print(F("Humidity: "));
//  Serial.print(h);
//  Serial.print(F("%  Temperature: "));
//  Serial.print(t);
//  Serial.println(F("°C "));
}


// ================================================================
// ===                      TEMP CONTROL                       ===
// ================================================================
void temp_ctrl(){
  
  if(t < new_temp){ //37
    digitalWrite(heater, HIGH); //heater on
    digitalWrite(fan, HIGH); //fan on
   }

  else if(t >= new_temp+1){ //38.5
    digitalWrite(heater, LOW); //heater off
    digitalWrite(fan, LOW); //fan off
   }


   // humidity
   if (h < new_humid+5){
    digitalWrite(fan, LOW); //fan off
    digitalWrite(w_pump, HIGH); //water pump on
    delay(5000);
    digitalWrite(w_pump, LOW); //water pump off
   }

   else if (h > new_humid+5){
     digitalWrite(fan, HIGH); //fan on
     digitalWrite(w_pump, LOW); //water pump off
   }
}

void temp_ctrl_off(){
  digitalWrite(heater, LOW); //heater off
  digitalWrite(fan, LOW); //fan off
  digitalWrite(w_pump, LOW);
}


// ================================================================
// ===                      EGG ROLLING                         ===
// ================================================================
void egg_roll(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
      interval = 3000;  // roller on berapa lama
    } else {
      ledState = LOW;
      interval = 5000;//(86400000/5);    // roller interval time, ikut sehari bahagi berapa kali nak pusing. 
    }
    digitalWrite(roller, ledState);
  }
}



// ================================================================
// ===                      INCUBATOR SEQUENCE                  ===
// ================================================================
void inc_seq(){

  if((millis() / 60000) == new_days){  //86400000
    temp_ctrl_off();
    Blynk.notify("Finished!");
  }

  else if((millis() / 60000) < new_days){
    temp_ctrl();
  }



///// egg rolling mech //////
  if((millis() / 60000) == (new_days-3)){  //86400000
    digitalWrite(roller, LOW);  // turn off egg rolling
  }

  else if((millis() / 60000) < (new_days-3)){
    egg_roll();
  }
}




// ================================================================
// ===                      BUTTON                              ===
// ================================================================
void Inc_Button(){

  Chick_BtnState = digitalRead(Chick_Btn);
  Quail_BtnState = digitalRead(Quail_Btn);
  Duck_BtnState = digitalRead(Duck_Btn);
  Goose_BtnState = digitalRead(Goose_Btn);
  
  if (Chick_BtnState == HIGH){
    //Serial.println("ada");
    inc_days = 21;
    inc_temp = 29.8;  //38
    inc_humid = 75;
    while(Chick_BtnState == HIGH){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CHICKEN EGG MODE");
      delay(3000);
      lcd.clear();
      break;
    }
    Serial.print(inc_temp);
    Serial.print("  ");
    Serial.println(inc_humid);
  }

  if (Quail_BtnState == HIGH){
    inc_days = 18;
    inc_temp = 29.8;  //36.5
    inc_humid = 70;
    while(Quail_BtnState == HIGH){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("QUAIL EGG MODE");
      delay(3000);
      lcd.clear();
      break;
    }
    Serial.print(inc_temp);
    Serial.print("  ");
    Serial.println(inc_humid);
  }

  if (Duck_BtnState == HIGH){
    inc_days = 28;
    inc_temp = 31.8;  //37.0
    inc_humid = 50;
    while(Duck_BtnState == HIGH){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("DUCK EGG MODE");
      delay(3000);
      lcd.clear();
      break;
    }
    Serial.print(inc_temp);
    Serial.print("  ");
    Serial.println(inc_humid);
  }

  if (Goose_BtnState == HIGH){
    inc_days = 32;
    inc_temp = 31.8;  //37.0
    inc_humid = 55;
    while(Goose_BtnState == HIGH){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("GOOSE EGG MODE");
      delay(3000);
      lcd.clear();
      break;
    }
    Serial.print(inc_temp);
    Serial.print("  ");
    Serial.println(inc_humid);
  }

//  else if(Chick_BtnState == LOW){
//    Serial.println("x dak");
//  }
  
}


// ================================================================
// ===                   PARAMETER SETTINGS                     ===
// ================================================================
void param_setting(){
    if (pinValue == 1){
    new_temp = App_Temp_Set;
    new_humid = App_Humid_Set;
    new_days = App_Days_Set;
  }

  else if (pinValue == 0){
    new_temp = inc_temp;
    new_humid = inc_humid;
    new_days = inc_days;
  }
}

BLYNK_WRITE(V4)
{
  pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("V4 Slider value is: ");
  Serial.println(pinValue);
}




// ================================================================
// ===                      LCD DISP                            ===
// ================================================================
void LCD_DISP(){

  lcd.setCursor(0,0);
  lcd.print("DAY:");
  lcd.print(1+(millis() / 86400000)); //86400000
  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(t);
  lcd.print("C");
  lcd.print("  RH:");
  lcd.print(h);
  lcd.print("%");
  
}


// ================================================================
// ===                DISP RUNNING DAYS KT APPS                 ===
// ================================================================
void sendUptime()
{
  Blynk.virtualWrite(V9, millis() / 86400000); //1 day = 8.64x10^7 millis , 86400000
}


// ================================================================
// ===                      APPS MANUAL SET                    ===
// ================================================================
BLYNK_WRITE(V0) // Temp Set
{
  App_Temp_Set = param.asDouble(); // assigning incoming value from pin V1 to a variable
  Serial.print("Temp Set value is: ");
  Serial.println(App_Temp_Set);
}

BLYNK_WRITE(V1) // humidity set
{
  App_Humid_Set = param.asInt();
  Serial.print("humid Set value is: ");
  Serial.println(App_Humid_Set);
}

BLYNK_WRITE(V2)  // days set
{
  App_Days_Set = param.asInt();
  Serial.print("Days Set value is: ");
  Serial.println(App_Days_Set);
}
