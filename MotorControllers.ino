#include "MotorControllers.h"

MotorControllers::MotorControllers(int rightMotorDigitalPin, int leftMotorDigitalPin){

  rightMotor.attach(rightMotorDigitalPin);
  leftMotor.attach(leftMotorDigitalPin);


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
