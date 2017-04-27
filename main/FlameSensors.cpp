#include "FlameSensors.h"

FlameSensors::FlameSensors(){

}

bool FlameSensors::frontFlameDetected(){
  int flameSensorReading = analogRead(FRONT_FLAME_SENSOR);
  Serial.println("============================ front sensor");
  Serial.println(flameSensorReading);
	if (flameSensorReading < FRONT_FLAME_THRESHOLD_VALUE) {
            //flame closeby, turn on fan
            return true;
  }
  return false;
}

bool FlameSensors::sideFlameDetected(){
    int leftFlameSensorReading = analogRead(LEFT_FLAME_SENSOR);
    int rightFlameSensorReading = analogRead(RIGHT_FLAME_SENSOR);
    Serial.println("RRRRRRRRRRRRRRRRRRRRRRRRRRR right flame sensor value:");
    Serial.println(rightFlameSensorReading);
    Serial.println("LLLLLLLLLLLLLLLLLLLLLLLLLLL left flame sensor value:");
    Serial.println(leftFlameSensorReading);
	if (leftFlameSensorReading < SIDE_FLAME_THRESHOLD_VALUE
		|| rightFlameSensorReading < SIDE_FLAME_THRESHOLD_VALUE){
		return true;
	}
	return false;
}
