// WORKS FINE INLY WITH DOIT ESP32 30 pin
#define PTR_PIN 26//4
#define J1X_PIN 32//34
#define J1Y_PIN 35//0
#define J2X_PIN 33
#define J2Y_PIN 34
#define ButtonJ1 4
#define ButtonJ2 15


typedef struct data_struct {
  int ptr;
  int j1x;
  int j1y;
  int j2x;
  int j2y;
  int bj1;
  int bj2;  
} data_struct;

data_struct data;

void setup() {
  Serial.begin(115200);
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
  Serial.println(String(data.ptr) + " " + String(data.j1x) + " " + String(data.j1y) + " " + String(data.j2x) + " " + String(data.j2y) + " " + String(data.bj1) + " " + String(data.bj2));
}