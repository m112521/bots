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
  radio.setChannel(5);                                  // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
  radio.setDataRate     (RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel      (RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
  radio.openReadingPipe (1, 0x1234567890LL);            // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
  radio.startListening  ();                             // Включаем приемник, начинаем прослушивать открытую трубу
  //  radio.stopListening   ();                         // Выключаем приёмник, если потребуется передать данные

  // motor pins
  for (int i = 4; i < 8; i++) {     
    pinMode(i, OUTPUT);
  }

  matrix.begin();
  rotServo.attach(2);
}

void loop(){
    if(radio.available()){                                // Если в буфере имеются принятые данные
        radio.read(&data, sizeof(data));                  // Читаем данные в массив data и указываем сколько байт читать

        int btnState = data[2];
        int tmblrState = data[3];

        int ptmrVal = data[4];
        servoAngle = map(ptmrVal, 0, 1023, 0, 180); 
        rotServo.write(servoAngle); 
        //Serial.println(ptmrVal);

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
            analogWrite(SPEED_1, ptmrVal); // set speed

            digitalWrite(DIR_2, HIGH); 
            analogWrite(SPEED_2, ptmrVal); 

            break;
          }
          case 1: {
            digitalWrite(DIR_1, LOW);
            analogWrite(SPEED_1, ptmrVal); 

            digitalWrite(DIR_2, LOW);
            analogWrite(SPEED_2, ptmrVal); 

            break;
          }
          case 2: {
            digitalWrite(DIR_1, HIGH); 
            analogWrite(SPEED_1, ptmrVal); 

            digitalWrite(DIR_2, LOW);
            analogWrite(SPEED_2, ptmrVal); 

            break;
          }
          case 3: {
            digitalWrite(DIR_1, LOW); 
            analogWrite(SPEED_1, ptmrVal); 

            digitalWrite(DIR_2, HIGH); 
            analogWrite(SPEED_2, ptmrVal); 

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
