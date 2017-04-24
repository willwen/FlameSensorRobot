#ifndef FlameSensors_h
#define FlameSensors_h

#include <Arduino.h>
#define FRONT_FLAME_SENSOR A1
#define LEFT_FLAME_SENSOR A3
#define RIGHT_FLAME_SENSOR A2

#define FRONT_FLAME_THRESHOLD_VALUE 400
#define SIDE_FLAME_THRESHOLD_VALUE 750
class FlameSensors{
	public:
		FlameSensors();
		bool frontFlameDetected();
		bool sideFlameDetected();
	private:	
};

#endif
