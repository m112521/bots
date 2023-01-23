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
  radio.setChannel(5);                                  // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
  radio.setDataRate     (RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel      (RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
  radio.openReadingPipe (1, 0x1234567890LL);            // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
  radio.startListening  ();                             // Включаем приемник, начинаем прослушивать открытую трубу
//  radio.stopListening   ();                             // Выключаем приёмник, если потребуется передать данные

  // motor pins
  for (int i = 4; i < 8; i++) {     
    pinMode(i, OUTPUT);
  }


  Serial.println("test");
}

void loop(){
    if(radio.available()){                                // Если в буфере имеются принятые данные
        radio.read(&data, sizeof(data));                  // Читаем данные в массив data и указываем сколько байт читать

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