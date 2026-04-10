#include "sensor_data.h"


mpu_data_collector sensor;

void setup()
{
    
  Serial.begin(115200);
  while (!Serial) delay(10);

  Wire.begin();
  Wire.setClock(100000);
}

void loop()
{
  sensor.upd_data();

  Serial.print("Acc: ");
  Serial.print(sensor.accx);
  Serial.print(", ");
  Serial.print(sensor.accy);
  Serial.print(", ");
  Serial.println(sensor.accz);

  Serial.print("Gyro: ");
  Serial.print(sensor.gyrox);
  Serial.print(", ");
  Serial.print(sensor.gyroy);
  Serial.print(", ");
  Serial.println(sensor.gyroz);

  delay(100);

}