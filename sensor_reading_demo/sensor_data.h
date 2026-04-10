#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


// Class we'll use to make collecting gyro and accelrometer data simpler
class mpu_data_collector
{
public:
  mpu_data_collector();

  // Will update the values from the sensor
  void upd_data();

  // Functions to read the data a=accelerometer and g=gyroscope
  float gx(); float gy(); float gz();
  float ax(); float ay(); float az();

  float gyrox, gyroy, gyroz;
  float accx, accy, accz;

private:
  Adafruit_MPU6050 mpu;
};

