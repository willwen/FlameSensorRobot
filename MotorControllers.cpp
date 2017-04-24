#include "MotorControllers.h"

MotorControllers::MotorControllers(){

  rightMotor.attach(RIGHT_MOTOR);
  leftMotor.attach(LEFT_MOTOR);


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

