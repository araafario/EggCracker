#include <LiquidCrystal_I2C.h>
#include "DHT.h"

//========== PINOUT =============//
/* RELAY1 - 13
 * RELAY2 - 12
 * PWM - 27
 * DIR - 26
 * DHT - 14
 * SCL - 22
 * SDA - 21 
 */

#define RELAYPIN_1 13
#define RELAYPIN_2 12
#define PWMPIN 27
#define DIRPIN 26

#define DHTPIN 14     // Digital pin connected to the DHT sensor
// Pin that can be used =  3, 4, 5, 12, 13 or 14
// Pin 15 can work but DHT must be disconnected during program upload.
#define DHTTYPE DHT11   // DHT 11
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht(DHTPIN, DHTTYPE);

int lcdColumns = 16;
int lcdRows = 2;
char Xbuffer[20];
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  //PIN 21 - SDA, PIN 22 - SCL

#define MAXTEMP 40 //Max Temperature for the Incubator
#define MINTEMP 30 //Min Temerature for the Incubator
#define MOTORSPEED 50

//#define DEBUG //Uncomment for Printing Variable

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAYPIN_1,OUTPUT);
  pinMode(RELAYPIN_2,OUTPUT);
  pinMode(PWMPIN,OUTPUT);
  pinMode(DIRPIN,OUTPUT);
  

  lcd.init();                      
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("PREPARING ...");
  delay(5000);
  lcd.clear();
  
  dht.begin();
  analogWrite(PWMPIN, MOTORSPEED);
}

void loop() {
 
  float h = dht.readHumidity();
  float t = dht.readTemperature(); //Celcius
  float f = dht.readTemperature(true); //Farenheit
  delay(1000);
  
  //==== WARNING EXPERIMENTAL CODE !! ====//
  // Used for re-read the sensor if it's reading failed 
  //
  //  if (isnan(h) || isnan(t) || isnan(f)) {
  //    Serial.println(F("Failed to read Sensor !!"));
  //    return;
  //  }
  //  float hif = dht.computeHeatIndex(f, h); // Compute heat index in Fahrenheit (the default)
  //  float hic = dht.computeHeatIndex(t, h, false); // Compute heat index in Celsius (isFahreheit = false)
  //
  //======================================//
  
  #ifdef DEBUG
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
  #endif

  //ON - OFF Control for the temperature
  if(t > MAXTEMP) {
    digitalWrite(RELAYPIN_1,LOW);
    digitalWrite(RELAYPIN_2,LOW);
  }
  else if (t < MINTEMP){
    digitalWrite(RELAYPIN_1,HIGH);
    digitalWrite(RELAYPIN_2,HIGH);    
  }

  lcd.clear();
  
  lcd.setCursor(0, 0);
  sprintf(Xbuffer,"HUMIDITY = %.2f",h);
  lcd.print(Xbuffer);
  
  lcd.setCursor(0, 1);
  sprintf(Xbuffer,"TEMP = %.2f C",t);
  lcd.print(Xbuffer);

  
}
