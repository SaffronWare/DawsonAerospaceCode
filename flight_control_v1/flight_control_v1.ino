// code written by man...
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// text 0x69 if running into issues
const int MPU_address = 0x68;
Adafruit_MPU6050 MPU_Sensor;

// how often we read from sensor/controller
const int SENSOR_READ_RATE = 60;
const float SENSOR_READ_DELAY = 1000.0f/(float)SENSOR_READ_RATE; // X1000 due to s->ms


// higher range -> less accuracy (choose wisely...)
const auto ACC_RANGE = MPU6050_RANGE_8_G;
const auto GYRO_RANGE = MPU6050_RANGE_500_DEG;

// higher -> more noisy but responsive. Lower -> less noisy less responsive
// I put it to higher than the default of 21Hz to prevent over filtering \ .
// since we are running a filter on it later on. 
const auto MPU_FILTER_BANDWIDTH = MPU6050_BAND_44_HZ;

float accX, accY, accZ;
float gAccX, gAccY, gAccZ;

void setup()
{   
    // make sure output is being read from this channel
    // this begins arduino data transfer with port

    Serial.begin(19200);
    while (!Serial)
    {
        delay(100);
    }
    Serial.println("----------------------------------------------");
    Serial.println("INITIALIZED SERIAL COMMUNICATION WITH ARDUINO");
    Serial.println("----------------------------------------------");
    Serial.print("\nStarting mpu-arduino communication (-1-)...");

    // Starting communication with MPU
    bool started_mpu = false;
    for (int i = 1; i <= 50; i++)
    {
        if (MPU_Sensor.begin(MPU_address))
        {
            started_mpu = true;
            break;
        }
        Serial.print("\r(-1) FAILED to start MPU, retrying... (x");
        Serial.print(i);
        Serial.print(")");
        delay(50);
    }
    if (!started_mpu)
    {
        Serial.print("\r (-1) FAILED TO START MPU AFTER 50 TRIES. TERMINATING...");

        // paused program forever
        while (true) {delay(10);}
    }
    Serial.println("\r(-1-) [Success] MPU started!              ");

    Serial.print("Setting accelerometer & gyro range (-2-)...");
    MPU_Sensor.setAccelerometerRange(ACC_RANGE);
    MPU_Sensor.setGyroRange(GYRO_RANGE);
    Serial.println("\r(-2-) [Success] Set accelerometer & gyro range!   ");

    Serial.print("Setting filter bandwidth (-3-)...");
    MPU_Sensor.setFilterBandwidth(MPU_FILTER_BANDWIDTH);
    Serial.println("\r(-3-) [Success] Set filter bandwidth!                       ");

    Serial.println("----------SETUP COMPLETE[3/3]---------------");
    delay(100);
}

void loop()
{

}