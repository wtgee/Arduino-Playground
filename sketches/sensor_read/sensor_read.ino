#include <json_arduino.h>

#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#include <stdarg.h>

char json_string[256];
token_list_t *token_list = NULL;

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
  prints("{\"msg\": \"Starting ee}\"}");
 
  dht.begin();
}

void loop() {
  read_temp_humidity();

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
