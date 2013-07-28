/* Photocell simple testing sketch. 
 
Connect one end of the photocell to 5V, the other end to Analog 0.
Then connect one end of a 10K resistor from Analog 0 to ground 
Connect LED from pin 11 through a resistor to ground 
For more information see http://learn.adafruit.com/photocells */
 
int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
int LEDpin = 11;          // connect Red LED to pin 11 (PWM pin)
int LEDbrightness;        // 
byte val = 0;
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
}
 
void loop(void) {
  val = analogRead(photocellPin);
  Serial.println(val,DEC);
  adjust_led(val);
  delay(100);
}

void adjust_led(int photocellReading){
  // LED gets brighter the darker it is at the sensor
  // that means we have to -invert- the reading from 0-1023 back to 1023-0
  photocellReading = 1023 - int(photocellReading);

  if(photocellReading < 0){
    photocellReading = 0;
  }
  if(photocellReading > 250){
    //now we have to map 0-1023 to 0-255 since thats the range analogWrite uses
    LEDbrightness = map(photocellReading, 250, 600, 0, 255);
  } else {
    LEDbrightness = 0;
  }

  analogWrite(LEDpin, LEDbrightness);
}
