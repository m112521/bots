// #include <Arduino.h>
#include <TB6612_ESP32.h>


#define ENCA1 18 // YELLOW
#define ENCA2 19 // WHITE

#define ENCB1 22 // YELLOW
#define ENCB2 23 // WHITE

#define AIN1 13 // ESP32 Pin D13 to TB6612FNG Pin AIN1
#define BIN1 12 // ESP32 Pin D12 to TB6612FNG Pin BIN1
#define AIN2 14 // ESP32 Pin D14 to TB6612FNG Pin AIN2
#define BIN2 27 // ESP32 Pin D27 to TB6612FNG Pin BIN2
#define PWMA 26 // ESP32 Pin D26 to TB6612FNG Pin PWMA
#define PWMB 25 // ESP32 Pin D25 to TB6612FNG Pin PWMB
#define STBY 33 // ESP32 Pin D33 to TB6612FNG Pin STBY


// *** Wiring connections from TB6612FNG Motor Controller to the Motors ***


// Connect the leads from your motors, don't worry if you don't get them correct the first time
// Motor 1 should go to A01 and A02
// Motor 2 should go to B01 and B02
// If the motors are NOT turning in the right direction reverse the pins (for example the A01 and A02) pins
// If the left and right motors are reversed then swap the A01 and A02 wires to the B01 and B02 pins 


// *** Wiring connections from the Battery Pack to the ESP32 and TB6612FNG Motor Controller ***


// A 4 Cell battery pack was used to supply 6 Volts of DC current to the robot and to the ESP32
// A wire splitter was used to connect the Positive (+) side of the Battery pack to the VIN pin of the ESP32,
// and to the VM pin of the TB6612FNG Motor Controller.
// Another wire splitter was used to connect the Negative (-) side of the Battery pack to the GND pin of the ESP32,
// and to the GND pin of the TB6612FNG Motor Controller


// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 1);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

volatile int posiA1 = 0;
volatile int posiB1 = 0;
// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.

long prevT = 0;
float eprev = 0;
float eintegral = 0;


void IRAM_ATTR ISRA1(){
  int b = digitalRead(ENCA2);
  if(b > 0){
    posiA1++;
  }
  else{
    posiA1--;
  }
}

void IRAM_ATTR ISRB1(){
  int b = digitalRead(ENCB2);
  if(b > 0){
    posiB1++;
  }
  else{
    posiB1--;
  }
}


void testRun() {
  forward(motor1, motor2, 255); 
  delay(2000);
  forward(motor1, motor2, 0);
  delay(2000);
  Serial.println(String(posiA1) + " " + String(posiB1));
  
  back(motor1, motor2, -255);
  delay(2000);
  back(motor1, motor2, 0);
  delay(2000);  
  Serial.println(String(posiA1) + " " + String(posiB1));
}

void setup()
{
  Serial.begin(115200);
  
  pinMode(ENCA1,INPUT);
  pinMode(ENCA2,INPUT);
  pinMode(ENCB1,INPUT);
  pinMode(ENCB2,INPUT);

  attachInterrupt(ENCA1, ISRA1, RISING);
  attachInterrupt(ENCB1, ISRB1, RISING);

  delay(2000);
  testRun();

  Serial.println("target pos");
}

vois calPID() {
  // set target position
  //int target = 1200;
  int target = 325*sin(prevT/1e6);

  // PID constants
  float kp = 1;
  float kd = 0.025;
  float ki = 0.0;

  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  int pos = 0;
  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos = posi;
  //}

  // error
  int e = pos - target;

  // derivative
  float dedt = (e-eprev)/(deltaT);

  // integral
  eintegral = eintegral + e*deltaT;

  // control signal
  float u = kp*e + kd*dedt + ki*eintegral;

  // motor power
  float pwr = fabs(u);
  if( pwr > 255 ){
    pwr = 255;
  }

  // motor direction
  int dir = 1;
  if(u<0){
    dir = -1;
  }

  // signal the motor
  setMotor(dir,pwr,PWM,IN1,IN2);


  // store previous error
  eprev = e;

  Serial.print(target);
  Serial.print(" ");
  Serial.print(pos);
  Serial.println();
}

void loop()
{
  //forward(motor1, motor2, 255);
  
    // delay(2000); // Short delay to catch your robot

    // motor1.drive(255, 2000);       // Turn Motor 1 for 2 seconds at full speed
    // //motor1.drive(-255,2000);
    // motor1.brake();
    // delay(2000);

    // motor2.drive(255, 2000);        // Turn Motor 2 for 2 seconds at full speed
    // //motor2.drive(-255,2000);
    // motor2.brake();
    // delay(2000);

    // forward(motor1, motor2, 255);        // Forward Motor 1 and Motor 2 for 1 seconds at full speed
    // delay(1000);

    // // Brake
    // brake(motor1, motor2);     // Stop Motor 1 and Motor 2 for 2 seconds 
    // delay(2000);     

    // back(motor1, motor2, -255);         // Reverse Motor 1 and Motor 2 for 1 seconds at full speed
    // delay(1000);

    // Brake again
    // brake(motor1, motor2);       // Stop Motor 1 and Motor 2 for 2 seconds 
    // delay(2000);  

    // left(motor1, motor2, 255);
    // delay(2000);
    // brake(motor1, motor2);
    // delay(1000); 
    // right(motor1, motor2, 255);
    // delay(2000);

    // brake(motor1, motor2);       // Stop Motor 1 and Motor 2 for 2 seconds 
    // delay(2000); 

    //Serial.println(String(posiA1) + " " + String(posiB1));
}
