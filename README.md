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


RobotWEB:

```c++
#include <TB6612_ESP32.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#define AIN1 13 // ESP32 Pin D13 to TB6612FNG Pin AIN1
#define BIN1 12 // ESP32 Pin D12 to TB6612FNG Pin BIN1
#define AIN2 14 // ESP32 Pin D14 to TB6612FNG Pin AIN2
#define BIN2 27 // ESP32 Pin D27 to TB6612FNG Pin BIN2
#define PWMA 26 // ESP32 Pin D26 to TB6612FNG Pin PWMA
#define PWMB 25 // ESP32 Pin D25 to TB6612FNG Pin PWMB
#define STBY 33 // ESP32 Pin D33 to TB6612FNG Pin STBY

const char* ssid = "GalaxyAzamat";
const char* password = "vhrg8328";

#define RELAY_PIN 15 // pin G15

int speed = 255;
int direction = 0;
int slider = 0;
int fire = 0;
int led = 0;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

JSONVar readings;

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 1);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<style>
    body {height: 100vh;overflow: hidden;margin: 0;}
    .parent {
        display: grid;
        grid-template-columns: repeat(2, 1fr);
        grid-template-rows: repeat(2, 1fr);
        grid-column-gap: 0px;
        grid-row-gap: 0px;
        height: 100%;
        }
        
        .div1 { grid-area: 1 / 2 / 3 / 3; height: 100vh;}
        .div2 { grid-area: 1 / 1 / 2 / 2; padding: 0.5rem;}
        .div3 { grid-area: 2 / 1 / 3 / 2; }
        .parent-j {
        display: grid;
        grid-template-columns: repeat(3, 1fr);
        grid-template-rows: repeat(3, 1fr);
        grid-column-gap: 0px;
        grid-row-gap: 0px;
        height: 100%;
        /* justify-items: center;
        align-items: center; */
        }

        .div-u { grid-area: 1 / 2 / 2 / 3; border: 1px solid gray;padding:2rem 3rem; border-radius:100% 100% 0 0;/**display:flex;flex-wrap:nowrap;justify-content:center;align-items:center;**/}
        .div-l { grid-area: 2 / 1 / 3 / 2; border: 1px solid gray;padding:2.95rem 2.4rem; border-radius:100% 0 0 100%;}
        .div-d { grid-area: 3 / 2 / 4 / 3; border: 1px solid gray;padding:2rem 3rem; border-radius:0 0 100% 100%;}
        .div-r { grid-area: 2 / 3 / 3 / 4; border: 1px solid gray;padding:2.95rem 2.4rem; border-radius:0 100% 100% 0;}
        .div-s { grid-area: 2 / 2 / 3 / 3; border: 1px solid rgb(233, 80, 80);padding:3rem 2.3rem; }

        .analog {border:1px solid rgb(216, 211, 211);display: flex;border-radius:15px;justify-content: space-between;align-items: stretch;}
        .analog-value{font-size:5rem;}
        .btn-set{width: 35%;border-radius:0 10px 10px 0;border:none;background:#524FF0;color:white;font-size:1.3rem}
        .analog-data{display: flex;flex-wrap: wrap;align-content: stretch;justify-content: flex-start;padding: 0.5rem;}
        .div-slider{width:100%;}
        .slider{width: 90%;}
        .circ-btn{border-radius:100%;border:1px solid red;width:180px;height:180px;background:none;}
</style>
<div class="parent">
    <div class="div1">
        <div class="parent-j">
            <button class="div-u" id="btn-up">F</button>
            <button class="div-l" id="btn-lt">L</button>
            <button class="div-d" id="btn-dn">B</button>
            <button class="div-r" id="btn-rt">R</button>
            <button class="div-s" id="btn-sp">STOP</button>
        </div>
    </div>
    <div class="div2">
        <div class="analog">
            <div class="analog-data"><div class="analog-value" id="slider-txt">0</div>
                <div class="div-slider"><input id="slider-val" class="slider" type="range" value="0" min="0" max="255"/></div>
            </div> 
            <button class="btn-set" id="btn-set">SET</button>
        </div>
    </div>
    <div class="div3"><button class="circ-btn" id="btn-fire">RELAY</button><button class="circ-btn" id="btn-led">LED</button></div>
</div>

<script>
let gateway = `ws://${window.location.hostname}/ws`;

let sliderTxt = document.querySelector("#slider-txt");
let sliderVal = document.querySelector("#slider-val");
let fireBtn = document.querySelector("#btn-fire");
let ledBtn = document.querySelector("#btn-led");
let ledState = 0;
let fireState = 0;

let websocket;
window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
    initButtons();
}

function initButtons() {
  document.querySelector('#btn-up').addEventListener('click', ()=>{ websocket.send(JSON.stringify({dir:1})) });
  document.querySelector('#btn-dn').addEventListener('click', ()=>{ websocket.send(JSON.stringify({dir:2})) });
  document.querySelector('#btn-lt').addEventListener('click', ()=>{ websocket.send(JSON.stringify({dir:3})) });
  document.querySelector('#btn-rt').addEventListener('click', ()=>{ websocket.send(JSON.stringify({dir:4})) });
  document.querySelector('#btn-sp').addEventListener('click', ()=>{ websocket.send(JSON.stringify({dir:0})) });

  document.querySelector('#btn-set').addEventListener('click', ()=>{ websocket.send(JSON.stringify({slider: parseInt(sliderVal.value)})) });
  document.querySelector('#btn-led').addEventListener('click', ()=>{ 
    ledState = !ledState;
    toggleBg(ledBtn, ledState);
    websocket.send(JSON.stringify({led:ledState ? 1 : 0})); 
  });
  
  document.querySelector('#btn-fire').addEventListener('click', ()=>{ 
    fireState = !fireState;
    toggleBg(fireBtn, fireState);
    websocket.send(JSON.stringify({fire:fireState ? 1 : 0})); 
  });

  document.querySelector('#slider-val').addEventListener('change', ()=>{
    document.querySelector("#slider-txt").innerHTML = sliderVal.value;
  });
}

function toggleBg(btn, state) {
    if (state) {
        btn.style.background = '#ff0000';
    }
    else {
        btn.style.background = '#ffffff';
    }
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

function getReadings(){
    websocket.send("getReadings");
}

function onMessage(event) {
    websocket.send("getReadings");
}</script>
)rawliteral";


String getSensorReadings(){
  readings["s"] = String(slider);
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
    if (myObject.hasOwnProperty("slider")) {
      slider = (int)myObject["slider"];
    }
    else if (myObject.hasOwnProperty("fire")) {
      fire = (int)myObject["fire"];      
    }
    else if (myObject.hasOwnProperty("led")) {
      led = (int)myObject["led"];      
    }
    else if (myObject.hasOwnProperty("dir")) {
      direction = (int)myObject["dir"];
      move(direction, speed);      
    }

    String sensorReadings = getSensorReadings();
    notifyClients(sensorReadings);
  }
}

void move(int direction, int speed) {  
  if (direction == 1) { // Forward
    forward(motor1, motor2, speed);
  }
  else if (direction == 2) { // Backward
    back(motor1, motor2, speed);
  }
  else if (direction == 3) { // Left
    motor1.drive(-255); // speed, optional duration
    motor2.drive(255);
  }
  else if (direction == 4) { // Right
    motor1.drive(255); // speed, optional duration
    motor2.drive(-255);
  }
  else if (direction == 0) { // Stop
    motor1.brake();
    motor2.brake();
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
  pinMode(RELAY_PIN, OUTPUT);  

  initWiFi();
  initWebSocket();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  server.begin();
}

void loop()
{
  if (fire == 1) {
    digitalWrite(RELAY_PIN, HIGH);
  }
  else {
    digitalWrite(RELAY_PIN, LOW);
  }
  //Serial.println(String(direction) + " " + String(slider) + " " + String(fire) + " " + String(led));
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


Transmitter (JOYSTICK):

```c++
#include <esp_now.h>
#include <WiFi.h>

// WORKS FINE INLY WITH DOIT ESP32 30 pin
#define PTR_PIN 36
#define J1X_PIN 32
#define J1Y_PIN 35
#define J2X_PIN 33
#define J2Y_PIN 34
#define ButtonJ1 21
#define ButtonJ2 18
#define TMB_PIN 19

// esp-now
// FC:B4:67:F1:C2:08
uint8_t broadcastAddress1[] = {0xFC, 0xB4, 0x67, 0xF1, 0xC2, 0x08};

typedef struct data_struct {
  int ptr;
  int j1x;
  int j1y;
  int j2x;
  int j2y;
  int bj1;
  int bj2;  
  int tmb;
} data_struct;

data_struct data;
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
  delay(1000);
}

void loop() {
  data.ptr = analogRead(PTR_PIN);
  data.j1x = analogRead(J1X_PIN);
  data.j1y = analogRead(J1Y_PIN);
  data.j2x = analogRead(J2X_PIN);
  data.j2y = analogRead(J2Y_PIN);
  data.bj1 = digitalRead(ButtonJ1);
  data.bj2 = digitalRead(ButtonJ2);
  data.tmb = digitalRead(TMB_PIN);
  Serial.println(String(data.ptr) + " " + String(data.j1x) + " " + String(data.j1y) + " " + String(data.j2x) + " " + String(data.j2y) + " " + String(data.bj1) + " " + String(data.bj2) + " " + String(data.tmb));

  esp_err_t result = esp_now_send(0, (uint8_t *) &data, sizeof(data));
  delay(200);
}
```


Reciever (ROBOT):

```c++
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
```    
