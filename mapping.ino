#include "Ultrasonic.h"
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>

//LSM9DS1 imu;

Ultrasonic ult1(10,9); //front
Ultrasonic ult2(13,12); //left
Ultrasonic ult3(3,4); //down

/* Declare Variables used by IMU. Float means the variable can have decimal places */ 
float roll;
float pitch;
float heading;
float initdrxn; //assumes initial direction is north (0 = 360 = North)
// change in accordance to actual course bearings, in degrees
float AX;
float AY;
float AZ;
float MX;
float MY;
float MZ;

int quadrant, color;
boolean stable = false;
char a = 'E';

#define LSM9DS1_M  0x1E
#define LSM9DS1_AG  0x6B
#define PRINT_CALCULATED // This line is active - the more-useful calculated values will print - see below
// #define PRINT_RAW // This line is not active (commented out) 
#define PRINT_SPEED 250
#define DECLINATION -12 // Irvine, CA declination 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  quadrant = 1;
  color = 1;
  /* Required settings for IMU communication */
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  imu.readMag();
  initdrxn = (imu.calcMag(imu.my))/(imu.calcMag(imu.mx));
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
  Serial.println("yo");
}

void loop() {
  // put your main code here, to run repeatedly:
  //calculate altitude values
  imu.readMag();
  imu.readAccel();
  AX = imu.calcAccel(imu.ax);
  AY = imu.calcAccel(imu.ay);
  AZ = imu.calcAccel(imu.az);
  MX = imu.calcMag(imu.mx);
  MY = imu.calcMag(imu.my);
  MZ = imu.calcMag(imu.mz);

  roll = atan2(AY, AZ);//rads
  roll *= 180.0 / PI; // Angle of roll [degrees]

  pitch = atan2(-AX, sqrt(AY * AY + AZ * AZ));//rads
  pitch *= 180.0 / PI; // Angle of pitch [degrees]
  pitch -= 1.75;

  heading = atan2(MY, MX); //substitutes yaw
  
  heading -= DECLINATION * PI / 180;
  heading *= 180.0 / PI; // Angle of heading [degrees] where 0 deg = 360 deg = North
  heading += .37;

  //check stability
  if (abs(roll) < 15 && abs(pitch) < 15 && abs(heading) < initdrxn + 15) {
    stable = true;
  } else {
    stable = false;
  }
  
  //Ultrasonic code, run if copter is stable
  //define quadrant conditions, quadrant 2 is upper left
  //and that when heading is NORTH:
  //ultrasonic 1 points north, 
  //ultrasonic 2 points west
  //ultrasonic 3 points to ground 
  if (stable){
    if (ult1.Ranging(CM) < 60 * 2.54 && ult2.Ranging(CM) < 60 * 2.54){
      quadrant = 2;
    }
    else if (ult2.Ranging(CM) < 60 * 2.54 && ult2.Ranging(CM)  < 120 * 2.54 && ult2.Ranging(CM) > 60 * 2.54){
      quadrant = 1;
    }
    else if (ult1.Ranging(CM) > 60 * 2.54 && ult1.Ranging(CM) < 120 * 2.54 && ult2.Ranging(CM) > 60 * 2.54 && ult2.Ranging(CM) < 120 * 2.54){
      quadrant = 4;
    }
    else if (ult1.Ranging(CM) > 60 * 2.54 && ult1.Ranging(CM) < 120 * 2.54 && ult2.Ranging(CM) < 60){
      quadrant = 3;
    } else {
      quadrant = 1;
    }
    //use 3rd ultrasonic to determine height, and corresponding color to send thru xbee
    if (quadrant >= 1 && quadrant <= 4){
      if (ult3.Ranging(CM) > 6.5 * 2.54 && ult3.Ranging(CM) < 8 * 2.54) {
        color = 1;
      }
      if (ult3.Ranging(CM) > 5  * 2.54 && ult3.Ranging(CM) < 6.5 * 2.54) {
        color = 2;
      }
      if (ult3.Ranging(CM) > 3.5 * 2.54 && ult3.Ranging(CM) < 5 * 2.54) {
        color = 3;
      }
      if (ult3.Ranging(CM) > 2 * 2.54 && ult3.Ranging(CM) < 3.5 * 2.54) {
        color = 4;
      }
    }
  } else {
    Serial.print("unstable"); //used in testing
  }
  //________________________________________________________________________________________________________
  //XBee code, sends color and quadrant data to led panel
  
  Serial.write(a); //FIRST: Send 'E' which indicates a new set of data is being transmitted
                   
  Serial.write(quadrant/256); // SECOND: Send the quadrant number as two bytes
  Serial.write(quadrant%256); 
  
  Serial.write(color/256); // THIRD: Send the color number as two bytes
  Serial.write(color%256);

  //USE THIS CODE TO TEST, IF THE SERIAL MONITOR SHOWS NUMBERS ITS WORKING
  Serial.println();
  Serial.print("ult1: ");
  Serial.print(ult1.Ranging(CM));
  
  Serial.print(" ult2: ");
  Serial.print(ult2.Ranging(CM));
  
  Serial.print(" ult3: ");
  Serial.print(ult3.Ranging(CM));
  
  Serial.print(" Quadrant: ");
  Serial.print(quadrant);
  
  Serial.print(" color :");
  Serial.print(color);
  Serial.println();
  delay(50);
}


