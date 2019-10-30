#include "MQ7.h"
#include <Wire.h>

int lastReading = 0;
MQ7 mq7(A0,5.0);

void setup() {
    Serial.begin(9600);
    Wire.begin(); 
}

void loop() {
    Serial.println(mq7.getPPM());
    lastReading= mq7.getPPM();
    Wire.beginTransmission(9); // transmit to device #9
    Wire.write(lastReading);              // sends x 
    Wire.endTransmission();    // stop transmitting
    delay(1000);
}