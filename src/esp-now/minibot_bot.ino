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

typedef struct data_struct {
  int jx;
  int jy;
} data_struct;

data_struct data;

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 3);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

Servo servo1;
Servo servo2;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  Serial.println(String(data.jx) + " " + String(data.jy));  
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
  if (data.jx > 600) {
    // Forward
    int speed = map(data.jx, 850, 1023, 150, 255);
    forward(motor1, motor2, speed); // 255 - max speed (voltage actually)
  }
  else if (data.jx < 400) {
    // Backward
    int speed = map(data.jx, 650, 0, 150, 255);
    back(motor1, motor2, speed);
  }
  else if (data.jy > 600) { 
    // Right
    int speed = map(data.jx, 600, 1023, 150, 255);
    motor1.drive(-speed); // speed, optional duration
    motor2.drive(speed);
  }
  else if (data.jy < 400) {  
    // Left
    int speed = map(data.jx, 400, 0, 150, 255);
    motor1.drive(speed); // speed, optional duration
    motor2.drive(-speed);
  }
  else {
    motor1.brake();
    motor2.brake();
  }
}