#include <Zumo32U4.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4LineSensors.h>
#include <Zumo32U4ProximitySensors.h>
#include <Zumo32U4Buzzer.h>

// Create an instance of the motors object
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4Buzzer buzzer;

// Constant to set the speed of the motors
const int MOTOR_SPEED = 100;

// Multiplier to control the speed of the motors
int multiplier = 1;

// Constants to represent the different modes
const int MODE_ONE = 1, MODE_TWO = 2, MODE_THREE = 3;

// Current mode
int mode = MODE_TWO;

// Timer variables
unsigned long prevMillis = 0;
unsigned long eventInterval = 1000;

void setup() {
  // Start serial communication
  Serial.begin(9600);

  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();
  buzzer.play("a");
}

void loop() {
  // Check if there is serial input available
  if (Serial.available() > 0) {
    // Read the serial input as a string
    String cmd = Serial.readStringUntil('\n');

    // Switch based on the current mode
    switch (mode) {
      // If the current mode is MODE_ONE
      case MODE_ONE:
        // Call manualControl function with the cmd string as parameter
        manualControl(cmd);
        break;

      // If the current mode is MODE_TWO
      case MODE_TWO:
        // Call semiAutoControl function with the cmd string as parameter
        semiAutoControl(cmd);
        break;

      // If the current mode is MODE_THREE
      case MODE_THREE:
        // Do nothing
        break;
    }

    // Call the serialFlush function to clear the serial buffer
    serialFlush();
  }

  // Get the current millis
  unsigned long currMillis = millis();

  // Check if the current millis minus the previous millis is greater than or equal to eventInterval
  if (currMillis - prevMillis >= eventInterval) {
    // Set the prevMillis to the current millis
    prevMillis = currMillis;

    // Turn on all the LEDS
    ledRed(1), ledYellow(1), ledGreen(1);
  }

  // Check if the current millis minus the previous millis is greater than or equal to eventInterval / 2
  else if (currMillis - prevMillis >= eventInterval / 2) {
    // Turn off all the LEDS
    ledRed(0), ledYellow(0), ledGreen(0);
  }
}

/* 
   manualControl function processes commands received through serial communication 
   to control the movement of the motors. The received command is compared with 
   predefined keywords to determine the desired action: forward, backward, left, 
   right, stop, accelerate, and decelerate.
*/
void manualControl(String cmd) {
  // return if no command is received
  if (cmd == "") return;

  // set motor speeds based on the received command
  if (cmd == "forward") motors.setSpeeds(MOTOR_SPEED * multiplier, MOTOR_SPEED * multiplier);
  else if (cmd == "backward") motors.setSpeeds(-(MOTOR_SPEED * multiplier), -(MOTOR_SPEED * multiplier));
  else if (cmd == "left") motors.setSpeeds(MOTOR_SPEED * multiplier, -(MOTOR_SPEED * multiplier));
  else if (cmd == "right") motors.setSpeeds(-(MOTOR_SPEED * multiplier), MOTOR_SPEED * multiplier);
  else if (cmd == "stop") motors.setSpeeds(0, 0);
  else if (cmd == "accelerate") updateMultiplier(multiplier - 1);
  else if (cmd == "decelerate") updateMultiplier(multiplier + 1);
}

/* 
   semiAutoControl function processes commands received through serial communication
   to control the behavior of the semi-autonomous mode. The function uses a static
   boolean variable isAutoActive to keep track of the current state of the semi-autonomous
   mode. If the received command is "switch", the state of isAutoActive is toggled.
*/
void semiAutoControl(String cmd) {
  static bool isAutoActive = false;

  if (cmd == "switch") {
    isAutoActive = !isAutoActive;
    if (isAutoActive) {
      Serial.println("Semi-Autonomous Mode Active");
      motors.setSpeeds(MOTOR_SPEED * multiplier, MOTOR_SPEED * multiplier);
    } else {
      Serial.println("Semi-Autonomous Mode Inactive");
      motors.setSpeeds(0, 0);
    }
  }

  String name = "";

  Serial1.print("Make a turn using 'L' or 'R', then press 'C' to resume. \n");/*prints out to the user*/

    switch(cmd) {
      case 'l': case'L':{ /*If A is entered turn left*/
        name = "Left";
        Serial1.println("Left turn 90 degrees \n"); /*prints out the command to the user*/
        motors.setSpeeds(-MOTOR_TURN_SPEED, MOTOR_TURN_SPEED);
        delay(330);
        motors.setSpeeds(0, 0);
        status = false;
        autoNavigator();
        break;
      }

    case 'r': case'R':{ /*If A is entered turn left*/
        name = "Right";
        Serial1.println("Left turn 90 degrees \n"); /*prints out the command to the user*/
        motors.setSpeeds(MOTOR_TURN_SPEED, -MOTOR_TURN_SPEED);
        delay(330);
        motors.setSpeeds(0, 0);
        status = false;
        autoNavigator();
        break;
      }
    case 'b': case'B':{ /*If A is entered turn left*/
        name = "180";
        Serial1.println("Turn 180 degrees \n"); /*prints out the command to the user*/
        motors.setSpeeds(-MOTOR_TURN_SPEED, MOTOR_TURN_SPEED);
        delay(660);
        motors.setSpeeds(0, 0);
        status = false;
        autoNavigator();
        break;
      }
      return;
    }
      
}

/* 
   updateMultiplier function updates the value of the multiplier variable. The multiplier
   is used to adjust the speed of the motors. The function takes an integer value as an 
   argument and updates the multiplier if the value is between 0 and 4 (inclusive).
*/
void updateMultiplier(int m) {
  if (m >= 0 && m <= 4) multiplier = m;
}

/*
   serialFlush function is used to clear the serial buffer by reading all available
   data until there is no more data left in the buffer.
*/
void serialFlush() {
  while (Serial.available() > 0) Serial.read();
}