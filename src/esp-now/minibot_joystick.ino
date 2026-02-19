#include <esp_now.h>
#include <WiFi.h>

// j1: X <-> pin 32; G <-> GND; V <-> 3.3V
// j2: X <-> pin 34; G <-> GND; V <-> 3.3V


// esp-now
// FC:B4:67:F1:C2:08
uint8_t broadcastAddress1[] = {0xCC, 0xDB, 0xA7, 0x2D, 0xD4, 0x8C};

typedef struct data_struct {
  int jx;
  int jy;
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
  analogReadResolution(10);
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
  data.jx = analogRead(34);
  data.jy = analogRead(32);
  Serial.println(String(data.jx) + "; " + String(data.jy));

  esp_err_t result = esp_now_send(0, (uint8_t *) &data, sizeof(data));
  delay(100);
}