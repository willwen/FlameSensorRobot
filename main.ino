#include <Arduino.h>
#include "MotorControllers.h"
#include "Encoder.h"
#include <ultrasonic.h>

#define STEPSIZE 90
#define ROTATIONSTEP 180
#define FRONTLAZERTHRESHOLD 15.0
#define SIDELAZERTHRESHOLD 9.50

//for flame sensor
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

int state = 0;//start off check for flame

ultrasonic frontUltrasonic(22, 23);
ultrasonic leftUltrasonic(24,25);
ultrasonic rightUltrasonic(26,27);
Encoder leftEncoder(2, 3); //on interrupt pins 2 and 3
MotorControllers* mc;


void setup()
{
  Serial.begin(9600);    // starts the Serial communication on Arduino
  //pin 9 is right, pin 8 is left 
  mc = new MotorControllers(9,8);
  leftEncoder.write(0);
}

void loop()
{
  switch(state){
    case 0:
    {
      mc->moveForward();
      Serial.println("moving forward");
      Serial.println(leftEncoder.read());
      if(leftEncoder.read() <= (-1 * STEPSIZE)){ //made a complete step
        leftEncoder.write(0);
        mc->stopMove();
        Serial.println("completed forward step.");
        state = 5; //go check for flame after a step
      }
      break;

    } // end case 0
    case 2: //rotate left 90 degrees
    {
      mc->rotateLeft();
      Serial.println("rotating left");
      Serial.println(leftEncoder.read());
      if(leftEncoder.read() >= (ROTATIONSTEP)){
        leftEncoder.write(0);
        Serial.println("completed 90 deg left rotation.");
        mc->stopMove();
        state = 5;//complete a rotation, check flame
      }
      break;
    } // end case 2
    case 3: //rotate left 90 degrees
    {
      mc->rotateRight();
      Serial.println("rotating right");
      Serial.println(leftEncoder.read());
      if(leftEncoder.read() <= (-1 *ROTATIONSTEP)){
        leftEncoder.write(0);
        Serial.println("completed 90 deg right rotation.");
        mc->stopMove();
        state = 5;//complete a rotation, check flame
      }
      break;
    }
    case 4:
    {
      //flame detected, turn on fan
      //rotate servo,
      //trigger relay
      
      //recheck if flame is extinguished
      break;

    } //end case 4
    case 5://check for flame
    {
    //      int flameSensorReading = analogRead(A0);
    //      Serial.println(flameSensorReading);
    //      int range = map(flameSensorReading, sensorMin, sensorMax, 0, 3);
    //      if(range == 0){
    //        Serial.println("** Close Fire **");
    //        state = 4;
    //        break;
    //      }
    //      else if(range == 1)
    //        Serial.println("** Distant Fire **");
    //      else if (range == 2)
    //        Serial.println("No Fire");
    //      //state = next state
    //      //no flame detected, check for obstacle in front
      state = 6;
      break;

    } //end case 5
    case 6:
    {
      //read max sonar forward sensor
      float frontDist = frontUltrasonic.distance();
      Serial.println("Sensor Reading:");
      Serial.println(frontDist);
      if(frontDist < FRONTLAZERTHRESHOLD){//obstacle detected, aka small range, aka small voltage
        //should eventually poll left and right sensor to find a favorable direction
        state = 2;
        break;
      }
      
      //see if left or right sensor detect an obstacle
      float leftDist = leftUltrasonic.distance();
      float rightDist = rightUltrasonic.distance();
      
      if(leftDist < SIDELAZERTHRESHOLD){
        state = 7;
      }
      else if(rightDist < SIDELAZERTHRESHOLD){
        state = 8;
      }
      else{
        //no obstacle go forward, take a step
        state = 0;
      }
      break;
    } //end case 6
    
    case 7: //rotate left 45 degrees
    {
      mc->rotateLeft();
      Serial.println("rotating left");
      Serial.println(leftEncoder.read());
      if(leftEncoder.read() >= (ROTATIONSTEP/2)){
        leftEncoder.write(0);
        Serial.println("completed 45 deg left rotation.");
        mc->stopMove();
        state = 5;//complete a rotation, check flame
      }
      break;
    }//end case 7
    
    case 8: //rotate right 45 degrees
    {
      mc->rotateRight();
      Serial.println("rotating right");
      Serial.println(leftEncoder.read());
      if(leftEncoder.read() <= (-1 *ROTATIONSTEP/2)){
        leftEncoder.write(0);
        Serial.println("completed 45 degree right rotation.");
        mc->stopMove();
        state = 5;//complete a rotation, check flame
      }
      break;
    }//end case 8
  }//end switch statement
} // end loop()
