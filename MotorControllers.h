
#ifndef MotorControllers_h
#define MotorControllers_h

#include <Servo.h>    // servo library -- vexMotor relies on the Servo library
#include <vexMotor.h> // includes the vexMotor library (must also include Servo.h)

//pin 9 is right, pin 8 is left
#define RIGHT_MOTOR 9
#define LEFT_MOTOR 8
    
#define Right
class MotorControllers
{
  public:
    MotorControllers();
    void moveForward();
    void stopMove();
    void rotateLeft();
    void rotateRight();
  private:
    vexMotor rightMotor;  // creates an instance of the vexMotor class
    vexMotor leftMotor;
};

#endif

