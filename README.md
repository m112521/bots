# bots

![Slice 3](https://github.com/m112521/bots/assets/85460283/3c20fe82-31df-4fae-954a-a83cff978d09)


Modela Aluminum:
- Шаг: 0.15
- Шаг по Z: 0.15-0.3
- Рабочая подача: 6
- Подача врезания: 4

PCB:

![photo_5449885385330121462_y](https://github.com/m112521/bots/assets/85460283/3382ff75-c3dc-4d82-9515-7627d5dfe2c6)



PCB TB6612 Pins (ESP32 30 pins - DOIT DEV1):

```c++
// #include <Arduino.h>
#include <TB6612_ESP32.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#define ENCA1 32 // YELLOW
#define ENCA2 19 // WHITE

#define ENCB1 22 // YELLOW // this on is used
#define ENCB2 23 // GREEN 
// BLACK - GND
// BLUE - 3V3

#define AIN1 13 // ESP32 Pin D13 to TB6612FNG Pin AIN1
#define BIN1 12 // ESP32 Pin D12 to TB6612FNG Pin BIN1
#define AIN2 14 // ESP32 Pin D14 to TB6612FNG Pin AIN2
#define BIN2 27 // ESP32 Pin D27 to TB6612FNG Pin BIN2
#define PWMA 26 // ESP32 Pin D26 to TB6612FNG Pin PWMA
#define PWMB 25 // ESP32 Pin D25 to TB6612FNG Pin PWMB
#define STBY 33 // ESP32 Pin D33 to TB6612FNG Pin STBY

#define AIN12 17
#define BIN12 4
#define AIN22 5
#define BIN22 2
#define PWMA2 18
#define PWMB2 15
#define STBY2 16


const char* ssid = "GalaxyAzamat";
const char* password = "vhrg8328";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

JSONVar readings;

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 1);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);
Motor motor3 = Motor(AIN12, AIN22, PWMA2, offsetA, STBY2, 5000, 8, 1);
Motor motor4 = Motor(BIN12, BIN22, PWMB2, offsetB, STBY2, 5000, 8, 2);

volatile int posiA1 = 0;
volatile int posiB1 = 0;

long prevT = 0;
float eprev = 0;
float eprev2 = 0;
float eintegral = 0;

int target = 0;

double kp = 1;
double kd = 0.025;
double ki = 0.0;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<p>Target=<span id="s1"></span></p>
<input id="target" type="number"/>
<hr/>
<p>Current=<span id="s5"></span></p>
<hr/>
<p>P=<span id="s2"></span></p>
<input id="p-textbox" type="number" value="1"/>
<hr/>
<p>D=<span id="s3"></span></p>
<input id="d-textbox" type="number" value="0.025"/>
<hr/>
<p>I=<span id="s4"></span></p>
<input id="i-textbox" type="number" value="0.0"/>

<!-- <input id="slider" type="range" min="0" max="255"/> -->
<button id="yo">SET</button>
<script>
let gateway = `ws://${window.location.hostname}/ws`;
let target = document.querySelector("#target");
let kp = document.querySelector("#p-textbox");
let kd = document.querySelector("#d-textbox");
let ki = document.querySelector("#i-textbox");

let websocket;
window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
    initButtons();
}

function initButtons() {
  document.getElementById('yo').addEventListener('click', forwardMove);
}

function forwardMove(){
  //websocket.send('f');
  console.log(parseFloat(ki.value));
  websocket.send(JSON.stringify({target: parseInt(target.value),kp:parseInt(kp.value),kd:parseFloat(kd.value),ki:parseFloat(ki.value)}));
}

function getReadings(){
    websocket.send("getReadings");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data);
    let myObj = JSON.parse(event.data);
    let keys = Object.keys(myObj);

    for (let i = 0; i < keys.length; i++){
        let key = keys[i];
        document.getElementById(key).innerHTML = myObj[key];
    }
    websocket.send("getReadings");
}</script>
)rawliteral";

void IRAM_ATTR ISRA1(){
   int b = digitalRead(ENCA2);
  if(b > 0){
    posiA1++;
  }
  else{
    posiA1--;
  }
}

void IRAM_ATTR ISRB1(){
  int b = digitalRead(ENCB2);
  if(b > 0){
    posiB1++;
  }
  else{
    posiB1--;
  }
}

String getSensorReadings(){
  // int val = analogRead(ptrPin);
  // Serial.println(val);
  readings["s1"] = String(target);
  readings["s2"] =  String(kp);
  readings["s3"] = String(kd);
  readings["s4"] = String(ki);
  readings["s5"] = String(posiB1);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    
    JSONVar myObject = JSON.parse((const char*)data);
    if (myObject.hasOwnProperty("target")) {
      target = (int)myObject["target"];
      kp = (int)myObject["kp"];
      ki = (double)myObject["ki"];
      kd = (double)myObject["kd"];
      // Serial.print("myObject[\"target\"] = ");
      // ledcWrite(ledChannel, brightness);
    }

    String sensorReadings = getSensorReadings();
    //Serial.println(sensorReadings);
    notifyClients(sensorReadings);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  Serial.begin(115200);
  
  pinMode(ENCA1,INPUT);
  pinMode(ENCA2,INPUT);
  pinMode(ENCB1,INPUT);
  pinMode(ENCB2,INPUT);

  attachInterrupt(ENCA1, ISRA1, RISING);
  attachInterrupt(ENCB1, ISRB1, RISING);
  //testRun();

  initWiFi();
  initWebSocket();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  server.begin();
}

void loop()
{
  //int target = 325*sin(prevT/1e6);
  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  int posB1 = 0;
  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    posB1 = posiB1;
  //}

  // error
  int e = posB1 - target;
  // derivative
  float dedt = (e-eprev)/(deltaT);
  // integral
  eintegral = eintegral + e*deltaT;
  // control signal
  float u = kp*e + kd*dedt + ki*eintegral;

  // motor power
  float pwr = fabs(u);
  if( pwr > 255 ){
    pwr = 255;
  }

  // motor direction
  int dir = 1;
  if(u<0){
    dir = -1;
  }

  //setMotor(dir,pwr,PWM,IN1,IN2);
  forward(motor1, motor2, dir*pwr);
  forward(motor3, motor4, dir*pwr);

  // store previous error
  eprev = e;

  // Serial.print(target);
  // Serial.print(" ");
  // Serial.print(posA1);
  // Serial.println();
    // forward(motor1, motor2, 255);
    // delay(2000); // Short delay to catch your robot
    // motor1.drive(255, 2000);       // Turn Motor 1 for 2 seconds at full speed
    // //motor1.drive(-255,2000);
    // motor1.brake();
    // forward(motor1, motor2, 255); //back(motor1, motor2, -255); // Reverse Motor 1 and Motor 2 for 1 seconds at full speed
    // left(motor1, motor2, 255); // brake(motor1, motor2);
}
```


PCB TB6612 Pins (ESP32 38 pins - ESP32 Dev Module):

- servo and PWM together не работают, если пин серы на противоположной стороне от пина PWM (мотор вибрирует) 

![ESP32-38 PIN-DEVBOARD](https://github.com/m112521/bots/assets/85460283/07154b7d-f3ae-4c9f-96c4-04e40ba8b598)

Which pins to use how: 

https://www.studiopieters.nl/esp32-pinout/

Library intall:

![lib](https://github.com/user-attachments/assets/8b4a06a5-9e49-469b-ba6e-e251a83007a1)

TB6612 lib:

https://github.com/pablopeza/TB6612FNG_ESP32


```c++
#include <Servo.h>
#include <TB6612_ESP32.h>

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

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 3);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

Servo servo1 = Servo();
Servo servo2 = Servo();

void setup() {
  pinMode(RELAY_PIN, OUTPUT);  
}

void loop() {
  digitalWrite(RELAY_PIN, LOW);
  forward(motor2, motor1, 255);        // Forward Motor 1 and Motor 2 for 1 seconds at full speed
  
  servo1.write(SERVO1_PIN, 0);
  servo2.write(SERVO2_PIN, 0);

  delay(2000); 
  motor1.brake();
  motor2.brake();
  digitalWrite(RELAY_PIN, HIGH);
  servo1.write(SERVO1_PIN, 180);
  servo2.write(SERVO2_PIN, 180);
  delay(2000); 
}
```

MAC:

```c++
// RX MAC
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){
  Serial.println(WiFi.macAddress());
}
```


Transmitter:

```c++
// TRANSMITTER (JOYSTICK)
#include <esp_now.h>
#include <WiFi.h>

// ADC1 pins for analor input: 32, 33, 34, 35, 36, 39
#define PTR_PIN 35

// 30:AE:A4:27:10:80
// EC:62:60:83:42:C4
// E4:65:B8:4C:6B:08

uint8_t broadcastAddress1[] = {0xE4, 0x65, 0xB8, 0x4C, 0x6B, 0x08};

typedef struct test_struct {
  int x;
  int y;
} test_struct;

test_struct test;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  Serial.begin(115200);
  pinMode(PTR_PIN, INPUT);
 
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  test.x = analogRead(PTR_PIN);
  test.y = random(0,20);
  Serial.println(test.x);
 
  esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
```


Reciever:

```c++
// RX (ROBOT)
#include <Servo.h>
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
  int x;
  int y;
} test_struct;

test_struct myData;

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 3);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

Servo servo1 = Servo();
Servo servo2 = Servo();

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(myData.x);
  Serial.print("y: ");
  Serial.println(myData.y);
  Serial.println();
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);  

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if (myData.x > 2000) {
    forward(motor2, motor1, 200); 
    digitalWrite(RELAY_PIN, HIGH);
    servo1.write(SERVO1_PIN, 180);
    servo2.write(SERVO2_PIN, 180);
  }
  else {
    motor1.brake();
    motor2.brake();
    digitalWrite(RELAY_PIN, LOW);
    servo1.write(SERVO1_PIN, 0);
    servo2.write(SERVO2_PIN, 0);
  }
}
```    
