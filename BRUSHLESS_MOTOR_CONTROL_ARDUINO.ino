#define SPEED_PIN 26 // BLUE wire
#define DIR_PIN 35 // WHITE wire

// YELLOW wire is not in use

// Wiring::
// Li-Ion_GND <-> Arduino_GND <-> BLDC_GND
// Li-Ion_V   <-> BLDC_V
// BLDC_BLUE  <-> ARDUINO_6 (~PWM)
// BLDC_WHITE <-> ARDUINO_8

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;


void setup() {
  pinMode(SPEED_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(SPEED_PIN, ledChannel);
}

void loop() {
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
  }

  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    ledcWrite(ledChannel, dutyCycle);   
    delay(15);
  }

}