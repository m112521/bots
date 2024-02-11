#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

#define SPEED_1      D5 
#define DIR_1        D4
#define SPEED_2      D6
#define DIR_2        D7

int command = 0;
int timeout = 80;
bool mode = false;

struct Led {
    byte id;
    byte gpio;
    byte status;
} led_resource;

const char* wifi_ssid = "fablab";
const char* wifi_passwd = "fablab77";

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

void init_led_resource()
{
    led_resource.id = 0;
    led_resource.gpio = 0;
    led_resource.status = LOW;
}

int init_wifi() {
    int retries = 0;

    Serial.println("Connecting to WiFi AP..........");

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_passwd);
    // check the status of WiFi connection to be WL_CONNECTED
    while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
        retries++;
        delay(WIFI_RETRY_DELAY);
        Serial.print("#");
    }
    return WiFi.status(); // return the WiFi connection status
}

void get_leds() {
  
  //move(3);
  http_rest_server.send(200, "application/json", "Ok");
    // JsonDocument jsonObj;

    // if (led_resource.id == 0)
    //     http_rest_server.send(204);
    // else {
    //     jsonObj["id"] = led_resource.id;
    //     jsonObj["gpio"] = led_resource.gpio;
    //     jsonObj["status"] = led_resource.status;
    //     http_rest_server.send(200, "application/json", "Ok");
    // }
}

void config_rest_server_routing() {
    http_rest_server.on("/", HTTP_GET, []() {
        http_rest_server.send(200, "text/html", "Welcome to the ESP8266 REST Web Server");
    });
    http_rest_server.on("/leds", HTTP_GET, get_leds);
}

void setup(void) {
     pinMode(D5, OUTPUT);
     pinMode(D6, OUTPUT);
     pinMode(D7, OUTPUT);
     pinMode(D4, OUTPUT);
  
    Serial.begin(115200);

    init_led_resource();
    if (init_wifi() == WL_CONNECTED) {
        Serial.print("Connetted to ");
        Serial.print(wifi_ssid);
        Serial.print("--- IP: ");
        Serial.println(WiFi.localIP());
    }
    else {
        Serial.print("Error connecting to: ");
        Serial.println(wifi_ssid);
    }

    config_rest_server_routing();

    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop(void) {
    http_rest_server.handleClient();
}
