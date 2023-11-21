# bots

![Slice 3](https://github.com/m112521/bots/assets/85460283/3c20fe82-31df-4fae-954a-a83cff978d09)



https://user-images.githubusercontent.com/85460283/211533535-494cbebc-9428-451c-97e2-594b010e6bee.mp4


## Bot_v3: Painter 

![20231120_194655](https://github.com/m112521/bots/assets/85460283/618bc435-7c0e-4bab-8598-3278408f607e)


## Assembly instructions

![Frame 17](https://github.com/m112521/bots/assets/85460283/13260ce5-ef3d-45a2-9be6-15b908426989)

![Frame 18](https://github.com/m112521/bots/assets/85460283/a00f8f69-ab40-48a2-9c14-609eccd7cc07)


## Code

1. IRremote lib install.
2. Find buttons 
``` c++
#include <IRremote.hpp>
#define IR_RECEIVE_PIN 0

void setup(){
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN);
}

void loop(){
   if (IrReceiver.decode()) {
      IrReceiver.resume(); // Enable receiving of the next value
      //Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      Serial.println(IrReceiver.decodedIRData.command);
  }
}

```

3. Map:
- '+' - 21
- '-' - 7
- 'Play/Pause' - 67
- 'CH+' - 71
- 'CH-' - 69

``` c++
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 0
#define IR_BUTTON_PLUS 21
#define IR_BUTTON_MINUS 7
#define IR_BUTTON_CH_PLUS 71
#define IR_BUTTON_CH_MINUS 69
#define IR_BUTTON_PLAY_PAUSE 67

#define SPEED_1      5 
#define DIR_1        4
 
#define SPEED_2      6
#define DIR_2        7

void setup(){
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN);

  for (int i = 4; i < 8; i++) {     
    pinMode(i, OUTPUT);
  }
}

void loop(){
   if (IrReceiver.decode()) {
      IrReceiver.resume(); // Enable receiving of the next value
      int command = IrReceiver.decodedIRData.command;
      
      switch (command) {
        case IR_BUTTON_PLUS: {
          digitalWrite(DIR_1, LOW); // set direction
          analogWrite(SPEED_1, 255); // set speed

          digitalWrite(DIR_2, LOW); // set direction
          analogWrite(SPEED_2, 255); // set speed

          break;
        }
        case IR_BUTTON_MINUS: {
          digitalWrite(DIR_1, HIGH); // set direction
          analogWrite(SPEED_1, 255); // set speed

          digitalWrite(DIR_2, HIGH); // set direction
          analogWrite(SPEED_2, 255); // set speed

          break;
        }
        case IR_BUTTON_CH_PLUS: { // stop mototrs
          digitalWrite(DIR_1, HIGH); // set direction
          analogWrite(SPEED_1, 255); // set speed

          digitalWrite(DIR_2, LOW); // set direction
          analogWrite(SPEED_2, 255); // set speed

          break;
        }
        case IR_BUTTON_CH_MINUS: { 
          digitalWrite(DIR_1, LOW); // set direction
          analogWrite(SPEED_1, 255); // set speed

          digitalWrite(DIR_2, HIGH); // set direction
          analogWrite(SPEED_2, 255); // set speed
          
          break;
        }
        case IR_BUTTON_PLAY_PAUSE: { // stop mototrs
          analogWrite(SPEED_1, 0); 
          analogWrite(SPEED_2, 0);  
          break;
        }
      }
  }
}
```
