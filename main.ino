#include <Arduino.h>
#include <Bounce2.h>
#include "MotorControllers.h"
#include "Encoder.h"
#include "ComplimentaryIMU.h"
#include "FlameSensors.h"
#include "ServoController.h"
#include <ultrasonic.h>
#include "InternalMap.h"

#define STEP_SIZE 135
#define ROTATION_STEP 180

#define FRONT_LAZER_THRESHOLD 10.0
#define SIDE_LAZER_THRESHOLD 9.50

#define FRONT_ULTRASONIC_INPUT 22
#define FRONT_ULTRASONIC_OUTPUT 23

#define LEFT_ULTRASONIC_INPUT 24
#define LEFT_ULTRASONIC_OUTPUT 25

#define RIGHT_ULTRASONIC_INPUT 26
#define RIGHT_ULTRASONIC_OUTPUT 27

#define PUSH_BUTTON 28
#define RELAY_DIGITAL_PIN 29

#define ROTATE_90_LEFT_STATE 2
#define ROTATE_90_RIGHT_STATE 3
//for flame sensor
const int sensorMin = 0; // sensor minimum
const int sensorMax = 1024; // sensor maximum

int state = 1; //start off waiting for button press

Bounce startButtonDebouncer = Bounce();
ultrasonic frontUltrasonic(FRONT_ULTRASONIC_INPUT, FRONT_ULTRASONIC_OUTPUT);
ultrasonic leftUltrasonic(LEFT_ULTRASONIC_INPUT, LEFT_ULTRASONIC_OUTPUT);
ultrasonic rightUltrasonic(RIGHT_ULTRASONIC_INPUT, RIGHT_ULTRASONIC_OUTPUT);
Encoder leftEncoder(2,3); //on interrupt pins 2 and 3
MotorControllers mc;
InternalMap internalMap;

int x = 30;
int y = 30;


int direction = NORTH;

FlameSensors flameSensors;
ServoController servoController;

bool startedFan = false;

unsigned long time;

void setup()
{
    Serial.begin(9600); // starts the Serial communication on Arduino
    leftEncoder.write(0);
    pinMode(RELAY_DIGITAL_PIN, OUTPUT);
    pinMode(PUSH_BUTTON, INPUT_PULLUP);
    startButtonDebouncer.attach(PUSH_BUTTON);
    startButtonDebouncer.interval(5);
}

void loop()
{
   
    servoController.doServoStep();
    
    switch (state) {
    case 0: {
        mc.moveForward();
        Serial.println("moving forward");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() <= (-1 * STEP_SIZE)) { //made a complete step
            leftEncoder.write(0);
            mc.stopMove();
            Serial.println("completed forward step.");
            state = 5; //go check for flame after a step
            internalMap.updateStep(x, y, direction);
        }
        break;

    } // end case 0
    case 1: {
        while (1) {
            Serial.println("waiting on button press...");
            startButtonDebouncer.update();
            if (startButtonDebouncer.read() == LOW)
                break;
            delay(250);
        }
        state = 5;
        break;
    }
    case ROTATE_90_LEFT_STATE: //rotate left 90 degrees
    {
        mc.rotateLeft();
        Serial.println("rotating left");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() >= (ROTATION_STEP)) {
            leftEncoder.write(0);
            Serial.println("completed 90 deg left rotation.");
            mc.stopMove();
            state = 5; //complete a rotation, check flame

            /*
             *  static int NORTH = 0;
                static int EAST = 1;
                static int SOUTH = 2;
                static int WEST = 3;
             */
             direction = (direction - 1) % 4;
             internalMap.updateStep(x, y, direction);
        }
        break;
    } // end case 2
    case ROTATE_90_RIGHT_STATE: //rotate right 90 degrees
    {
        mc.rotateRight();
        Serial.println("rotating right");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() <= (-1 * ROTATION_STEP)) {
            leftEncoder.write(0);
            Serial.println("completed 90 deg right rotation.");
            mc.stopMove();
            state = 5; //complete a rotation, check flame
            /*
             *  static int NORTH = 0;
                static int EAST = 1;
                static int SOUTH = 2;
                static int WEST = 3;
             */
             direction = (direction + 1) % 4;
             internalMap.updateStep(x, y, direction);
        }
        break;
    } //end case 3
    case 4: {
        //flame detected, turn on fan
        //trigger relay
        digitalWrite(RELAY_DIGITAL_PIN, HIGH);
        if (!startedFan) {
            Serial.println("Flame detected ~~~");
            //start fan timer
            time = millis();
            startedFan = true;
        }
        //run fan for 5 seconds
        Serial.println("millis");
        Serial.println(millis());

        Serial.println("time");
        Serial.println(time);

        if (millis() - time >= 5000) {
            digitalWrite(RELAY_DIGITAL_PIN, LOW);
            startedFan = false;
            //recheck if flame is extinguished
            state = 9;
            //update time for use in state 9
            time = millis();
        }
        break;

    } //end case 4
    case 5: //check for flame
    {
        if (flameSensors.frontFlameDetected()) {
            //flame closeby, turn on fan
            state = 4;
        }
        else if(flameSensors.sideFlameDetected()){
          Serial.println("left or right found a flame!!");
          state = 10;
        }
        else {
            //no flame detected, check for obstacle in front
            state = 6;
        }
        break;

    } //end case 5
    case 6: {
//            randomSeed(frontDist);//provide random numbers
//            state = random(2, 4);
//            Serial.println("random value");
//            Serial.println(state);

        //read forward sensor
        float frontDist = frontUltrasonic.distance();
        Serial.println("Sensor Reading:");
        Serial.println(frontDist);
        float leftDist = leftUltrasonic.distance();
        float rightDist = rightUltrasonic.distance();
        char notAllowedPaths = 'A';
        if (frontDist < FRONT_LAZER_THRESHOLD) { //obstacle detected, aka small range, aka small voltage
            //just go rotate 90 left because both flame sensors read nothing
            notAllowedPaths = 'B';
        }
        if (leftDist < SIDE_LAZER_THRESHOLD) {
          if(notAllowedPaths == 'B'){
            notAllowedPaths = 'E';
          }
          else{
            notAllowedPaths = 'C';
          }
        }
        else if (rightDist < SIDE_LAZER_THRESHOLD) {
            if (notAllowedPaths == 'A') // all allowed

            else if(notAllowedPaths == 'B') //
            else if (notAllowedPaths == 'C')

            else if (notAllowedPaths == 'E')


        }

        //see if left or right sensor detect an obstacle
        switch(giveNewSafeSpot(x, y, notAllowedPaths)){
          case (GO_FORWARD):{
            state = 0;
            break;
          }
          case (GO_LEFT):{
            state = ROTATE_90_LEFT_STATE;
            break;
          }
          case (GO_RIGHT):{
            //rotate right 90
            state = ROTATE_90_RIGHT_STATE;
            break;
          }
          case(GO_REVERSE):{
            //need to code state
            break;
          }
         }

        Serial.println("leftDist:");
        Serial.println(leftDist);
        Serial.println("rightDist:");
        Serial.println(rightDist);
        //detect an obstacle on left side sensor

        else {
            //no obstacle, no useful flame readings,  go forward, take a step
            state = 0;
        }
        break;
    } //end case 6

//    case 7: //rotate left 45 degrees
//    {
//        mc.rotateLeft();
//        Serial.println("rotating 45 left");
//        Serial.println(leftEncoder.read());
//        if (leftEncoder.read() >= (ROTATION_STEP / 2)) {
//            leftEncoder.write(0);
//            Serial.println("completed 45 deg left rotation.");
//            mc.stopMove();
//            state = 5; //complete a rotation, check flame
//        }
//        break;
//    } //end case 7
//
//    case 8: //rotate right 45 degrees
//    {
//        mc.rotateRight();
//        Serial.println("rotating 45 right");
//        Serial.println(leftEncoder.read());
//        if (leftEncoder.read() <= (-1 * ROTATION_STEP / 2)) {
//            leftEncoder.write(0);
//            Serial.println("completed 45 degree right rotation.");
//            mc.stopMove();
//            state = 5; //complete a rotation, check flame
//        }
//        break;
//    } //end case 8
    case 9: //final check for flame still exist
    {   
        int flameSensorReading = analogRead(FRONT_FLAME_SENSOR);
        if (flameSensorReading < 900) {
            //flame still exists
            state = 5;
        }
        if (millis() - time >= 4000) { //assuming you scann for flame for 4 seconds, and you still see no flame
          //flame extinguished, halt macine
              while (1)
                  ;
        }
        break;
    }
    case 10: // YOU KNOW A FLAME IS CLOSEBY
    {
      //rotating right
      mc.rotateRight();

      if (analogRead(FRONT_FLAME_SENSOR) < 750){
        leftEncoder.write(0);
        Serial.println("completed 45 degree right rotation.");
        mc.stopMove();
        state = 4;
      }

      break;
    }
    } //end switch statement
} // end loop()




