#include <Arduino.h>
#include "MotorControllers.h"
#include "Encoder.h"
#include <ultrasonic.h>

#define STEP_SIZE 90
#define ROTATION_STEP 180

#define FRONT_LAZER_THRESHOLD 15.0
#define SIDE_LAZER_THRESHOLD 9.50

#define FRONT_ULTRASONIC_INPUT 22
#define FRONT_ULTRASONIC_OUTPUT 23

#define LEFT_ULTRASONIC_INPUT 24
#define LEFT_ULTRASONIC_OUTPUT 25

#define RIGHT_ULTRASONIC_INPUT 26
#define RIGHT_ULTRASONIC_INPUT 27

#define FRONT_FLAME_SENSOR A1
#define LEFT_FLAME_SENSOR A2
#define RIGHT_FLAME_SENSOR A3
#define FRONT_FLAME_THRESHOLD_VALUE 500

#define PUSH_BUTTON A4
#define RELAY_DIGITAL_PIN 28

//for flame sensor
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

int state = 1;//start off waiting for button press

ultrasonic frontUltrasonic(FRONT_ULTRASONIC_INPUT, FRONT_ULTRASONIC_OUTPUT);
ultrasonic leftUltrasonic(LEFT_ULTRASONIC_INPUT, LEFT_ULTRASONIC_OUTPUT);
ultrasonic rightUltrasonic(RIGHT_ULTRASONIC_INPUT, RIGHT_ULTRASONIC_INPUT);
Encoder leftEncoder(2, 3); //on interrupt pins 2 and 3
MotorControllers* mc;
Servo fanServo;

//servo can go between 60 and 120
int servoValue = 60;
bool isServoIncrement = true;

bool startedFan = false;

unsigned long time;

void setup()
{
  Serial.begin(9600);    // starts the Serial communication on Arduino
  //pin 9 is right, pin 8 is left
  mc = new MotorControllers(9, 8);
  leftEncoder.write(0);
  //S7 pin
  fanServo.attach(7);
  pinMode(RELAY_DIGITAL_PIN, OUTPUT);

}


void loop()
{
  switch (state) {

    //handle servo rotation
    if(servoValue > 120){
      isServoIncrement = false;
    }
    else if (servoValue < 60){
      isServoIncrement = true;
    }
    if(isServoIncrement){
      servoValue += 1;
    }
    else{
      servoValue -=1;
    }
    fanServo.write(servoValue);

    case 0:
      {
        mc->moveForward();
        Serial.println("moving forward");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() <= (-1 * STEP_SIZE)) { //made a complete step
          leftEncoder.write(0);
          mc->stopMove();
          Serial.println("completed forward step.");
          state = 5; //go check for flame after a step
        }
        break;

      } // end case 0
    case 1:
      {
        while (analogRead(PUSH_BUTTON) != 0) {
          delay(500);
        }
        state = 5;
        break;
      }
    case 2: //rotate left 90 degrees
      {
        mc->rotateLeft();
        Serial.println("rotating left");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() >= (ROTATION_STEP)) {
          leftEncoder.write(0);
          Serial.println("completed 90 deg left rotation.");
          mc->stopMove();
          state = 5;//complete a rotation, check flame
        }
        break;
      } // end case 2
    case 3: //rotate right 90 degrees
      {
        mc->rotateRight();
        Serial.println("rotating right");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() <= (-1 * ROTATION_STEP)) {
          leftEncoder.write(0);
          Serial.println("completed 90 deg right rotation.");
          mc->stopMove();
          state = 5;//complete a rotation, check flame
        }
        break;
      }//end case 3
    case 4:
      {
        //flame detected, turn on fan
        //rotate servo,
        //trigger relay
        if(!startedFan){
          digitalWrite(4, HIGH);
          //start fan timer
          time = millis();
          startedFan = true;
        }
        //run fan for 5 seconds
        if(millis() - time == 5000){
          digitalWrite(4, LOW);
          startedFan = false;
          state = 9;
        }
        //recheck if flame is extinguished
        break;

      } //end case 4
    case 5://check for flame
      {
        int flameSensorReading = analogRead(FRONT_FLAME_SENSOR);
        if(flameSensorReading < FRONT_FLAME_THRESHOLD_VALUE){
          //flame closeby, turn on fan
          state = 4;
        }
        else{
          //no flame detected, check for obstacle in front
          state = 6;
        }
        break;

      } //end case 5
    case 6:
      {
        int leftFlameSensorReading = analogRead(LEFT_FLAME_SENSOR);
        int rightFlameSensorReading = analogRead(RIGHT_FLAME_SENSOR);

        if (leftFlameSensorReading < 700){
          //rotate left 45 degrees
          state = 7;
          break;
        }
        else if (rightFlameSensorReading < 700){
          //rotate right 45 degrees
          state = 8;
          break;
        }


        //read forward sensor
        float frontDist = frontUltrasonic.distance();
        Serial.println("Sensor Reading:");
        Serial.println(frontDist);
        if (frontDist < FRONT_LAZER_THRESHOLD) { //obstacle detected, aka small range, aka small voltage
          //just go rotate 90 left because both flame sensors read nothing
          state = 2;
          break;
        }

        //see if left or right sensor detect an obstacle
        float leftDist = leftUltrasonic.distance();
        float rightDist = rightUltrasonic.distance();
        //detect an obstacle on left side sensor
        if (leftDist < SIDE_LAZER_THRESHOLD) {
          //rotate left 45
          state = 7;
        }
        else if (rightDist < SIDE_LAZER_THRESHOLD) {
          state = 8;
        }
        else {
          //no obstacle, no useful flame readings,  go forward, take a step
          state = 0;
        }
        break;
      } //end case 6

    case 7: //rotate left 45 degrees
      {
        mc->rotateLeft();
        Serial.println("rotating left");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() >= (ROTATION_STEP / 2)) {
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
        if (leftEncoder.read() <= (-1 * ROTATION_STEP / 2)) {
          leftEncoder.write(0);
          Serial.println("completed 45 degree right rotation.");
          mc->stopMove();
          state = 5;//complete a rotation, check flame
        }
        break;
      }//end case 8
     case 9: //final check for flame still exist
     {
        int flameSensorReading = analogRead(FRONT_FLAME_SENSOR);
        if(flameSensorReading < 900){
          //flame still exists
          state = 6;
        }
        else{//flame extinguished, halt macine
          while(1);
        }
        break;
     }
  }//end switch statement
} // end loop()
