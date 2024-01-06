#define SPEED_PIN D2 // BLUE wire
#define DIR_PIN D4 // WHITE wire

// YELLOW wire is not in use

// Wiring::
// Li-Ion_GND <-> Arduino_GND <-> BLDC_GND
// Li-Ion_V   <-> BLDC_GND
// BLDC_BLUE  <-> ARDUINO_6 (~PWM)
// BLDC_WHITE <-> ARDUINO_8


void setup() {
  pinMode(SPEED_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
}

void loop() {
  analogWrite(SPEED_PIN, 255); // from ~120 t0 255
}