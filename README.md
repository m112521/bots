# bots


BOM:
- NRF,
- NRF+,
- x2 5v to 3.3 v,
- Motor Shield,
- Arduino Leonardo/Amperka Iskra Neo,
- x2 18650,
- x2 DC motor,
- joystick.


1. RF24 by TMRh20 lib install (through Arduino IDE library manager).

![lib install](https://user-images.githubusercontent.com/85460283/214080122-c7909fbe-ddb3-4348-938a-55308347660f.png)


2. Joystick code: 

``` c++
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10);
int data[2];  

const int xPin = A1;
const int yPin = A0;
const int buttonPin = 2;

int xPosition = 0;
int yPosition = 0;
int buttonState = 0;

int speed = 0;
int direction = 0;
                                
void setup(){
    radio.begin();
    radio.setChannel(5);           
    radio.setDataRate     (RF24_1MBPS); 
    radio.setPALevel      (RF24_PA_HIGH);
    radio.openWritingPipe (0x1234567890LL);  
    
    pinMode(xPin, INPUT);
    pinMode(yPin, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);
}
void loop(){
    xPosition = analogRead(xPin);
    yPosition = analogRead(yPin);
    buttonState = digitalRead(buttonPin);
    

    if (xPosition > 520) {
      // go backward
      direction = 0; // 0 - Forward, 1 - backward, 2 - right, 3 - left
      speed = map(xPosition, 520, 1023, 5, 255);
    }
    else if (xPosition < 510) {
      // go backward
      direction = 1;
      speed = map(xPosition, 520, 0, 5, 255);
    }
    else if (yPosition > 520) {
      // go right
      direction = 2; // 0 - Forward, 1 - backward, 2 - right, 3 - left
      speed = 255; //map(xPosition, 520, 1023, 5, 255);
    }
    else if (yPosition < 510) {
      // go left
      direction = 3; // 0 - Forward, 1 - backward, 2 - right, 3 - left
      speed = map(xPosition, 520, 0, 5, 255);
    }
    else {
      direction = 4;
      speed = 0;
    }

    data[0] = direction;
    data[1] = speed;
    radio.write(&data, sizeof(data));
}

```

3. Bot code:

``` c++
#include <SPI.h>                                         
#include <nRF24L01.h>                                    
#include <RF24.h>        

#define SPEED_1      5 
#define DIR_1        4
 
#define SPEED_2      6
#define DIR_2        7

RF24 radio(8, 9); // nRF24L01+ (CE, CSN)
int data[2]; 

void setup(){
  Serial.begin(9600);
  digitalWrite(DIR_1, LOW);
  analogWrite(SPEED_1, 250);

  delay(1000);

  digitalWrite(DIR_1, LOW);
  analogWrite(SPEED_1, 0);
    
  radio.begin();                                        
  radio.setChannel(5);
  radio.setDataRate     (RF24_1MBPS);
  radio.setPALevel      (RF24_PA_HIGH);
  radio.openReadingPipe (1, 0x1234567890LL);
  radio.startListening  (); 
//  radio.stopListening   (); 

  // motor pins
  for (int i = 4; i < 8; i++) {     
    pinMode(i, OUTPUT);
  }

  Serial.println("test");
}

void loop(){
    if(radio.available()){
        radio.read(&data, sizeof(data));

        // 0 - Forward, 1 - backward, 2 - right, 3 - left
        switch (data[0]) {
          case 0: {
            digitalWrite(DIR_1, HIGH); // set direction
            analogWrite(SPEED_1, data[1]); // set speed

            digitalWrite(DIR_2, HIGH); // set direction
            analogWrite(SPEED_2, data[1]); // set speed

            break;
          }
          case 1: {
            digitalWrite(DIR_1, LOW); // set direction
            analogWrite(SPEED_1, data[1]); // set speed

            digitalWrite(DIR_2, LOW); // set direction
            analogWrite(SPEED_2, data[1]); // set speed

            break;
          }
          case 2: {
            digitalWrite(DIR_1, HIGH); // set direction
            analogWrite(SPEED_1, data[1]); // set speed

            digitalWrite(DIR_2, LOW); // set direction
            analogWrite(SPEED_2, data[1]); // set speed

            break;
          }
          case 3: {
            digitalWrite(DIR_1, LOW); // set direction
            analogWrite(SPEED_1, data[1]); // set speed

            digitalWrite(DIR_2, HIGH); // set direction
            analogWrite(SPEED_2, data[1]); // set speed

            break;
          }
          case 4: {
            analogWrite(SPEED_1, 0); // set speed
            analogWrite(SPEED_2, 0); // set speed

            break;
          }
        }
    }
}
```
