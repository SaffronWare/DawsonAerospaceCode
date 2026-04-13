#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Kalman.h>  // by Kristian Lauszus

Kalman kalmanX, kalmanY;
float angleX, angleY;
uint32_t timer;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int MPU_address = 0x68;
float gyroX_bias = 0;
float gyroY_bias = 0;
float gyroZ_bias = 0;
unsigned long previousTime = 0;
float gyroX = 0;
float gyroY = 0;
float gyroZ = 0;

void setup() {
 if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
   for(;;);
 }
   display.clearDisplay();
  Wire.begin();
  Wire.beginTransmission(MPU_address);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
  for (int i = 0; i < 500; i++){
    Wire.beginTransmission(MPU_address);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_address, 6, true);
    if(Wire.available() >= 6) {
      gyroX_bias += (int16_t)(Wire.read() << 8 | Wire.read());
      gyroY_bias += (int16_t)(Wire.read() << 8 | Wire.read());
      gyroZ_bias += (int16_t)(Wire.read() << 8 | Wire.read());
    }
    delay(2);
  }
  
  gyroX_bias /= 500.0;
  gyroY_bias /= 500.0;
  gyroZ_bias /= 500.0;
  previousTime = millis();

  float roll  = atan2(ay, az) * RAD_TO_DEG;
  float pitch = atan2(-ax, az) * RAD_TO_DEG;
  kalmanX.setAngle(roll);
  kalmanY.setAngle(pitch);
  timer = micros();
}
float pitch = 0;
float roll = 0;
float alpha = 0.96;
float temp = 0;
void loop() {
  Wire.beginTransmission(MPU_address);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_address, 14, true);
  
  
  if (Wire.available() >= 14) {

    unsigned long currentTime = millis();
    float dt = (currentTime - previousTime) / 1000.0; 
    previousTime = currentTime;
 
    //Accelerometer
    int16_t ax = Wire.read() << 8 | Wire.read();
    int16_t ay = Wire.read() << 8 | Wire.read();
    int16_t az = Wire.read() << 8 | Wire.read();
 
    //Temperature
    int16_t rawTemp = Wire.read() << 8 | Wire.read();
    temp = (rawTemp / 340.0) + 36.53; // Standard conversion to Celsius
    //Gyroscope
    int16_t gx = Wire.read() << 8 | Wire.read();
    int16_t gy = Wire.read() << 8 | Wire.read();
    int16_t gz = Wire.read() << 8 | Wire.read();
 
    gyroX += ((gx - gyroX_bias)/131)*dt;
    gyroY += ((gy - gyroY_bias)/131)*dt;
    gyroZ += ((gz - gyroZ_bias)/131)*dt;
 
    float gyroXrate = (gx - gyroX_bias) / 131.0;
    float gyroYrate = (gy - gyroY_bias) / 131.0;
    
    float accpitch = atan2(ay, az) * 180.0 / PI;
    float accroll = atan2(-ax, az) * 180.0 / PI;
 
    pitch = kalmanX.getAngle(roll,  gyroXrate, dt);
    roll = kalmanY.getAngle(pitch, gyroYrate, dt);

    //pitch = alpha*(pitch+gyroYrate*dt) + (1-alpha) * accpitch;
    //roll = alpha*(roll+gyroXrate*dt) + (1-alpha) * accroll;
  }
  float roll  = atan2(ay, az) * RAD_TO_DEG;
  float pitch = atan2(-ax, az) * RAD_TO_DEG;
  kalmanX.setAngle(roll);
  kalmanY.setAngle(pitch);
  timer = micros();

  float dt = (micros() - timer) / 1000000.0;
  timer = micros();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(pitch);
  display.println(roll);
  display.println(temp);
  display.display();
  delay(5);
}

