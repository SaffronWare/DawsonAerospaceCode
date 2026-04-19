// code written by man... -TTT (triple t aka tung tung tung sahur)
// install mpu6050 library
// IMPORTANT NOTE FOR FUTURE LIBRARY FUCKING USEERS:
// Please go into adafruit_mpu6050.h and do #define MPU6050_DEVICE_ID 0x70 cus our chip is a bootleg goy
#include <Servo.h>
#include <Wire.h>

Aryan_Code_Airplane_Brain.ino
9 KB

// code written by man... -TTT (triple t aka tung tung tung sahur)
// install mpu6050 library
// IMPORTANT NOTE FOR FUTURE LIBRARY FUCKING USEERS:
// Please go into adafruit_mpu6050.h and do #define MPU6050_DEVICE_ID 0x70 cus our chip is a bootleg goy
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// IMPORTANT WARNING. ALL SIGNS AND OFFSETS AND TUNING PARAMETERS SHOULD BE **PHYSICALLY**
// TESTED FOR ANY NEW PLANE OR AFTER ANY SIGNIFICANT HARDWARE MODIFICATIONS BEFORE FLIGHT.

// UNITS NOTICE
// angles are in radians for now... unless a d is in front of var name
// dt is in seconds whereas time management is done in microseconds.


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

// For filtering gyro drift and accelerometer noise
const float filter_coeficient = 1.0f; // higher the more gyro has control, less accelerometer has control

// Essentially damps the correction to not overshoot due to angular momentum
const float correction_damping = 1.0f; // FINE TUNE EXPERIMENTALLY PLEASE!
const float servo_correction_factor = 3.0f; // FINE TUNEEEE

// preventsservos fromfucking exploding everywhere
const float limit_roll_correction = 45.0f * PI / 180.0f;
const float limit_pitch_correction = 45.0f * PI / 180.0f;
const float limit_yaw_correction = 45.0f * PI / 180.0f;

// PINSSS FINALLYL ATFUCKING 1AM 
const int yaw_input_pin = -1293219301; // i dont fucking know SET THIS TMW!
const int roll_input_pin = -102938120392813;
const int pitch_input_pin= 12123092813;
const int throttle_input_pin = 12938120938210938;

const int rudder_pin = 5; // proper
const int elevator_pin = 1209382309;
const int aileron_pin =123213;

const int RUDDER0 = 124;
const int ELEVATOR0 = 124;
const int AILERON0 = 124;

Servo Rudder;
Servo Elevator;
Servo Aileron;

// If were in debugging we wont actually update servo data.
const bool DEBUGGING = true;
const bool RUDDER_DEBUGGING =true;



void setup()
{   
    // make sure output is being read from this channel
    // this begins arduino data transfer with port

    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }
    Serial.println("----------------------------------------------");
    Serial.println("INITIALIZED SERIAL COMMUNICATION WITH ARDUINO");
    Serial.println("----------------------------------------------");
    Serial.print("\nStarting mpu-arduino communication (-1-)...");

    Wire.begin();
    Serial.println("Wire began!");
    //delay(2000);

    // Starting communication with MPU
    bool started_mpu = false;
    for (int i = 1; i <= 50; i++)
    {
        if (MPU_Sensor.begin(MPU_address))
        {
            Serial.println("Mpu working...");
            started_mpu = true;
            break;
        }
        Serial.print("(-1) FAILED to start MPU, retrying... (x");
        Serial.print(i);
        Serial.println(")");
        delay(50);
    }
    if (!started_mpu)
    {
        Serial.println("\r (-1) FAILED TO START MPU AFTER 50 TRIES. TERMINATING...");

        // paused program forever
        while (true) {delay(10);}
    }
    Serial.println("\r(-1-) [Success] MPU started!              ");

    Serial.println("Setting accelerometer & gyro range (-2-)...");
    MPU_Sensor.setAccelerometerRange(ACC_RANGE);
    MPU_Sensor.setGyroRange(GYRO_RANGE);
    Serial.println("\r(-2-) [Success] Set accelerometer & gyro range!   ");

    Serial.println("Setting filter bandwidth (-3-)...");
    MPU_Sensor.setFilterBandwidth(MPU_FILTER_BANDWIDTH);
    Serial.println("\r(-3-) [Success] Set filter bandwidth!");

    Serial.println("Attaching rudder (-4-)");
    Rudder.attach(rudder_pin);
    Serial.println("(-4-) [Success] rudder attached");

    Serial.println("----------SETUP COMPLETE[3/3]---------------");
    delay(100);
    Serial.println("(-4) MAIN PROGRAM STARTING...");


}

// ---------------------- LOOP VARIABLES --------------------
// angle data
float roll = 0; float pitch = 0; float yaw = 0;
float dRoll = 0; float dPitch = 0; float dYaw = 0;

// controller angle data
float desiredRoll = 0; float desiredPitch = 0; float strengthYaw = 0;

// servo angle for correction
float servoRollCorrection; float servoPitchCorrection; float servoYawCorrection;

float rudderAngle = 0; float elevatorAngle = 0; float aileronAngle = 0;

// sensor data
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
        bool got_read = MPU_Sensor.getEvent(&a, &g, &temp);


        if (!got_read)
        {
          Serial.println("croski ur reading aint workin");
        }

        // storing sensor data into our variables
        accX = a.acceleration.x;
        accY = a.acceleration.y;
        accZ = a.acceleration.z;
        gAccX = g.gyro.x;
        gAccY = g.gyro.y;
        gAccZ = g.gyro.z;
        // IMPORTANTTTTT!!!!!!!! (not done yet!)
        // Test to make sure of the follow
        // A) roll is associated with gyro.x, pitch with gyro.y and yaw with gyro.z
        // B) roll increases as plane rotates clockwise from forward direction
        // C) yaw increases when turning towards right wing
        // D) pitch increases as nose goes up 
        // COMPLEMENTARY FILTERING! 
        roll = filter_coeficient * (roll + gAccX * dt) + (1-filter_coeficient) * atan2(-accX, sqrt(accY*accY + accZ*accZ));
        pitch = filter_coeficient * (pitch + gAccY * dt) + (1-filter_coeficient) * atan2(accY, accZ);
        yaw += gAccZ * dt; // unfortunately no possible filtering for yaw
        
        dRoll = roll * 180.0f / PI; dPitch = pitch * 180.0f / PI; dYaw = yaw * 180.0f/PI;


        
        
        // IGNORE ABOVE

        // choosing angle for servo to take to correct the over/under shooting of desired angle
        // For yaw ignore as gyro drift is not corrected, we will let user fully control yaw
                //servoYawCorrection = servo_correction_factor * (desiredYaw - yaw) 
                    // - correction_damping * gAccZ;
        servoPitchCorrection =  servo_correction_factor * (desiredPitch - pitch)
            - correction_damping * gAccY;
        servoRollCorrection =  servo_correction_factor * (desiredRoll - roll)
            - correction_damping * gAccX;

        // for yaw we just turn based on strength of yaw input from controller
        // Essentially giving user full control over yaw
        servoYawCorrection = servo_correction_factor * strengthYaw;
        
        // constraiing to maximum correction angle
        servoPitchCorrection = constrain(servoPitchCorrection, -limit_pitch_correction, limit_pitch_correction);
        servoRollCorrection = constrain(servoRollCorrection, -limit_roll_correction, limit_roll_correction);
        servoYawCorrection = constrain(servoYawCorrection, -limit_yaw_correction, limit_yaw_correction);


        rudderAngle = servoYawCorrection * 180 / PI;
        aileronAngle = servoRollCorrection * 180/PI;
        elevatorAngle = servoPitchCorrection * 180/PI;

        Rudder.write(rudderAngle + RUDDER0);
        Aileron.write(aileronAngle + AILERON0);
        Elevator.write(elevatorAngle + ELEVATOR0);

        if (RUDDER_DEBUGGING)
        {
            Rudder.write(RUDDER0);
        }

        Serial.write(ser)

         // IGNORE SIMPLY FOR DATA TRANSFER
        Serial.print("DBG");
        Serial.print(" ");
        Serial.print(dRoll); //  X is properly roll & forward direction-
        Serial.print(" ");
        Serial.print(dPitch);
        Serial.print(" ");
        Serial.print(dYaw);
        Serial.print(" ");
        Serial.print(rudderAngle);
        Serial.print(" ");
        Serial.print(elevatorAngle);
        Serial.print(" ");
        Serial.print(aileronAngle);
        Serial.print(" ")
        Serial.println("");

        
        
        



    }
}
