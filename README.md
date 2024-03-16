# bots

![Slice 3](https://github.com/m112521/bots/assets/85460283/3c20fe82-31df-4fae-954a-a83cff978d09)


PCB:

![photo_5449885385330121462_y](https://github.com/m112521/bots/assets/85460283/3382ff75-c3dc-4d82-9515-7627d5dfe2c6)


Test code:

```c++
/* 
  ESP32 PCA9685 Servo Control 
  esp32-pca9685.ino 
  Driving multiple servo motors with ESP32 and PCA9685 PWM module 
  Use I2C Bus 
 
  DroneBot Workshop 2020 
  https://dronebotworkshop.com 
*/ 
 
// Include Wire Library for I2C 
#include <Wire.h> 
 
// Include Adafruit PCA9685 Servo Library 
#include <Adafruit_PWMServoDriver.h> 
 
// Creat object to represent PCA9685 at default I2C address 
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40); 
 
// Define maximum and minimum number of "ticks" for the servo motors 
// Range from 0 to 4095 
// This determines the pulse width 
 
#define SERVOMIN  80  // Minimum value 
#define SERVOMAX  600  // Maximum value 
 
// Define servo motor connections (expand as required) 
#define SER0  0   //Servo Motor 0 on connector 0 
#define SER1  12  //Servo Motor 1 on connector 12 
#define SER2  8   //Servo Motor 0 on connector 0 
#define SER3  4  //Servo Motor 1 on connector 12 
 
// Variables for Servo Motor positions (expand as required) 
int pwm0; 
int pwm1; 
 
void setup() { 
 
  // Serial monitor setup 
  Serial.begin(115200); 
 
  // Print to monitor 
  Serial.println("PCA9685 Servo Test"); 
 
  // Initialize PCA9685 
  pca9685.begin(); 
 
  // Set PWM Frequency to 50Hz 
  pca9685.setPWMFreq(50); 
 
} 
 
void loop() { 
 
// pca9685.setPWM(SER0, 0, 80); 
// pca9685.setPWM(SER1, 0, 80); 
// pca9685.setPWM(SER2, 0, 80); 
// pca9685.setPWM(SER3, 0, 80); 
 
// delay(3000); 
 
// pca9685.setPWM(SER0, 0, 600); 
// pca9685.setPWM(SER1, 0, 600); 
// pca9685.setPWM(SER2, 0, 600); 
// pca9685.setPWM(SER3, 0, 600); 
 
// delay(3000); 
 
  // // Move Motor 0 from 0 to 180 degrees 
  for (int posDegrees = 0; posDegrees <= 180; posDegrees++) { 
 
    // Determine PWM pulse width 
    pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); 
    // Write to PCA9685 
    pca9685.setPWM(SER0, 0, pwm0); 
    pca9685.setPWM(SER3, 0, pwm0); 
    // Print to serial monitor 
    Serial.print("Motor 0 = "); 
    Serial.println(posDegrees); 
    delay(5); 
  } 
 
  // Move Motor 1 from 180 to 0 degrees 
  for (int posDegrees = 180; posDegrees >= 0; posDegrees--) { 
 
    // Determine PWM pulse width 
    pwm1 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); 
    // Write to PCA9685 
    pca9685.setPWM(SER1, 0, pwm1); 
    pca9685.setPWM(SER2, 0, pwm1); 
    // Print to serial monitor 
    Serial.print("Motor 1 = "); 
    Serial.println(posDegrees); 
    delay(5); 
  } 
 
  // Move Motor 0 from 180 to 0 degrees 
  for (int posDegrees = 180; posDegrees >= 0; posDegrees--) { 
 
    // Determine PWM pulse width 
    pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); 
    // Write to PCA9685 
    pca9685.setPWM(SER0, 0, pwm0); 
    pca9685.setPWM(SER3, 0, pwm0); 
    // Print to serial monitor 
    Serial.print("Motor 0 = "); 
    Serial.println(posDegrees); 
    delay(5); 
  } 
 
 
  // Move Motor 1 from 0 to 180 degrees 
  for (int posDegrees = 0; posDegrees <= 180; posDegrees++) { 
 
    // Determine PWM pulse width 
    pwm1 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); 
    // Write to PCA9685 
    pca9685.setPWM(SER1, 0, pwm1); 
    pca9685.setPWM(SER2, 0, pwm1); 
    // Print to serial monitor 
    Serial.print("Motor 1 = "); 
    Serial.println(posDegrees); 
    delay(5); 
  } 
 
 
}
```
