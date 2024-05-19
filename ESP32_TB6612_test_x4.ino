#include <TB6612_ESP32.h>

#define AIN1 13 // ESP32 Pin D13 to TB6612FNG Pin AIN1
#define BIN1 12 // ESP32 Pin D12 to TB6612FNG Pin BIN1
#define AIN2 14 // ESP32 Pin D14 to TB6612FNG Pin AIN2
#define BIN2 27 // ESP32 Pin D27 to TB6612FNG Pin BIN2
#define PWMA 26 // ESP32 Pin D26 to TB6612FNG Pin PWMA
#define PWMB 25 // ESP32 Pin D25 to TB6612FNG Pin PWMB
#define STBY 33 // ESP32 Pin D33 to TB6612FNG Pin STBY

#define AIN12 2 
#define AIN22 15
#define PWMA2 4
#define STBY2 16

#define BIN12 18
#define BIN22 19
#define PWMB2 5 


//Motor motor1 = Motor(AIN1, AIN2, PWMA, 1, STBY, 5000, 8, 1);
Motor motor3 = Motor(BIN12, BIN22, PWMB2, 1, STBY2, 5000, 8, 1);

void setup() {
  // put your setup code here, to run once:
  motor3.drive(255, 3000);
  motor3.brake();  
}

void loop() {
}
