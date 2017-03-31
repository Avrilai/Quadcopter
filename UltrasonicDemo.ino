#include "Ultrasonic.h"

void setup() {
  Serial.begin(9600);
  Ultrasonic ult1 = ultrasonic(12,13);
  // put your setup code here, to run once:
}

void loop() {
  Serial.print(ult1.Ranging(CM));
  Serial.print("cm");
  delay(100);
  // put your main code here, to run repeatedly:

}
