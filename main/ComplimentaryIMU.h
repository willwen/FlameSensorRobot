#ifndef ComplimentaryIMU_h
#define ComplimentaryIMU_h


//IMU Compliment
#include <Wire.h>
#include <L3G.h>
#include <LSM303.h>

const float error = -.04; //error in each IMU reading every second

class ComplimentaryIMU{
	public:
		L3G gyro;
		LSM303 accel;


		float gyro_x; //gyro x val
		float gyro_y; //gyro x val
		float gyro_z; //gyro x val
    
    ComplimentaryIMU();
		void gyroZero();
		void Accel_Init();
		void readVals();
    void init();
    long timer2 = 0;
    void accountForError();

	private:
		float G_Dt = 0.020; // Integration time (DCM algorithm)  We will run the integration loop at 50Hz if possible

		long timer = 0; //general purpose timer
		long timer1 = 0;
    float G_gain=.00875; // gyros gain factor for 250deg/sec

		float gyro_xold; //gyro cummulative x value
		float gyro_yold; //gyro cummulative y value
		float gyro_zold; //gyro cummulative z value
		float gerrx; // Gyro x error
		float gerry; // Gyro y error
		float gerrz; // Gyro 7 error

		float A_gain = .00875; // gyros gain factor for 250deg/sec
		float accel_x; //gyro x val
		float accel_y; //gyro x val
		float accel_z; //gyro x val
		float accel_xold; //gyro cummulative x value
		float accel_yold; //gyro cummulative y value
		float accel_zold; //gyro cummulative z value
		float aerrx; // Accel x error
		float aerry; // Accel y error
		float aerrz; // Accel 7 error

		void readGyro();
		void printGyro();
		void accelZero();
		void readAccel();
		void complimentaryFilter();

};


#endif
