#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include  "WiFi.h" 
WiFiClient client;
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

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

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

#define MAXTEMP 39 //Max Temperature for the Incubator
#define MINTEMP 37 //Min Temerature for the Incubator
#define MOTORSPEED 50

//#define DEBUG //Uncomment for Printing Variable

String thingSpeakAddress = "api.thingspeak.com";
String writeAPIKey = "VYIHER65R7QOQZQU";
String tsfield1Name;
String request_string;

void setup() {
  Serial.begin(115200);
  
  WiFi.disconnect();
  WiFi.begin("HUAWEI-dE3F", "wifirumah");
  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  pinMode(RELAYPIN_1,OUTPUT);
  pinMode(RELAYPIN_2,OUTPUT);
  pinMode(PWMPIN,OUTPUT);
  pinMode(DIRPIN,OUTPUT);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(PWMPIN, ledChannel);

  lcd.init();                      
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("PREPARING ...");
  delay(5000);
  lcd.clear();
  
  dht.begin();
  ledcWrite(PWMPIN, MOTORSPEED);

}

void loop() {
 
  //float h = dht.readHumidity();
  //float t = dht.readTemperature(); //Celcius
  //float f = dht.readTemperature(true); //Farenheit
  float h = 0,t = 0,f = 0;
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
    Serial.println(f);
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
  kirim_thingspeak(t,h);
}

void kirim_thingspeak(float suhu, float hum) {
  if (client.connect("api.thingspeak.com", 80)) {
    request_string = "/update?";
    request_string += "key=";
    request_string += writeAPIKey;
    request_string += "&";
    request_string += "field1";
    request_string += "=";
    request_string += suhu;
    request_string += "&";
    request_string += "field2";
    request_string += "=";
    request_string += hum;
    Serial.println(String("GET ") + request_string + " HTTP/1.1\r\n" +
                 "Host: " + thingSpeakAddress + "\r\n" +
                 "Connection: close\r\n\r\n");
                 
    client.print(String("GET ") + request_string + " HTTP/1.1\r\n" +
                 "Host: " + thingSpeakAddress + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");

  }
}
