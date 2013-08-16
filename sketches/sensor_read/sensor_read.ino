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

  calibrate_analog();

  dht.begin();
}

void loop() {
  if (millis() - last_print > 1000) {
    // One second elapsed, send message.
    aJsonObject *msg = read_sensors();
    aJson.print(msg, &serial_stream);
    Serial.println(); // Add newline 
    aJson.deleteItem(msg);
    last_print = millis();
  }

  if (serial_stream.available()) {
    // First, skip any accidental whitespace like newlines.
    serial_stream.skip();
  }

  if (serial_stream.available()) {
    // Something real on input, let's take a look. 
    aJsonObject *msg = aJson.parse(&serial_stream);
    processMessage(msg);
    aJson.deleteItem(msg);
  }
}

/* Generate message like: { "analog": [0, 200, 400, 600, 800, 1000] } */
aJsonObject *read_sensors()
{
  aJsonObject *msg = aJson.createObject();

    // Read the analog values and map it to a percentage
  int analogValues[6];
  for (int i = 0; i < 6; i++) {
    analogValues[i] = map(analogRead(i), 0, 1023, 0, 100);
  }
  aJsonObject *analog = aJson.createIntArray(analogValues, 6);
  aJson.addItemToObject(msg, "analog", analog);

  // Temp/Humidity
  aJson.addNumberToObject(msg, "temp", dht.readTemperature());
  aJson.addNumberToObject(msg, "humidity", dht.readHumidity());

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
