# bots

Arduino based remotely controlled robot with NRF24 radio modules. PCB joystick coming soon.

![IMG_20230202_183918](https://user-images.githubusercontent.com/85460283/216372205-ae602cb4-aa78-47df-a376-f1b1e74b6634.jpg)
![IMG_20230202_183834](https://user-images.githubusercontent.com/85460283/216372172-7d129b30-7f78-42f2-a136-4f96c8a83dc3.jpg)



https://user-images.githubusercontent.com/85460283/216372195-9f39fe78-ddbf-4d21-ac38-ee1651fc902b.mp4



TBD:
- [x] on/off switch
- [x] servo launcher
- [x] led strip

BOM:
- NRF,
- NRF+,
- x2 5v to 3.3 v,
- Motor Shield,
- Arduino Leonardo/Amperka Iskra Neo,
- x2 18650,
- x2 DC motor,
- joystick,
- x2 switch,
- RGB LED matrix (4x4),
- potentiometer,
- button,
- powerbank,
- servo 180.


1. RF24 by TMRh20 lib install (through Arduino IDE library manager) and NeoPixel by Adafruit.

![lib install](https://user-images.githubusercontent.com/85460283/214080122-c7909fbe-ddb3-4348-938a-55308347660f.png)


2. Joystick code: 

``` c++
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);
int data[5];  

const int xPin = A1;
const int yPin = A0;
const int buttonPin = 4;
const int ptmrPin = A2;
const int tumblrPin = 0;

int xPosition = 0;
int yPosition = 0;
int buttonState = 0;
int tumblrState = 0;
int ptmrState = 0;

int speed = 0;
int direction = 0;
                                
void setup(){
    radio.begin();
    radio.setChannel(5); // Channel (from 0 to 127), 5 - freq 2,405 GHz (only 1 receiver on a single channel and up to 6 transmitters)
    radio.setDataRate     (RF24_1MBPS); // Speed (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.setPALevel      (RF24_PA_HIGH); // Transmitter power (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe (0x1234567890LL); 
    
    pinMode(xPin, INPUT);
    pinMode(yPin, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(tumblrPin, INPUT);
    pinMode(ptmrPin, INPUT);
}
void loop(){
    xPosition = analogRead(xPin);
    yPosition = analogRead(yPin);
    buttonState = digitalRead(buttonPin);
    tumblrState = digitalRead(tumblrPin);
    ptmrState = analogRead(ptmrPin);
    

    if (xPosition > 520) {
      // go forward
      direction = 3; // 3 - Forward, 2 - backward, 1 - right, 0 - left
      speed = map(xPosition, 520, 1023, 5, 255);
    }
    else if (xPosition < 510) {
      // go backward
      direction = 2;
      speed = map(xPosition, 520, 0, 5, 255);
    }
    else if (yPosition > 520) {
      // go right
      direction = 1; // 0 - Forward, 1 - backward, 2 - right, 3 - left
      speed = 255; //map(xPosition, 520, 1023, 5, 255);
    }
    else if (yPosition < 510) {
      // go left
      direction = 0; // 0 - Forward, 1 - backward, 2 - right, 3 - left
      speed = map(xPosition, 520, 0, 5, 255);
    }
    else {
      direction = 4;
      speed = 0;
    }

    data[0] = direction;
    data[1] = speed;
    data[2] = !buttonState;
    data[3] = tumblrState;
    data[4] = ptmrState;
    radio.write(&data, sizeof(data)); // check delivery: if(radio.write(&data, sizeof(data)))

    Serial.println(ptmrState);
    //Serial.println(speed);
    delay(100);
}
```

3. Bot code:

``` c++
#include <SPI.h>                                         
#include <nRF24L01.h>                                    
#include <RF24.h>        
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define SPEED_1      5 
#define DIR_1        4
 
#define SPEED_2      6
#define DIR_2        7

#define MATRIX_PIN    0
#define LED_COUNT 16

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_COUNT, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

RF24 radio(8, 9); // nRF24L01+ (CE, CSN)
int data[5]; 

Servo rotServo; 
int servoAngle;

void setup(){
  Serial.begin(9600);
    
  radio.begin();                                        
  radio.setChannel(5);
  radio.setDataRate (RF24_1MBPS);
  radio.setPALevel (RF24_PA_HIGH);
  radio.openReadingPipe (1, 0x1234567890LL);
  radio.startListening ();                            
  //  radio.stopListening ();                         

  // motor pins
  for (int i = 4; i < 8; i++) {     
    pinMode(i, OUTPUT);
  }

  matrix.begin();
  rotServo.attach(2);
}

void loop(){
    if(radio.available()){                                
        radio.read(&data, sizeof(data));

        int btnState = data[2];
        int tmblrState = data[3];

        int ptmrVal = data[4];
        servoAngle = map(ptmrVal, 0, 1023, 0, 180); 
        rotServo.write(servoAngle); 
        Serial.println(ptmrVal);

        if (tmblrState == 1) {    
          if (btnState) {
            colorWipe(matrix.Color(255, 0, 0), 50);
            colorWipe(matrix.Color(0, 255, 0), 50);
            colorWipe(matrix.Color(0, 0, 255), 50);
            colorWipe(matrix.Color(0, 0, 0), 50);
          }   
          else {
            crossBlink(matrix.Color(255, 0, 0), 50);
          }
        }
        else {
          Serial.println("no");
          crossBlink(matrix.Color(0, 0, 0), 50);
        }

        // 0 - Forward, 1 - backward, 2 - right, 3 - left
        switch (data[0]) {
          case 0: {
            digitalWrite(DIR_1, HIGH); // set direction
            analogWrite(SPEED_1, 255); // set speed

            digitalWrite(DIR_2, HIGH); 
            analogWrite(SPEED_2, 255); 

            break;
          }
          case 1: {
            digitalWrite(DIR_1, LOW);
            analogWrite(SPEED_1, data[1]); 

            digitalWrite(DIR_2, LOW);
            analogWrite(SPEED_2, data[1]); 

            break;
          }
          case 2: {
            digitalWrite(DIR_1, HIGH); 
            analogWrite(SPEED_1, data[1]); 

            digitalWrite(DIR_2, LOW);
            analogWrite(SPEED_2, data[1]); 

            break;
          }
          case 3: {
            digitalWrite(DIR_1, LOW); 
            analogWrite(SPEED_1, data[1]); 

            digitalWrite(DIR_2, HIGH); 
            analogWrite(SPEED_2, data[1]); 

            break;
          }
          case 4: {
            analogWrite(SPEED_1, 0); 
            analogWrite(SPEED_2, 0); 

            break;
          }
        }        
    }
}

void crossBlink(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < matrix.numPixels(); i++) {
    if (i % 5 == 0 || i % 3 == 0) { 
      matrix.setPixelColor(i, c);
      matrix.show();
      //delay(wait);
    }
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < matrix.numPixels(); i++) {
    matrix.setPixelColor(i, c);
    matrix.show();
  }
  //delay(wait);
}
```
