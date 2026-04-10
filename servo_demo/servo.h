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


