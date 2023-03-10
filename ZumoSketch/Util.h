#pragma once

// Function to print the line sensor readings to the Serial monitor
void printReadingsToSerial() {
  char stringBuffer[200];
  sprintf(stringBuffer, "Values: %d %d %d %d %d %d \n",
          proximitySensorValues[0],
          proximitySensorValues[1],
          proximitySensorValues[2],
          proximitySensorValues[3],
          proximitySensorValues[4],
          proximitySensorValues[5]);
  SERIAL_COM.print(stringBuffer);
}

// Function to update the speed multiplier
void updateMultiplier(int m) {
  // Check if the provided multiplier is within the valid range (0 to 4)
  if (m >= 0 && m <= 4) {
    // If it is, update the multiplier
    multiplier = m;
  }

  motorSpeed = MOTOR_SPEED * multiplier;
}

// Function to flush the Serial buffer
void SerialFlush() {
  // Read and discard any data in the Serial buffer
  while (SERIAL_COM.available() > 0) SERIAL_COM.read();
}

// Function to rotate the robot by the specified angle at the specified speed
void turnZumo(int64_t angle, uint16_t motorSpeedLeft, uint16_t motorSpeedRight, bool (*condition)(int64_t newAngle)) {
  turnSensorReset();
  motors.setSpeeds(motorSpeedLeft, motorSpeedRight);
  while (condition(angle)) {
    turnSensorUpdate();
  }
  motors.setSpeeds(0, 0);
}

// Function to check if the robot as turned left by the specified angle
bool leftTurnCheck(int64_t newAngle) {
  if (turnAngle <= newAngle) {
    return true;
  }

  return false;
}

// Function to check if the robot as turned right by the specified angle
bool rightTurnCheck(int64_t newAngle) {
  if (turnAngle >= newAngle) {
    return true;
  }

  return false;
}

// Function to send the specified command to the server about the direction the robot is facing.
void drawLine(uint8_t newPos) {

  lastPos += newPos;

  // If the lastPos is greater than 3, we need to wrap it around
  // 0 = up, 1 = right, 2 = down, 3 = left
  if (lastPos > 3) {
    lastPos = 3 - (lastPos % 3);
  }

  // Takes the new position and sends the corresponding command to the server
  switch (lastPos) {
    case 0:
      SERIAL_COM.println("u");
      break;
    case 1:
      SERIAL_COM.println("r");
      break;
    case 2:
      SERIAL_COM.println("d");
      break;
    case 3:
      SERIAL_COM.println("l");
      break;
  }
}

// Function to send the specified command to the server if the robot has stopped or has found a point of interest
void drawStop(bool isPoint) {
  if (isPoint) {
    SERIAL_COM.println("p");
    delay(75);
    drawLine(0);
    return;      
  }
  SERIAL_COM.println("s");
}