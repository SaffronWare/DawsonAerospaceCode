#include "servo.h"


ServoObject::ServoObject()
{

}

ServoObject::setup(int _channel_pin, int _servo_pin)
{
    servo_pin = _servo_pin; channel_pin = _channel_pin;
    
    pinMode(channel_pin, INPUT);
  
    servo.attach(servo_pin);
}

void ServoObject::update()
{
    int ch1 = pulseIn(CH1_PIN, HIGH, 25000);
    int ch1_mapped = map(ch1, 1000, 2000, -100, 100);

    if (ch1 != 0) {  // 0 means signal lost — don't move servo
        int servoAngle = map(ch1, 1000, 2000, 0, 180);
        servo1.write(servoAngle);
    }
}