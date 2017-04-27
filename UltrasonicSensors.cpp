#include "UltrasonicSensors.h"

UltrasonicSensors::UltrasonicSensors():
  frontUltrasonic(FRONT_ULTRASONIC_INPUT, FRONT_ULTRASONIC_OUTPUT),
  leftUltrasonic(LEFT_ULTRASONIC_INPUT, LEFT_ULTRASONIC_OUTPUT),
  rightUltrasonic(RIGHT_ULTRASONIC_INPUT, RIGHT_ULTRASONIC_OUTPUT)
{
	
}

float UltrasonicSensors::calcFrontDistance(){
	float val = frontUltrasonic.distance();
  Serial.println("Front Sensor Reading:");
	Serial.println(val);
	return val ;
}
float UltrasonicSensors::calcLeftDistance(){
	float val = leftUltrasonic.distance();
  Serial.println("Left Sensor Reading:");
	Serial.println(val);
	return val ;
}
float UltrasonicSensors::calcRightDistance(){
	float val = rightUltrasonic.distance();
  Serial.println("Right Sensor Reading:");
	Serial.println(val);
	return val ;
}
bool UltrasonicSensors::leftMoreThanRight(){
  if(leftUltrasonic.distance() > rightUltrasonic.distance())
    return 1;
  else
    return 0;
}

