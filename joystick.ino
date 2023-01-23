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
    radio.setChannel(5);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate     (RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel      (RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe (0x1234567890LL);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
    
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
    radio.write(&data, sizeof(data));                     // указывая сколько байт массива мы хотим отправить. с проверкой их доставки: if( radio.write(&data, sizeof(data)) ){ приняты;}else{не приняты приёмником;}

    // Serial.print(direction);
    // Serial.println(speed);
    //delay(100);
}
