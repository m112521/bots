#include <TB6612_ESP32.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#define AIN1 13
#define BIN1 12
#define AIN2 14
#define BIN2 27
#define PWMA 26
#define PWMB 25
#define STBY 33

const char* ssid = "bot1";
const char* password = "12345678";

#define RELAY_PIN 15
#define LED_PIN 2 

int speed = 0; 
int direction = 0;
int fire = 0;
int led = 0;

unsigned long lastCommandTime = 0; 
const long COMMAND_TIMEOUT = 3000; 

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

JSONVar readings;

const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 1);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Bot Controller</title>
    <style>
        :root {
            --bg-color: #121212;
            --card-bg: #1e1e1e;
            --text-color: #ffffff;
            --btn-bg: #2c2c2c;
            --btn-active: #444444;
            --accent: #bb86fc;
            --fire-color: #cf6679;
            --led-color: #03dac6;
        }

        * { box-sizing: border-box; margin: 0; padding: 0; }

        body {
            background-color: var(--bg-color);
            color: var(--text-color);
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            height: 100dvh; /* Dynamic viewport height prevents overflow on mobile address bars */
            width: 100vw;
            overflow: hidden;
            display: flex;
            justify-content: center;
            align-items: center;
            -webkit-user-select: none;
            user-select: none;
            touch-action: none; /* Prevent all default browser touch behaviors */
        }

        .controller {
            display: flex;
            flex-direction: column;
            width: 100%;
            max-width: 500px;
            height: 100%;
            padding: 15px;
            gap: 15px;
        }

        /* --- D-PAD SECTION --- */
        .dpad-section {
            flex: 1; /* Takes up remaining vertical space */
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 0; /* Prevents flexbox overflow */
        }

        .dpad {
            position: relative;
            width: min(70vw, 350px);
            aspect-ratio: 1 / 1; /* FORCES A PERFECT SQUARE - NO STRETCHING */
            max-height: 100%;
        }

        .dpad-btn {
            position: absolute;
            width: 33.33%;
            height: 33.33%;
            background-color: var(--btn-bg);
            border: none;
            border-radius: 12px;
            display: flex;
            justify-content: center;
            align-items: center;
            outline: none;
            -webkit-tap-highlight-color: transparent;
            transition: background-color 0.1s;
        }

        .dpad-btn:active { background-color: var(--btn-active); }

        /* Positioning the cross */
        .up    { top: 0;    left: 33.33%; }
        .down  { bottom: 0; left: 33.33%; }
        .left  { top: 33.33%; left: 0;    }
        .right { top: 33.33%; right: 0;   }

        /* CSS Triangles for flawless arrows */
        .arrow { width: 0; height: 0; }
        .arrow-up    { border-left: 12px solid transparent; border-right: 12px solid transparent; border-bottom: 18px solid white; }
        .arrow-down  { border-left: 12px solid transparent; border-right: 12px solid transparent; border-top: 18px solid white; }
        .arrow-left  { border-top: 12px solid transparent; border-bottom: 12px solid transparent; border-right: 18px solid white; }
        .arrow-right { border-top: 12px solid transparent; border-bottom: 12px solid transparent; border-left: 18px solid white; }


        /* --- SPEED SECTION --- */
        .speed-section {
            flex: 0 0 auto; /* Only takes space it needs */
            background-color: var(--card-bg);
            border-radius: 20px;
            padding: 15px 20px;
            display: flex;
            align-items: center;
            gap: 20px;
        }

        .speed-label {
            font-size: 18px;
            min-width: 50px;
            text-align: center;
            color: var(--accent);
            font-weight: bold;
        }

        .slider {
            flex: 1;
            -webkit-appearance: none;
            appearance: none;
            height: 8px;
            background: #444;
            border-radius: 5px;
            outline: none;
        }

        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 30px;
            height: 30px;
            border-radius: 50%;
            background: var(--accent);
            cursor: pointer;
        }

        /* --- ACTION SECTION --- */
        .action-section {
            flex: 0 0 auto; /* Only takes space it needs */
            display: flex;
            justify-content: space-around;
            gap: 20px;
            padding-bottom: 10px;
        }

        .action-btn {
            flex: 1;
            aspect-ratio: 1.2 / 1; /* Slightly wider than tall */
            max-height: 120px;
            background-color: var(--btn-bg);
            border: 3px solid transparent;
            border-radius: 20px;
            color: var(--text-color);
            font-size: 18px;
            font-weight: bold;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            gap: 8px;
            outline: none;
            -webkit-tap-highlight-color: transparent;
            transition: all 0.2s;
        }

        .indicator {
            width: 16px;
            height: 16px;
            border-radius: 50%;
            background-color: #555;
            transition: background-color 0.2s;
        }

        .fire-active { border-color: var(--fire-color); }
        .fire-active .indicator { background-color: var(--fire-color); box-shadow: 0 0 10px var(--fire-color); }

        .led-active { border-color: var(--led-color); }
        .led-active .indicator { background-color: var(--led-color); box-shadow: 0 0 10px var(--led-color); }

    </style>
</head>
<body>

    <div class="controller">
        <!-- D-Pad -->
        <div class="dpad-section">
            <div class="dpad">
                <button class="dpad-btn up" id="btn-up"><div class="arrow arrow-up"></div></button>
                <button class="dpad-btn left" id="btn-lt"><div class="arrow arrow-left"></div></button>
                <button class="dpad-btn right" id="btn-rt"><div class="arrow arrow-right"></div></button>
                <button class="dpad-btn down" id="btn-dn"><div class="arrow arrow-down"></div></button>
            </div>
        </div>

        <!-- Speed Slider -->
        <div class="speed-section">
            <div class="speed-label"><span id="slider-txt">0</span><br>SPD</div>
            <input type="range" id="slider-val" class="slider" min="0" max="255" value="0">
        </div>

        <!-- Action Buttons -->
        <div class="action-section">
            <button class="action-btn" id="btn-fire">
                <div class="indicator"></div>
                RELAY
            </button>
            <button class="action-btn" id="btn-led">
                <div class="indicator"></div>
                LED
            </button>
        </div>
    </div>

<script>
let gateway = `ws://${window.location.hostname}/ws`;
let websocket;

window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
    initButtons();
}

function initButtons() {
  document.querySelector('#btn-up').addEventListener('touchstart', (e)=>{ e.preventDefault(); websocket.send(JSON.stringify({dir:11})); });
  document.querySelector('#btn-up').addEventListener('touchend', ()=>{ websocket.send(JSON.stringify({dir:12})); });

  document.querySelector('#btn-dn').addEventListener('touchstart', (e)=>{ e.preventDefault(); websocket.send(JSON.stringify({dir:21})); });
  document.querySelector('#btn-dn').addEventListener('touchend', ()=>{ websocket.send(JSON.stringify({dir:22})); });

  document.querySelector('#btn-lt').addEventListener('touchstart', (e)=>{ e.preventDefault(); websocket.send(JSON.stringify({dir:31})); });
  document.querySelector('#btn-lt').addEventListener('touchend', ()=>{ websocket.send(JSON.stringify({dir:32})); });

  document.querySelector('#btn-rt').addEventListener('touchstart', (e)=>{ e.preventDefault(); websocket.send(JSON.stringify({dir:41})); });
  document.querySelector('#btn-rt').addEventListener('touchend', ()=>{ websocket.send(JSON.stringify({dir:42})); });

  document.querySelector('#btn-led').addEventListener('click', ()=>{ 
    let btn = document.querySelector('#btn-led');
    btn.classList.toggle('led-active');
    let state = btn.classList.contains('led-active') ? 1 : 0;
    websocket.send(JSON.stringify({led: state})); 
  });
  
  document.querySelector('#btn-fire').addEventListener('click', ()=>{ 
    let btn = document.querySelector('#btn-fire');
    btn.classList.toggle('fire-active');
    let state = btn.classList.contains('fire-active') ? 1 : 0;
    websocket.send(JSON.stringify({fire: state})); 
  });

  document.querySelector('#slider-val').addEventListener('input', (e)=>{
    let val = e.target.value;
    document.querySelector("#slider-txt").innerHTML = val;
    websocket.send(JSON.stringify({slider: parseInt(val)}));
  });
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
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log("Data received", event.data);
}
</script>
</body>
</html>
)rawliteral";


String getSensorReadings(){
  readings["s"] = String(speed); 
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

void initWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.print("Access Point started. Connect to WiFi: ");
  Serial.println(ssid);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    
    lastCommandTime = millis(); 

    JSONVar myObject = JSON.parse((const char*)data);
    
    if (strcmp((const char*)data, "getReadings") == 0) {
      notifyClients(getSensorReadings());
      return;
    }

    bool stateChanged = false;

    if (myObject.hasOwnProperty("slider")) {
      speed = (int)myObject["slider"];
      stateChanged = true;
    }
    else if (myObject.hasOwnProperty("fire")) {
      fire = (int)myObject["fire"];      
      stateChanged = true;
    }
    else if (myObject.hasOwnProperty("led")) {
      led = (int)myObject["led"];      
      stateChanged = true;
    }
    else if (myObject.hasOwnProperty("dir")) {
      direction = (int)myObject["dir"];
      if (direction != 0) {
        move(direction, speed);      
      }
    }

    if (stateChanged) {
      notifyClients(getSensorReadings());
    }
  }
}

void move(int direction, int speed) {
  if (direction == 11) { 
    forward(motor1, motor2, speed);
  }
  else if (direction == 12 || direction == 22 || direction == 32 || direction == 42) { 
    motor1.brake();
    motor2.brake();
  }
  else if (direction == 21) { 
    back(motor1, motor2, speed);
  }
  else if (direction == 31) { 
    motor1.drive(-speed); 
    motor2.drive(speed);
  }
  else if (direction == 41) { 
    motor1.drive(speed); 
    motor2.drive(-speed);
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
  pinMode(LED_PIN, OUTPUT);

  initWiFi();
  initWebSocket();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  server.begin();
}

void loop()
{
  ws.cleanupClients();

  digitalWrite(RELAY_PIN, fire);
  digitalWrite(LED_PIN, led);

  if (direction != 0 && (millis() - lastCommandTime > COMMAND_TIMEOUT)) {
    Serial.println("Command timeout! Stopping motors.");
    motor1.brake();
    motor2.brake();
    direction = 0;
  }
}
