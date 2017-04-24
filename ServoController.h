
#ifndef ServoController_h
#define ServoController_h

#include <Arduino.h>
#include <Servo.h>

#define SERVO_PIN 7

#define SERVO_STEP 1

class ServoController{
	public:
		ServoController();
		void doServoStep();
	private:
		Servo fanServo;
		//servo can go between 60 and 110
		int servoValue = 60;
		bool isServoIncrement = true;

};

#endif
