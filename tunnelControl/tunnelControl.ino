#include <Servo.h>

#define CLOSED_PIN 2
#define OPENED_PIN 4
#define PHOTO_PIN A0

Servo serve;
bool luxState = false;
bool tmrState = false;

unsigned long pTmr = 0;

void setup() {
  pinMode(INPUT, CLOSED_PIN);
  pinMode(INPUT, OPENED_PIN);
  pinMode(INPUT, PHOTO_PIN);

  serve.attach(6);
  serve.write(90);

  Serial.begin(9600);
}

void loop() {
  int photoVal = analogRead(PHOTO_PIN); // tonnel: > 700
  int closedVal = digitalRead(CLOSED_PIN); // default: 1
  int openedVal = digitalRead(OPENED_PIN);

  unsigned long cTmr = millis();

  // if (photoVal < 700) {
  //   luxState = true;
  //   pTmr = millis();
  // }

  if (photoVal < 700 && closedVal == 0 && openedVal == 1) {
    serve.write(45);
  }
  else if (closedVal == 1 && openedVal == 0 && !tmrState) {
    serve.write(90);
    pTmr = millis();    
    tmrState = true;
  }
 
 if (tmrState) {
    cTmr = millis();
    if ((cTmr - pTmr > 5000) && closedVal == 1) {
      // close the door     
      serve.write(135);
    }
    else if ((cTmr - pTmr > 5000) && closedVal == 0) {
      serve.write(90);
      tmrState = false; 
    }
  }

  // serve.write(45);  // CCW
  //serve.write(90);   // CCW
  // serve.write(135); // CW
}
