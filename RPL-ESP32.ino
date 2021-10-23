#include "DHT.h"

#define RELAYPIN_1 4
#define RELAYPIN_2 5

#define DHTPIN 3     // Digital pin connected to the DHT sensor
// Pin that can be used =  3, 4, 5, 12, 13 or 14
// Pin 15 can work but DHT must be disconnected during program upload.
#define DHTTYPE DHT11   // DHT 11
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht(DHTPIN, DHTTYPE);

//#define DEBUG //Uncomment for Printing Variable

void setup() {
  Serial.begin(9600);
  
  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);
  pinMode(RELAYPIN_1,OUTPUT);
  pinMode(RELAYPIN_2,OUTPUT);
  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature(); //Celcius
  float f = dht.readTemperature(true); //Farenheit

  //==== WARNING EXPERIMENTAL CODE !! ====//
  // Used for re-read the sensor if it's reading failed 
  //
  //  if (isnan(h) || isnan(t) || isnan(f)) {
  //    Serial.println(F("Failed to read Sensor !!"));
  //    return;
  //  }
  //
  //======================================//
  
  float hif = dht.computeHeatIndex(f, h); // Compute heat index in Fahrenheit (the default)
  float hic = dht.computeHeatIndex(t, h, false); // Compute heat index in Celsius (isFahreheit = false)

#ifdef DEBUG
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
#endif
  
}
