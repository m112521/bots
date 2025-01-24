/**
https://github.com/madhephaestus/ESP32Servo/
*/

#include <ESP32Servo.h>
#include <TB6612_ESP32.h>

#define SERVO1_PIN 5
#define SERVO2_PIN 4
#define SERVO3_PIN 16
#define SERVO4_PIN 17

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
}

void loop() {
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  delay(2000);
  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  delay(2000);
  servo1.write(180);
  servo2.write(180);
  servo3.write(180);
  servo4.write(180);
  delay(2000);
}
