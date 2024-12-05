#include <Servo.h>

#define TOP_STOP 4
#define BOTTOM_STOP 2
#define PHOTO_PIN A0

Servo servo;

int state = 0; // RESET
int state_prev = 0;
int light_threshold = 500; // ~630 - closed; ~312 - open;

unsigned long t = 0;
unsigned long t_0 = 0;
unsigned long door_delay = 30000;

void setup() {
  pinMode(PHOTO_PIN, INPUT);
  
  servo.attach(6);
  servo.write(90);
  
  Serial.begin(9600);
}

void loop() {
  state_machine();
  //Serial.println(analogRead(PHOTO_PIN));

  if (state_prev != state) {
    Serial.println(state);
  }
}

void state_machine() {
  state_prev = state;

  switch(state) {
    case 0: // RESET
      if (!digitalRead(TOP_STOP)) {  
        servo.write(90);      
        state = 1;
        delay(2000);
      }
      else {
        servo.write(135);        
      }
      break;
    case 1: // START
      if (analogRead(PHOTO_PIN) < light_threshold) {
        state = 2;        
      } 
      break;

    case 2: // OPENING
      servo.write(45); // rotate forward
      if (!digitalRead(BOTTOM_STOP)) {
        state = 3;
      }
      break;

    case 3: // OPEN
      delay(1000);
      servo.write(90); // stop servo
      t_0 = millis();
      state = 4;
      break;

    case 4: // WAIT
      t = millis();
      if (t - t_0 > door_delay) {
        state = 5;        
      }
      break;  

    case 5: // CLOSING
      servo.write(135); // rotate backward
      if (!digitalRead(TOP_STOP)) {
        state = 6;        
      }      
      break;

    case 6: // CLOSED
      servo.write(90);
      state = 0;
      break;
  }
}