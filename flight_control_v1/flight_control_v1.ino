// code written by man...
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// --------------------- CONSTANTS & SETUP -------------------------

// text 0x69 if running into issues
const uint8_t MPU_address = 0x68;
Adafruit_MPU6050 MPU_Sensor;

// how often we read from sensor/controller
const unsigned int SENSOR_READ_RATE = 60;
const unsigned long SENSOR_READ_DELAY = 1000000UL/SENSOR_READ_RATE; // X1_000_000 due to s->micro(s)

// higher range -> less accuracy (choose wisely...)
const auto ACC_RANGE = MPU6050_RANGE_8_G;
const auto GYRO_RANGE = MPU6050_RANGE_500_DEG;

// higher -> more noisy but responsive. Lower -> less noisy less responsive
// I put it to higher than the default of 21Hz to prevent over filtering \ .
// since we are running a filter on it later on. 
const auto MPU_FILTER_BANDWIDTH = MPU6050_BAND_44_HZ;

\\ For filtering gyro drift and accelerometer noise
const float filter_coeficient = 0.98f; \\ higher the more gyro has control, less accelerometer has control


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
    Serial.println("(-4) MAIN PROGRAM STARTING...");
}

// ---------------------- LOOP VARIABLES --------------------
float roll = 0; float pitch = 0; float yaw = 0;

float accX, accY, accZ;
float gAccX, gAccY, gAccZ;

// For accurate timestep computation
unsigned long last_time = 0;
float dt = 0.0f; // important, tells how much time since last iteration of code update
void loop()
{
    // getting new accelerometer and gyro data
    unsigned long now_time = micros();
    // if enough time has passed, run code.
    if (now_time - last_time >= SENSOR_READ_DELAY)
    {
        dt = (float)(now_time - last_time) * 1e-6f;
        last_time = now_time;
        sensors_event_t a, g, temp; // temp stands for temperature and not temporary lol (victim of this)
        MPU_Sensor.getEvent(&a, &g, &temp);

        // storing sensor data into our variables
        accX = a.acceleration.x;
        accY = a.acceleration.y;
        accZ = a.acceleration.z;
        gAccX = g.gyro.x;
        gAccY = g.gyro.y;
        gAccZ = g.gyro.z;

        // Test to make sure of the follow
        // A) roll is associated with gyro.x, pitch with gyro.y and yaw with gyro.z
        // B) roll increases as plane rotates clockwise from forward direction
        // C) yaw increases when turning towards right wing
        // D) pitch increases as nose goes up  
        roll = filter_coeficient * (roll + gAccX * dt) + (1-filter_coeficient) * atan2(-accX,accZ);
        pitch = filter_coeficient * (pitch + gAccY * dt) + (1-filter_coeficient) * atan2(accY, accZ);
        yaw += gAccZ * dt; // unfortunately no possible filtering for yaw

    }
}