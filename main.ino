#include <Arduino.h>
#include <Bounce2.h>
#include "MotorControllers.h"
#include "Encoder.h"
#include "ComplimentaryIMU.h"
#include "FlameSensors.h"
#include "ServoController.h"
#include <ultrasonic.h>
#include "InternalMap.h"
#include <LiquidCrystal.h>

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

#define FIRE_FOUND_LED 53
#define FIRE_EXTINGUISHED_LED 52

#define LINE_SENSOR_PIN A0


int state = 1; //start off waiting for button press

Bounce startButtonDebouncer = Bounce();
ultrasonic frontUltrasonic(FRONT_ULTRASONIC_INPUT, FRONT_ULTRASONIC_OUTPUT);
ultrasonic leftUltrasonic(LEFT_ULTRASONIC_INPUT, LEFT_ULTRASONIC_OUTPUT);
ultrasonic rightUltrasonic(RIGHT_ULTRASONIC_INPUT, RIGHT_ULTRASONIC_OUTPUT);
Encoder leftEncoder(2,3); //on interrupt pins 2 and 3
MotorControllers mc;
ComplimentaryIMU imu;
InternalMap internalMap;

int x = 0;
int y = 0;

LiquidCrystal lcd(40,41,42,43,44,45);

int direction = NORTH;

FlameSensors flameSensors;
ServoController servoController;

bool startedFan = false;

unsigned long time;

void setup()
{
    lcd.begin(16,2);
    lcd.setCursor(0, 0);
    // print the number of seconds since reset:
    lcd.print("Calibrating IMU");
    
    lcd.setCursor(0,1);
    lcd.print("...");
    Serial.begin(9600); // starts the Serial communication on Arduino
    leftEncoder.write(0);
    pinMode(RELAY_DIGITAL_PIN, OUTPUT);
    pinMode(PUSH_BUTTON, INPUT_PULLUP);
    pinMode(FIRE_FOUND_LED, OUTPUT);
    pinMode(FIRE_EXTINGUISHED_LED, OUTPUT);
    digitalWrite(FIRE_FOUND_LED, LOW);
    digitalWrite(FIRE_EXTINGUISHED_LED, LOW);
    startButtonDebouncer.attach(PUSH_BUTTON);
    startButtonDebouncer.interval(5);
    mc.init();
    servoController.init();
    servoController.write(105);
    imu.init();
    mc.setInitialGyro_Z(imu.gyro_x);
    Serial.println("gyro_x intial val");
    Serial.println(imu.gyro_x);
    lcd.setCursor(0, 0);
    // print the number of seconds since reset:
    lcd.print("Press Button to ");
    lcd.setCursor(0,1);
    lcd.print("Start...");
}

void loop()
{
    
    Serial.println("state");
    Serial.println(state);
    imu.readVals();
    if((millis()-imu.timer2)>=500)  
    {
      imu.accountForError();          
    }
    switch (state) {
     
    
     case 0: {
        mc.moveForward(imu.gyro_x);
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
        Serial.println("waiting on button press...");
        startButtonDebouncer.update();
        if (startButtonDebouncer.read() == LOW){
            Serial.println("broke out");
            lcd.clear();
            state = 5;
            break;
        }
        delay(250);
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
            
             direction = (direction - 1) % 4;
             if(direction < 0)
              direction = direction+4;
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
             direction = (direction + 1) % 4;
             if(direction < 0)
              direction = direction+4;
        }
        break;
    } //end case 3
    case 4: {
        servoController.doServoStep();

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

        if (millis() - time >= 7500) {
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
        Serial.println("~~~~~~~~~~~~~~STEP COMPLETED ~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        if (flameSensors.frontFlameDetected()) {
            digitalWrite(FIRE_FOUND_LED,HIGH);
            //flame closeby, turn on fan
            state = 4;
        }
        else if(flameSensors.sideFlameDetected()){
          digitalWrite(FIRE_FOUND_LED,HIGH);
          Serial.println("left or right found a flame!!");
          time = millis();
          state = 10;
        }
        else {
            //no flame detected, check for cliff, then check for obstacle in front
            state = 7;
        }
        break;

    } //end case 5
    case 6: {
        lcd.clear();
        lcd.setCursor(0, 0);
        // print the number of seconds since reset:
        lcd.print("x: ");
        lcd.print(x);
        lcd.print(" y: ");
        lcd.print(y);
        digitalWrite(FIRE_EXTINGUISHED_LED, LOW);
        //read forward sensor
        float frontDist = frontUltrasonic.distance();
        float leftDist = leftUltrasonic.distance();
        float rightDist = rightUltrasonic.distance();
        Serial.println("Front Sensor Reading:");
        Serial.println(frontDist);
        Serial.println("Left Sensor Reading:");
        Serial.println(leftDist);
        Serial.println("Right Sensor Reading:");
        Serial.println(rightDist);
//        char notAllowedPaths = 'A';
        if (frontDist < FRONT_LAZER_THRESHOLD) { //obstacle detected, aka small range, aka small voltage
            //just go rotate 90 left because both flame sensors read nothing
//            Serial.println("Because of front sensor obstacle, not allowed to 'b'");
//            notAllowedPaths = 'B';
            if(leftDist < rightDist) //right side has more area
              state = ROTATE_90_RIGHT_STATE;
            else
              state = ROTATE_90_LEFT_STATE;
//            randomSeed(frontDist);//provide random numbers
//            state = random(2, 4);
//            Serial.println("random value");
//            Serial.println(random(2,4));
        }
        else if(leftDist<SIDE_LAZER_THRESHOLD || rightDist < SIDE_LAZER_THRESHOLD){
          //do not go forward..
//          notAllowedPaths = 'G';
            if(leftDist < rightDist) //right side has more area
              state = ROTATE_90_RIGHT_STATE;
            else
              state = ROTATE_90_LEFT_STATE;
        }
        else{
          Serial.println("INITAL GYRO _ x BEFORE MOVING FORWARD:");
          Serial.println(imu.gyro_x);
          mc.setInitialGyro_Z(imu.gyro_x);
          state = 0;
        }
//        if (leftDist < SIDE_LAZER_THRESHOLD) {
//          Serial.println("Because of left sensor obstacle");
//          
//          if(notAllowedPaths == 'B'){
//            notAllowedPaths = 'E';
//          }
//          else{
//            notAllowedPaths = 'C';
//          }
//          Serial.println(notAllowedPaths);
//
//        }
//        else if (rightDist < SIDE_LAZER_THRESHOLD) {
//            Serial.println("Because of left sensor obstacle");
//
//            if (notAllowedPaths == 'A') // all allowed
//              notAllowedPaths = 'D';
//            else if(notAllowedPaths == 'B')
//              notAllowedPaths = 'F';
//            else if (notAllowedPaths == 'C')
//              notAllowedPaths = 'G';
//            else if (notAllowedPaths == 'E')
//              notAllowedPaths = 'H';
//            Serial.println(notAllowedPaths);
//        }

        //see if left or right sensor detect an obstacle
//        switch(internalMap.giveNewSafeSpot(x, y, notAllowedPaths)){
//          case (GO_FORWARD):{
//            state = 0;
//            break;
//          }
//          case (GO_LEFT):{
//            state = ROTATE_90_LEFT_STATE;
//            break;
//          }
//          case (GO_RIGHT):{
//            //rotate right 90
//            state = ROTATE_90_RIGHT_STATE;
//            break;
//          }
//          case(GO_REVERSE):{
//            //need to code state
//            break;
//          }
//         }

//        Serial.println("leftDist:");
//        Serial.println(leftDist);
//        Serial.println("rightDist:");
//        Serial.println(rightDist);
        //detect an obstacle on left side sensor

      break;
    } //end case 6
    case 7: //check for cliff
    {
      int val = analogRead(LINE_SENSOR_PIN);
      Serial.println("line sensor reading");
      Serial.println(val);
      
      if(val < 1000)
        state = 6;
      else
      {
        if(leftUltrasonic.distance() <  rightUltrasonic.distance()) //right side has more area
            state = ROTATE_90_RIGHT_STATE;
        else
            state = ROTATE_90_LEFT_STATE;
      }
      break;
      
    }
    case 9: //final check for flame still exist
    {   
        int flameSensorReading = analogRead(FRONT_FLAME_SENSOR);
        if (flameSensorReading < 900) {
            //flame still exists
            state = 5;
        }
        if (millis() - time >= 4000) { //assuming you scann for flame for 4 seconds, and you still see no flame
          //flame extinguished, halt macine
          digitalWrite(FIRE_EXTINGUISHED_LED, HIGH);

              while (1)
                  ;
        }
        break;
    }
    case 10: // YOU KNOW A FLAME IS CLOSEBY
    {
      //rotating right
      mc.rotateRight();
      servoController.doServoStep();
      
      Serial.println("state 10");
      if (analogRead(FRONT_FLAME_SENSOR) < 750){
        leftEncoder.write(0);
        Serial.println("completed 45 degree right rotation.");
        mc.stopMove();
        state = 4;
      }
      else if (millis() - time >= 15000) { //assuming you scann for flame for 4 seconds, 
        //go back to moving around
          
         state = 5;
      }

      break;
    }

    } //end switch statement
} // end loop()




