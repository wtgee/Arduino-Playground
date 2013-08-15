#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#include <stdarg.h>

int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
byte val = 0;

void prints(char *fmt, ... ){
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.println(tmp);
}

void setup() {
  Serial.begin(115200); 

  // Send initial blank values
  prints("{\"humidity\": 0, \"temp\": 0, \"photocell_0\": 0}");
 
  dht.begin();
}

void loop() {
  read_temp_humidity();
  read_photocell(photocellPin);

  delay(1000);
}

/*
* read_temp_humidity
*
* Reading temperature or humidity takes about 250 milliseconds!
* Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
*/
void read_temp_humidity() {

  int h = dht.readHumidity();
  int t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    prints("Failed to read from DHT");
  } else {
    prints("{\"humidity\": %d, \"temp\": %d}",h,t);    
  }  
}

/*
* read_photocell
*
* Reads the analog value from a photocell at given analog pin
*/
void read_photocell(int photocellPin) {
  val = analogRead(photocellPin);

  if (isnan(val)) {
    prints("Failed to read from photocell %d",photocellPin);
  } else {
    // We want this to be a percentage of maximum (256)
    int percent = ((val*100)/256);
    
    prints("{\"photocell_%d\": %d}",photocellPin, percent);
  }  
}
