#include "MotorControllers.h"

MotorControllers::MotorControllers(){


}

void MotorControllers::init(){
  rightMotor.attach(9);
  leftMotor.attach(8);
  stopMove();
}

const float kp = 1.5;
void MotorControllers::moveForward(float gyro_Z_after){
  float error = gyro_Z_after - gyro_Z_before; // if this is positive, robot veering left
  
  //if negative, robot is veering right
  rightMotor.write(100 - (kp *error));
  leftMotor.write(-100 - (kp * error));
  Serial.println("error value:");
  Serial.println(error);
  Serial.println("Right motor getting:");
  Serial.println(100 - (kp *error));
  Serial.println("Left motor getting:");
  Serial.println(-100 - (kp * error));
  
}
void MotorControllers::setInitialGyro_Z(float val){
  gyro_Z_before = val;
}

void MotorControllers::stopMove(){
  rightMotor.write(0);
  leftMotor.write(0);
}

void MotorControllers::rotateLeft(){
  rightMotor.write(50);
  leftMotor.write(50);
}
void MotorControllers::rotateRight(){
  rightMotor.write(-50);
  leftMotor.write(-50);

}

