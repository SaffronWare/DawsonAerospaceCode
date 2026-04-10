#include "sensor_data.h"

mpu_data_collector::mpu_data_collector()
{
    // for now empty constructor
}

void mpu_data_collector::init()
{
    // begins communicate with the mpu6050 through I2C
    // 0x68 is the mpu's adress (TO BE TESTED!)
    if (!mpu.begin(0x68)) {
        while (1) delay(10);
    }

    // The bigger range -> less precise.
    // less range -> more precise but limited values
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void mpu_data_collector::upd_data()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    accx = a.acceleration.x;
    accy = a.acceleration.y;
    accz = a.acceleration.z;

    gyrox = g.gyro.x;
    gyroy = g.gyro.y;
    gyroz = g.gyro.z;
}

float mpu_data_collector::gx()
{
    return gyrox;
}

float mpu_data_collector::gy()
{
    return gyroy;
}

float mpu_data_collector::gz()
{
    return gyroz;
}

float mpu_data_collector::ax()
{
    return accx;
}

float mpu_data_collector::ay()
{
    return accy;
}

float mpu_data_collector::az()
{
    return accz;
}