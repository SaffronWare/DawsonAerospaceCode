#pragma once 


class ServoObject
{
private:
    int channel_pin; // pin to read controller data
    int servo_pin; // pin to output servo 
    Servo servo; // our servo
public:
    ServoObject();
    void setup(int _channel_pin, int _servo_pin);
    void update();
}

void setup() {
  Serial.begin(115200);
  pinMode(CH1_PIN, INPUT);
  
  servo1.attach(servo1_pin);
}

void loop() {
  int ch1 = pulseIn(CH1_PIN, HIGH, 25000);
  int ch1_mapped = map(ch1, 1000, 2000, -100, 100);

  if (ch1 != 0) {  // 0 means signal lost — don't move servo
    int servoAngle = map(ch1, 1000, 2000, 0, 180);
    servo1.write(servoAngle);
  }
  Serial.print("CH1: ");
  Serial.print(ch1);
  Serial.print("us (");
  Serial.print(ch1_mapped);
  Serial.println("%)");
  
}