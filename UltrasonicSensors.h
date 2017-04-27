#ifndef UltrasonicSensors_h
#define UltrasonicSensors_h

#define FRONT_LAZER_THRESHOLD 10.0
#define SIDE_LAZER_THRESHOLD 9.50

#define FRONT_ULTRASONIC_INPUT 22
#define FRONT_ULTRASONIC_OUTPUT 23

#define LEFT_ULTRASONIC_INPUT 24
#define LEFT_ULTRASONIC_OUTPUT 25

#define RIGHT_ULTRASONIC_INPUT 26
#define RIGHT_ULTRASONIC_OUTPUT 27

#include <Arduino.h>
#include <ultrasonic.h>





class UltrasonicSensors
{
	public:
		UltrasonicSensors();
		float calcFrontDistance();
		float calcLeftDistance();
		float calcRightDistance();
    bool leftMoreThanRight();
	private:
		ultrasonic frontUltrasonic;
		ultrasonic leftUltrasonic;
		ultrasonic rightUltrasonic;
};

#endif
