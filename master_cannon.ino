/*
  This program is the MASTER:
  In charge of navigation (wheels) and distance measurement (sensor).
  The slave receives distance from sensor.
  Slave program found as slave_cannon.
*/

#include "Wire.h"

const int slave_port = 2; // make sure coincides with slave code

// for distance reading
const int trig = 11;
const int echo = 12;

const int led = 13;
const int ena = 5;
const int enb = 10;
const int in1 = 6;
const int in2 = 7;
const int in3 = 8;
const int in4 = 9;

long duration, distance; // Duration used to calculate distance

const int maximumRange = 400; // Max distance  
const int minimumRange = 1; // Min distance 

int readDistance()
{
  digitalWrite(trig, LOW); 
  delayMicroseconds(2); // prev 2

  digitalWrite(trig, HIGH);
  delayMicroseconds(10); // previously 10
 
  digitalWrite(trig, LOW);
  //delayMicroseconds(50);

  duration = pulseIn(echo, HIGH, 30000UL);
  if(duration > 0) {
    distance = (duration * 0.03395) / 2;
    if (distance < maximumRange && distance > minimumRange) {
      Serial.println(distance);
    } 
    else {
      Serial.println("Object out of range");
    }
  }
}

void setup() {
  Wire.begin(); // master doesn't need to address a port

  // angle reading
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // moving the car
  Serial.begin (9600);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  readDistance();
  Serial.println(distance);
  delay(500); // previously 50

  if (Serial.available() > 0) {
    char inputvalue = char(Serial.read());
    Serial.println(inputvalue);
    if (inputvalue == 'F') { // forward 
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(ena, 255);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      digitalWrite(enb, 255);
    }
    else if (inputvalue == 'B') { //backwards
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(ena, 255);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      digitalWrite(enb, 255);
    }

    else if (inputvalue == 'R') { //RIGHT
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(ena, 0);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      digitalWrite(enb, 255);
    }

    else if (inputvalue == 'L') { //LEFT
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(ena, 255);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      digitalWrite(enb, 0);
    }

    else if (inputvalue == 'C') { // counterclockwise rotation
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(ena, 255);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      digitalWrite(enb, 255);
    }

    else if (inputvalue == 'A') { // clock wise rotation
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(ena, 255);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      digitalWrite(enb, 255);
    }

    else if (inputvalue == 'M') { // LED ON 
      digitalWrite(led, HIGH);
    }

    else if (inputvalue == 'm') { // LED OFF 
      digitalWrite(led, LOW);
    }

    else if (inputvalue == 'N') { // wheels off 
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(ena, 0);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      digitalWrite(enb, 0);
    }
      
    // send distance to slave and shoot
    else if(inputvalue == 'S') {
      int16_t dist = distance; // within range, the value can be casted to an integer (orig long)
      Serial.println("output distance: ");
      Serial.println(dist);
      Wire.beginTransmission(slave_port);
      Wire.write(lowByte(dist)); // transmission event, send distance for angle calculation
      Wire.write(highByte(dist)); 
      Wire.endTransmission();
    }
  }
}
