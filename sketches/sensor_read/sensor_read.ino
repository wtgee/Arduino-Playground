/* Reading of sensors off Arduino
 *
 *
 */

#include <stdarg.h>
#include <aJSON.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

unsigned long last_print = 0;
aJsonStream serial_stream(&Serial);

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

  dht.begin();
}

void loop() {
    /*
  root = aJson.createObject();

  aJson.addNumberToObject(root,"temp",read_temp());
  aJson.addNumberToObject(root,"humidity",read_humidity());

  int photoCells[] = { 0,1,2,3,4  };

  char str[15];
  for (int i = 0; i < 5; i++){
    sprintf(str,"photocell.%d",i);
    aJson.addNumberToObject(root, str, read_photocell(i));
  }

  Serial.print(aJson.print(root));

  Serial.println();
  delay(500);
  */




  if (millis() - last_print > 1000) {
    /* One second elapsed, send message. */
    aJsonObject *msg = createMessage();
    aJson.print(msg, &serial_stream);
    Serial.println(); /* Add newline. */
    aJson.deleteItem(msg);
    last_print = millis();
  }

  if (serial_stream.available()) {
    /* First, skip any accidental whitespace like newlines. */
    serial_stream.skip();
  }

  if (serial_stream.available()) {
    /* Something real on input, let's take a look. */
    aJsonObject *msg = aJson.parse(&serial_stream);
    processMessage(msg);
    aJson.deleteItem(msg);
  }
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

/* Generate message like: { "analog": [0, 200, 400, 600, 800, 1000] } */
aJsonObject *createMessage()
{
  aJsonObject *msg = aJson.createObject();

  int analogValues[6];
  for (int i = 0; i < 6; i++) {
    analogValues[i] = analogRead(i);
  }
  aJsonObject *analog = aJson.createIntArray(analogValues, 6);
  aJson.addItemToObject(msg, "analog", analog);

  return msg;
}

/* Process message like: { "pwm": { "8": 0, "9": 128 } } */
void processMessage(aJsonObject *msg)
{
  aJsonObject *pwm = aJson.getObjectItem(msg, "pwm");
  if (!pwm) {
    Serial.println("no pwm data");
    return;
  }

  const static int pins[] = { 8, 9 };
  const static int pins_n = 2;
  for (int i = 0; i < pins_n; i++) {
    char pinstr[3];
    snprintf(pinstr, sizeof(pinstr), "%d", pins[i]);

    aJsonObject *pwmval = aJson.getObjectItem(pwm, pinstr);
    if (!pwmval) continue; /* Value not provided, ok. */
    if (pwmval->type != aJson_Int) {
      Serial.print("invalid data type ");
      Serial.print(pwmval->type, DEC);
      Serial.print(" for pin ");
      Serial.println(pins[i], DEC);
      continue;
    }

    Serial.print("setting pin ");
    Serial.print(pins[i], DEC);
    Serial.print(" to value ");
    Serial.println(pwmval->valueint, DEC);
    analogWrite(pins[i], pwmval->valueint);
  }
}
