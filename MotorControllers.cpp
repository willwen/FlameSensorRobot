#include "MotorControllers.h"

MotorControllers::MotorControllers(){


}

void MotorControllers::init(){
  rightMotor.attach(9);
  leftMotor.attach(8);
  stopMove();
}

void MotorControllers::moveForward(){
  rightMotor.write(100);
  leftMotor.write(-100);

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

