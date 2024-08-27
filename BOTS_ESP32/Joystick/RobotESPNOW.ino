// RX (ROBOT)
#include <ESP32Servo.h>
#include <TB6612_ESP32.h>
#include <esp_now.h>
#include <WiFi.h>

#define BIN1 12 // ESP32 Pin D12 to TB6612FNG Pin BIN1
#define BIN2 27 // ESP32 Pin D27 to TB6612FNG Pin BIN2
#define AIN1 13 // ESP32 Pin D13 to TB6612FNG Pin AIN1
#define AIN2 14 // ESP32 Pin D14 to TB6612FNG Pin AIN2
#define STBY 33 // ESP32 Pin D33 to TB6612FNG Pin STBY
#define PWMA 26 // ESP32 Pin D26 to TB6612FNG Pin PWMA
#define PWMB 25 // ESP32 Pin D25 to TB6612FNG Pin PWMB


#define RELAY_PIN 15 // pin G15

#define SERVO1_PIN 5
#define SERVO2_PIN 4

typedef struct test_struct {
  int ptr = 0;
  int j1x = 1800;
  int j1y = 1800;
  int j2x = 1800;
  int j2y = 1800;
  int bj1 = 0;
  int bj2 = 0;  
  int tmb = 0;
} test_struct;

test_struct myData;

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 3);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

Servo servo1;
Servo servo2;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.println(String(myData.ptr) + " " + String(myData.j1x) + " " + String(myData.j1y) + " " + String(myData.j2x) + " " + String(myData.j2y) + " " + String(myData.bj1) + " " + String(myData.bj2) + " " + String(myData.tmb));  
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);  
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
  // servo1.write(posDegrees);
}

void loop() {
  // myData.ptr, myData.j1x, myData.j1y, myData.j2x, myData.j2y, myData.bj1, yData.bj2, myData.tmb
  // Forward/Back: myData.j2y
  // Left/Right:  myData.j1x (or j2x)
  // [1700-1900]

  if (myData.j2y > 2000) {
    // Forward
    forward(motor1, motor2, 255); // 255 - max speed (voltage actually)
  }
  else if (myData.j2y < 1600) {
    // Backward
    back(motor1, motor2, 255);
  }
  else if (myData.j1x > 2000) { // or j2x
    // Right
    motor1.drive(-255); // speed, optional duration
    motor2.drive(255);
  }
  else if (myData.j1x < 1600) { // or j2x
    // Left
    motor1.drive(255); // speed, optional duration
    motor2.drive(-255);
  }
  else {
    motor1.brake();
    motor2.brake();
  }
      
  // Realy control with tumbler
  if (myData.tmb == 1) {
    digitalWrite(RELAY_PIN, HIGH);
  }
  else {
    digitalWrite(RELAY_PIN, LOW);
  }
}