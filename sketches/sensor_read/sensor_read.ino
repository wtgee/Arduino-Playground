/* Reading of sensors off Arduino
 *
 *
 */

#include <stdarg.h>
#include <aJSON.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

aJsonObject *root;

DHT dht(DHTPIN, DHTTYPE);

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

  root = aJson.createObject();

  dht.begin();
}

void loop() {
  aJson.addNumberToObject(root,"temp",read_temp());
  aJson.addNumberToObject(root,"humidity",read_humidity());

  int photoCells[] = { 0,1,2,3,4  };

  char str[15];
  for (int i = 0; i < 5; i++){
    sprintf(str,"photocell.%d",i);
    aJson.addNumberToObject(root, str, read_photocell(i));
  }

  Serial.println(aJson.print(root));

  delay(500);
}

/*
* read_temp
 *
 * Reading temperature or humidity takes about 250 milliseconds!
 * Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 */
int read_temp() {

  int t = dht.readTemperature();

  if (isnan(t)) {
    return t;
  }

  return 0;
}

/*
* read_humidity
 *
 * Reading temperature or humidity takes about 250 milliseconds!
 * Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 */
int read_humidity() {

  int h = dht.readHumidity();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (!isnan(h)) {
      return h;
  }

  return 0;
}

/*
* read_photocell
 *
 * Reads the analog value from a photocell at given analog pin
 */
int read_photocell(int photocellPin) {
  int light_value = analogRead(photocellPin);

  if(!isnan(light_value)) {
      return light_value;
  }

  return 0;
}

