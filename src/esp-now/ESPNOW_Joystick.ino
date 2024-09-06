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