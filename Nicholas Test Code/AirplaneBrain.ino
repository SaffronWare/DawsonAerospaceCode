#include <Servo.h>

const int InputPin1 = 2; 
const int InputPin2 = 3; 
//Input pins

Servo Rudder;
Servo Elevator;

void setup() {
  Serial.begin(9600);

  pinMode(InputPin1, INPUT);
  pinMode(InputPin2, INPUT);
    
  Rudder.attach(5); 
  Elevator.attach(6); 
}

void loop() {
  int Input1 = pulseIn(InputPin1, HIGH);
  
  int angle2 = map(Input1, 1000, 2000, 83, 180);
  Rudder.write(angle1);


  int Input2 = pulseIn(InputPin2, HIGH);
  
  int angle3 = map(Input2, 1000, 2000, 83, 180);
  Elevator.write(angle2);


}
