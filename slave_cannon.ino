/*
  This program is the SLAVE:
  In charge of preparing the cannon, setting the angle, and firing.
  Master program found as master_cannon.
*/

#include "Servo.h"
#include "Wire.h"
#include "Math.h"

Servo servo;

volatile int16_t received = 0; // value received and updated by master

int motorPin1 = 3;
int motorPin2 = 6;

int motorState = 0; // 0: off, 1: on
unsigned long motorStartTime = 0;
const long motorDuration = 10000; // 7 seconds to shoot?
int speed = 200;

// constants for angle calculation
int init_vel = 4; // meters/s of the flywheel
float g = 9.81;

int servoPin = 8;

void setup() 
{
  Wire.begin(2); // ensure master has same port number
  Wire.onReceive(receiveEvent);

  servo.attach(servoPin);
  servo.write(0); // set servo at neutral (horizontal) (arg is angle)

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  Serial.begin(9600);
}

void loop() 
{
  // Check how long motor has been running
  if(motorState == 1) {
    if(millis() - motorStartTime >= motorDuration) {
      motorOff();
      motorState = 0;
      Serial.println("56: Motors turned off.");
      Serial.println("------------");
    }
  }

  delay(400);
}

int16_t calculateAngle(int16_t distance)
{
  // from the sensor's position, calculate angle needed to hit target
  float correctedDistance = distance + 25;

  float distMeters = (float)correctedDistance/100;

  Serial.println((String) "71: corrected distance: " + correctedDistance);

  // angle calculation, broken in numerator and denominator
  float num = pow(init_vel, 2) + sqrt(abs(pow(init_vel, 4) - sq(g) * sq(distMeters)));
  
  float den = g*distMeters;

  float ret = (atan(num/den)) * 180 / PI; // atan returns in rad, convert to deg

  Serial.println((String) "78: Unrounded calculated angle: " + ret);

  return round(ret); // servo.write only takes int
}

void prepareCannon(int16_t angle)
{
  for(int x = 0; x <= angle; x++)
  {
    servo.write(x);
    delay(10);
  }
  Serial.println((String)"90 prepare cannon arg angle: "+ angle);
}

void motorOn(int speed)
{
  analogWrite(motorPin1, speed);
  analogWrite(motorPin2, speed); // turn motors on
  Serial.println("105: Motors turned on.");
}

void motorOff()
{
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW); // turn motors off
}


void receiveEvent(int howMany) {
  if(howMany >= 2) {
    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    received = (int16_t)( (msb << 8) | lsb ); // reconstruct LSB-first
    
    Serial.println((String) "121 distance received from master: " + received); // debug

    int16_t angle = calculateAngle(received);
    Serial.println((String)"124 Calculated Angle: "+ angle);

    prepareCannon(angle);
    motorOn(speed); // arg is speed
    motorStartTime = millis(); // record current time
    motorState = 1; // check loop() 
  }
  else if(howMany == 1) {
    Wire.read(); // handles single byte msgs or error
  }
}
