// SDS011 dust sensor example
// -----------------------------
//
// By R. Zschiegner (rz@madavi.de). https://github.com/misan/SDS011
// April 2016

#include "SDS011.h"

float p10, p25;
int err;
int airPin = 23;
int airDutyCycle = 2000;
int airCount = 0;

SDS011 my_sds;

SDS011 my_sds_2;

#ifdef ESP32
HardwareSerial port2(2);
HardwareSerial port1(1);
#endif

void setup() {
  my_sds.begin(&port2, 17, 16);
  Serial.begin(115200);

  pinMode(airPin, OUTPUT);
}

void loop() {
  err = my_sds.read(&p25, &p10);
  if (!err) {
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
  }
  delay(100);
  if(airCount > airDutyCycle){
    airCount = 0;
    digitalWrite(airPin, HIGH);
  }
  else{
    airCount++;
    digitalWrite(airPin, LOW);
  }
}
