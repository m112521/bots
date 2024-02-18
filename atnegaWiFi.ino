void setup() {
  Serial.begin(115200);
  pinMode(8, OUTPUT);
}

void loop() {
  if(Serial.available()>0) {
    int val = Serial.parseInt();

    if (val == 5) {
      digitalWrite(8, HIGH);
      Serial.println("ON led");
    }
    else if (val == 6) {
      digitalWrite(8, LOW);
      Serial.println("Off led");
    }
  }
}
