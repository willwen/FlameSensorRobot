#ifndef MotorControllers_h
#define MotorControllers_h

#include <Servo.h>    // servo library -- vexMotor relies on the Servo library
#include <vexMotor.h> // includes the vexMotor library (must also include Servo.h)

#define STEP_SIZE 135
#define ROTATION_STEP 180

//pin 9 is right, pin 8 is left
const int RIGHT_MOTOR = 9;
const int LEFT_MOTOR = 8;

class MotorControllers
{
  public:
    MotorControllers();
    void moveForward(float gyro_Z_after);
    void stopMove();
    void rotateLeft();
    void rotateRight();
    void init();
    void MotorControllers::setInitialGyro_Z(float val);
  private:
    float gyro_Z_before;
    float gyro_Z_after;
    vexMotor rightMotor;  // creates an instance of the vexMotor class
    vexMotor leftMotor;
};

#endif

