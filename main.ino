#include <Arduino.h>
#include <Bounce2.h>
#include "MotorControllers.h"
#include "Encoder.h"
#include "ComplimentaryIMU.h"
#include "FlameSensors.h"
#include "ServoController.h"
#include "InternalMap.h"
#include "UltrasonicSensors.h"
#include <LiquidCrystal.h>

#define PUSH_BUTTON 28
#define RELAY_DIGITAL_PIN 29
#define FIRE_FOUND_LED 53
#define FIRE_EXTINGUISHED_LED 52
#define LINE_SENSOR_PIN A0

#define BUTTON_PRESS_TO_START_STATE 0
#define MOVE_FORWARD_STATE 1
#define ROTATE_90_LEFT_STATE 2
#define ROTATE_90_RIGHT_STATE 3
#define BLAST_FAN_STATE 4
#define POLL_FLAME_SENSORS_STATE 5
#define POLL_WALL_SENSORS_STATE 6
#define CHECK_CLIFF_STATE 7
#define CHECK_IF_EXTINGUISHED_STATE 8
#define FLAME_CLOSEBY_STATE 9

int state = BUTTON_PRESS_TO_START_STATE; //start off waiting for button press
int direction = NORTH;
int x = 0;
int y = 0;
bool startedFan = false;
unsigned long time;

Bounce startButtonDebouncer = Bounce();
Encoder leftEncoder(2,3); //on interrupt pins 2 and 3
MotorControllers mc;
ComplimentaryIMU imu;
InternalMap internalMap;
LiquidCrystal lcd(40,41,42,43,44,45);
FlameSensors flameSensors;
ServoController servoController;
UltrasonicSensors ultrasonicSensors;

void setup()
{
    Serial.begin(9600); // starts the Serial communication on Arduino
    //set up pins
    pinMode(RELAY_DIGITAL_PIN, OUTPUT);
    pinMode(PUSH_BUTTON, INPUT_PULLUP);
    pinMode(FIRE_FOUND_LED, OUTPUT);
    pinMode(FIRE_EXTINGUISHED_LED, OUTPUT);
    digitalWrite(FIRE_FOUND_LED, LOW);
    digitalWrite(FIRE_EXTINGUISHED_LED, LOW);
    //set up start button debounce
    startButtonDebouncer.attach(PUSH_BUTTON);
    startButtonDebouncer.interval(5);
    //intialize motor controller
    mc.init();
    //initalize servo and set servo height level
    servoController.init();
    servoController.write(95);
    //set encoder val to 0
    leftEncoder.write(0);
    //update lcd screen
    lcd.begin(16,2);
    lcd.setCursor(0, 0);
    lcd.print("Calibrating IMU");
    lcd.setCursor(0,1);
    lcd.print("...");
    //needs a couple seconds to calibrate
    imu.init();
    //set the intial gyro value to know forward gyro value
    mc.setInitialGyro_Z(imu.gyro_x);
    Serial.println("gyro_x intial val");
    Serial.println(imu.gyro_x);
    //ready to start!    
    lcd.setCursor(0, 0);
    lcd.print("Press Button to ");
    lcd.setCursor(0,1);
    lcd.print("Start...");
}

void loop()
{
    Serial.println("state");
    Serial.println(state);
    //print IMU Vals every .5 seconds
    imu.readVals();
    if((millis()-imu.timer2)>=500)  
    {
      //subtract a constant factor for error in drift (error is about .04)
      imu.accountForError();          
    }

    switch (state) {
    case BUTTON_PRESS_TO_START_STATE:
        {
        Serial.println("waiting on button press...");
        startButtonDebouncer.update();
        if (startButtonDebouncer.read() == LOW) {
            Serial.println("broke out");
            lcd.clear();
            state = POLL_FLAME_SENSORS_STATE;
            break;
        }
        delay(250);
        break;
        }
    case MOVE_FORWARD_STATE:
        {
        mc.moveForward(imu.gyro_x);
        Serial.println("moving forward");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() <= (-1 * STEP_SIZE)) { //made a complete step
            leftEncoder.write(0);
            mc.stopMove();
            Serial.println("completed forward step.");
            state = POLL_FLAME_SENSORS_STATE; //go check for flame after a step
            internalMap.updateStep(x, y, direction);
        }
        break;
        } // end case 0
    
    case ROTATE_90_LEFT_STATE: //rotate left 90 degrees
        {
        mc.rotateLeft();
        Serial.println("rotating left");
        Serial.println(leftEncoder.read());
        if (leftEncoder.read() >= (ROTATION_STEP)) {
            leftEncoder.write(0);
            Serial.println("completed 90 deg left rotation.");
            mc.stopMove();
            state = POLL_FLAME_SENSORS_STATE; //complete a rotation, check flame
    
            direction = (direction - 1) % 4;
            if (direction < 0)
                direction = direction + 4;
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
            state = POLL_FLAME_SENSORS_STATE; //complete a rotation, check flame
            direction = (direction + 1) % 4;
            if (direction < 0)
                direction = direction + 4;
        }
        break;
        } //end case 3
    case BLAST_FAN_STATE:
        {
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
            state = CHECK_IF_EXTINGUISHED_STATE;
            //update time for use in state 9
            time = millis();
        }
        break;
    
        } //end case 4
    case POLL_FLAME_SENSORS_STATE://check for flame
        {
        Serial.println("~~~~~~~~~~~~~~STEP COMPLETED ~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        if (flameSensors.frontFlameDetected()) {
            digitalWrite(FIRE_FOUND_LED, HIGH);
            //flame closeby, turn on fan
            state = BLAST_FAN_STATE;
        }
        else if (flameSensors.sideFlameDetected()) {
            digitalWrite(FIRE_FOUND_LED, HIGH);
            Serial.println("left or right found a flame!!");
            time = millis();
            state = FLAME_CLOSEBY_STATE;
        }
        else {
            //no flame detected, check for cliff
            state = CHECK_CLIFF_STATE;
        }
        break;
      } //end case 5
    case POLL_WALL_SENSORS_STATE:
        {
        lcd.clear();
        lcd.setCursor(0, 0);
        // print the number of seconds since reset:
        lcd.print("x: ");
        lcd.print(x);
        lcd.print(" y: ");
        lcd.print(y);
        digitalWrite(FIRE_EXTINGUISHED_LED, LOW);
        //read forward sensor
        float frontDist = ultrasonicSensors.calcFrontDistance();
        float leftDist = ultrasonicSensors.calcLeftDistance();
        float rightDist = ultrasonicSensors.calcRightDistance();
        if (frontDist < FRONT_LAZER_THRESHOLD) { //obstacle detected, aka small range, aka small voltage
            //just go rotate 90 left because both flame sensors read nothing
            if (leftDist < rightDist) //right side has more area
                state = ROTATE_90_RIGHT_STATE;
            else
                state = ROTATE_90_LEFT_STATE;
        }
        else if (leftDist < SIDE_LAZER_THRESHOLD || rightDist < SIDE_LAZER_THRESHOLD) {
            //do not go forward..
            if (leftDist < rightDist) //right side has more area
                state = ROTATE_90_RIGHT_STATE;
            else
                state = ROTATE_90_LEFT_STATE;
        }
        else {
            Serial.println("INITAL GYRO _ x BEFORE MOVING FORWARD:");
            Serial.println(imu.gyro_x);
            mc.setInitialGyro_Z(imu.gyro_x);
            state = MOVE_FORWARD_STATE;
        }
        break;
        } //end case 6
    case CHECK_CLIFF_STATE://check for cliff
        {
        int val = analogRead(LINE_SENSOR_PIN);
        Serial.println("line sensor reading");
        Serial.println(val);
        if (val < 1000) // if just white surface
            state = POLL_WALL_SENSORS_STATE;
        else //detected black tape, aka cliff
            { //rotate left/ right
            if (ultrasonicSensors.leftMoreThanRight()) //left side has more area
                state = ROTATE_90_LEFT_STATE;
            else
                state = ROTATE_90_RIGHT_STATE;

        }
        break;
        }
    case CHECK_IF_EXTINGUISHED_STATE: //final check for flame still exist
        {
        int flameSensorReading = analogRead(FRONT_FLAME_SENSOR);
        if (flameSensorReading < 900) {
            //flame still exists
            state = POLL_FLAME_SENSORS_STATE;
        }
        if (millis() - time >= 4000) { //assuming you scann for flame for 4 seconds, and you still see no flame
            //flame extinguished, halt macine
            digitalWrite(FIRE_EXTINGUISHED_LED, HIGH);
            while (1)
                ;
        }
        break;
        }
    case FLAME_CLOSEBY_STATE: // YOU KNOW A FLAME IS CLOSEBY
        {
        //rotating right
        mc.rotateRight();
        servoController.doServoStep();
        Serial.println("state 10");
        if (analogRead(FRONT_FLAME_SENSOR) < 750) {
            leftEncoder.write(0);
            Serial.println("completed 45 degree right rotation.");
            mc.stopMove();
            state = BLAST_FAN_STATE;
        }
        else if (millis() - time >= 15000) { //assuming you scann for flame for 4 seconds,
            //go back to moving around
            state = POLL_FLAME_SENSORS_STATE;
        }
        break;
        }
    } //end switch statement
} // end loop()




