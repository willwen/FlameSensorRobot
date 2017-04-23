
#ifndef MotorControllers_h
#define MotorControllers_h

#include <Servo.h>    // servo library -- vexMotor relies on the Servo library
#include <vexMotor.h> // includes the vexMotor library (must also include Servo.h)

class MotorControllers
{
  public:
    MotorControllers(int rightMotorDigitalPin, int leftMotorDigitalPin);
    void moveForward();
    void stopMove();
    void rotateLeft();
    void rotateRight();
  private:
    vexMotor rightMotor;  // creates an instance of the vexMotor class
    vexMotor leftMotor;
};

#endif
