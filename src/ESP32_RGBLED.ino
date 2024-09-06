const int ledPinR = 25;
const int ledPinG = 33;
const int ledPinB = 32;

const int frequency = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

void setup() {
  ledcSetup(redChannel, frequency, resolution);
  ledcSetup(greenChannel, frequency, resolution);
  ledcSetup(blueChannel, frequency, resolution);
 
  ledcAttachPin(ledPinR, redChannel);
  ledcAttachPin(ledPinG, greenChannel);
  ledcAttachPin(ledPinB, blueChannel);

  Serial.begin(9600);  

  // pinMode(ledPinR, OUTPUT);
  // pinMode(ledPinG, OUTPUT);
  // pinMode(ledPinB, OUTPUT);
}

void loop() {
  int R255 = map(255, 0, 255, 0, 1023);
  int G255 = map(0, 0, 255, 0, 1023);
  int B255 = map(0, 0, 255, 0, 1023);

  ledcWrite(redChannel, 1023 - R255);
  ledcWrite(greenChannel, 1023 - G255);
  ledcWrite(blueChannel, 1023 - B255);

  delay(3000);

  R255 = map(0, 0, 255, 0, 1023);
  G255 = map(255, 0, 255, 0, 1023);
  B255 = map(0, 0, 255, 0, 1023);

  ledcWrite(redChannel, 1023 - R255);
  ledcWrite(greenChannel, 1023 - G255);
  ledcWrite(blueChannel, 1023 - B255);

  delay(3000);

  R255 = map(0, 0, 255, 0, 1023);
  G255 = map(0, 0, 255, 0, 1023);
  B255 = map(255, 0, 255, 0, 1023);

  ledcWrite(redChannel, 1023 - R255);
  ledcWrite(greenChannel, 1023 - G255);
  ledcWrite(blueChannel, 1023 - B255);

  delay(3000);
}
