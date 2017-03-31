/*  This code is a simplified example that prints out all readings from the IMU. 
 *  Three types of sensors - accelerometers, gyroscopes, and a magnetometer - 
 *  provide 3-axis acceleration data, 3-axis rotational velocity data, 
 *  and compass heading data.
 */  

/* Include libraries required for IMU sensor */
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>

LSM9DS1 imu; // Name IMU object "imu"

/* Declare Variables used by IMU. Float means the variable can have decimal places */ 
float roll;
float pitch;
float heading;
float AX;
float AY;
float AZ;
float MX;
float MY;
float MZ;

#define LSM9DS1_M  0x1E
#define LSM9DS1_AG  0x6B
#define PRINT_CALCULATED // This line is active - the more-useful calculated values will print - see below
// #define PRINT_RAW // This line is not active (commented out) 
#define PRINT_SPEED 250
#define DECLINATION -12 // Irvine, CA declination 

void setup() {
  Serial.begin(9600);
  /* Required settings for IMU communication */
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  if (!imu.begin()) // This line means "If the IMU does NOT(!) begin, print the following message..."
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                  "work for an out of the box LSM9DS1 " \
                  "Breakout, but may need to be modified " \
                  "if the board jumpers are.");
    while (1); // wait forever 
  }

}

void loop() {
  //print gyro values
  imu.readGyro();
  Serial.print("G: ");
  #ifdef PRINT_CALCULATED // The values calcuated are in units [degrees / second]
    Serial.print(imu.calcGyro(imu.gx), 2);
    Serial.print(", ");
    Serial.print(imu.calcGyro(imu.gy), 2);
    Serial.print(", ");
    Serial.print(imu.calcGyro(imu.gz), 2);
    Serial.println(" deg/s");
  #elif defined PRINT_RAW
    Serial.print(imu.gx);
    Serial.print(", ");
    Serial.print(imu.gy);
    Serial.print(", ");
    Serial.println(imu.gz);
  #endif

  //print accel values
  imu.readAccel();
    Serial.print("A: ");
  #ifdef PRINT_CALCULATED // The values calculated are in units [g's] where 1g = 9.81 m/s^2
    Serial.print(imu.calcAccel(imu.ax), 2);
    Serial.print(", ");
    Serial.print(imu.calcAccel(imu.ay), 2);
    Serial.print(", ");
    Serial.print(imu.calcAccel(imu.az), 2);
    Serial.println(" g");
  #elif defined PRINT_RAW 
    Serial.print(imu.ax);
    Serial.print(", ");
    Serial.print(imu.ay);
    Serial.print(", ");
    Serial.println(imu.az);
  #endif

  //print mag values
  imu.readMag();
    Serial.print("M: ");
  #ifdef PRINT_CALCULATED // The values calcuated are in units [gauss]
    Serial.print(imu.calcMag(imu.mx), 2);
    Serial.print(", ");
    Serial.print(imu.calcMag(imu.my), 2);
    Serial.print(", ");
    Serial.print(imu.calcMag(imu.mz), 2);
    Serial.println(" gauss");
  #elif defined PRINT_RAW
    Serial.print(imu.mx);
    Serial.print(", ");
    Serial.print(imu.my);
    Serial.print(", ");
    Serial.println(imu.mz);
  #endif

  //calculate attitude values
  AX = imu.ax;
  AY = imu.ay;
  AZ = imu.az;
  MX = imu.mx;
  MY = imu.my;
  MZ = imu.mz;
  
  roll = atan2(AY, AZ); 
  roll *= 180.0 / PI; // Angle of roll [degrees]
  
  pitch = atan2(-AX, sqrt(AY * AY + AZ * AZ));
  pitch *= 180.0 / PI; // Angle of pitch [degrees]

  heading = atan2(MY, MX);
  
  heading -= DECLINATION * PI / 180;
  heading *= 180.0 / PI; // Angle of heading [degrees] where 0 deg = 360 deg = North
  
  Serial.print(roll,2);
  Serial.print(", ");
  Serial.print(pitch,2);
  Serial.print(", ");
  Serial.println(heading,2);
  Serial.println(" ");
  delay(500);

}
