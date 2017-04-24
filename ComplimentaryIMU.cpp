#include "ComplimentaryIMU.h"

ComplimentaryIMU::ComplimentaryIMU(){
   Wire.begin(); // i2c begin
   if (!gyro.init()){ // gyro init
       Serial.println("Failed to autodetect gyro type!");
       while (1); 
   }
   timer=millis(); // init timer for first reading
   gyro.enableDefault(); // gyro init. default 250/deg/s
   delay(1000);// allow time for gyro to settle
   Serial.println("starting calibration");
   gyroZero();
   Accel_Init();
}


void ComplimentaryIMU::readVals(){
    // reads imu every 20ms
   if((millis()-timer)>=20)  
   {
     complimentaryFilter();
   }
   // prints the gyro value once per second
   if((millis()-timer2)>=1000)  
   {
     printGyro();
     //get vars gyro_x, gyro_y, gyro_z
     
   }
}

void ComplimentaryIMU::gyroZero()
{
    // takes 200 samples of the gyro
    for (int i = 0; i < 200; i++) {
        gyro.read();
        gerrx += gyro.g.x;
        gerry += gyro.g.y;
        gerrz += gyro.g.z;
        delay(20);
    }
    gerrx = gerrx / 200; // average reading to obtain an error/offset
    gerry = gerry / 200;
    gerrz = gerrz / 200;

    Serial.println(gerrx); // print error vals
    Serial.println(gerry);
    Serial.println(gerrz);
}

void ComplimentaryIMU::readGyro()
{
    gyro.read(); // read gyro
    timer = millis(); //reset timer
    gyro_x = (float)(gyro.g.x - gerrx) * G_gain; // offset by error then multiply by gyro gain factor
    gyro_y = (float)(gyro.g.y - gerry) * G_gain;
    gyro_z = (float)(gyro.g.z - gerrz) * G_gain;

    gyro_x = gyro_x * G_Dt; // Multiply the angular rate by the time interval
    gyro_y = gyro_y * G_Dt;
    gyro_z = gyro_z * G_Dt;

    gyro_x += gyro_xold; // add the displacment(rotation) to the cumulative displacment
    gyro_y += gyro_yold;
    gyro_z += gyro_zold;

    gyro_xold = gyro_x; // Set the old gyro angle to the current gyro angle
    gyro_yold = gyro_y;
    gyro_zold = gyro_z;
}

void ComplimentaryIMU::printGyro()
{
    timer2 = millis();

    // The gyro_axis variable keeps track of roll, pitch,yaw based on the complimentary filter
    Serial.print(" GX: ");
    Serial.print(gyro_x);
    Serial.print(" GY: ");
    Serial.print(gyro_y);
    Serial.print(" GZ: ");
    Serial.print(gyro_z);

    Serial.print("  Ax =  ");
    Serial.print(accel_x);
    Serial.print("  Ay =  ");
    Serial.print(accel_y);
    Serial.print("  Az =  ");
    Serial.println(accel_z);
}

void ComplimentaryIMU::Accel_Init()
{
    accel.init();
    accel.enableDefault();
    Serial.print("Accel Device ID");
    Serial.println(accel.getDeviceType());
    switch (accel.getDeviceType()) {
    case LSM303::device_D:
        accel.writeReg(LSM303::CTRL2, 0x18); // 8 g full scale: AFS = 011
        break;
    case LSM303::device_DLHC:
        accel.writeReg(LSM303::CTRL_REG4_A, 0x28); // 8 g full scale: FS = 10; high resolution output mode
        break;
    default: // DLM, DLH
        accel.writeReg(LSM303::CTRL_REG4_A, 0x30); // 8 g full scale: FS = 11
    }
}

void ComplimentaryIMU::accelZero()
{
    //I found this to be more problematic than it was worth.
    //not implemented
    // takes 100 samples of the accel
    for (int i = 0; i < 100; i++) {
        gyro.read();
        aerrx += accel.a.x >> 4;
        aerry += accel.a.y >> 4;
        aerrz += accel.a.z >> 4;
        delay(10);
    }
    aerrx = gerrx / 100; // average reading to obtain an error/offset
    aerry = gerry / 100;
    aerrz = gerrz / 100;
    Serial.println("accel starting values");
    Serial.println(aerrx); // print error vals
    Serial.println(aerry);
    Serial.println(aerrz);
}

// Reads x,y and z accelerometer registers
void ComplimentaryIMU::readAccel()
{
    accel.readAcc();

    accel_x = accel.a.x >> 4; // shift left 4 bits to use 12-bit representation (1 g = 256)
    accel_y = accel.a.y >> 4;
    accel_z = accel.a.z >> 4;

    // accelerations in G
    accel_x = (accel_x / 256);
    accel_y = (accel_y / 256);
    accel_z = (accel_z / 256);
}

void ComplimentaryIMU::complimentaryFilter()
{
    readGyro();
    readAccel();
    float x_Acc, y_Acc;
    float magnitudeofAccel = (abs(accel_x) + abs(accel_y) + abs(accel_z));
    if (magnitudeofAccel < 6 && magnitudeofAccel > 1.2) {
        x_Acc = atan2(accel_y, accel_z) * 180 / PI;
        gyro_x = gyro_x * 0.98 + x_Acc * 0.02;

        y_Acc = atan2(accel_x, accel_z) * 180 / PI;
        gyro_y = gyro_y * 0.98 + y_Acc * 0.02;
    }
}
