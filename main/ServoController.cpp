#include "ServoController.h"

ServoController::ServoController(){
}

void ServoController::init(){
    fanServo.attach(SERVO_PIN);

}
void ServoController::doServoStep(){
     //handle servo rotation
    if (servoValue > 120) {
        isServoIncrement = false;
    }
    else if (servoValue < 50) {
        isServoIncrement = true;
    }
    if (isServoIncrement) {
        servoValue += SERVO_STEP;
    }
    else {
        servoValue -= SERVO_STEP;
    }
    fanServo.write(servoValue);
//    Serial.println("writing servo value:");
//    Serial.println(servoValue);
}

void ServoController::write(int val){
  fanServo.write(val);
}

