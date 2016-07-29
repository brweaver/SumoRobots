#include "robot.h"
#include <Wire.h>
#include <Zumo32U4.h>

Robot::Robot() {
  randomSeed(millis());
}
void Robot::resetDefaultDelay() {
  stateInitTime = micros();
  stateDelayTime = Robot::defaultDelay;
}
void Robot::resetDelay(int delay) {
  stateInitTime = micros();
  stateDelayTime = Robot::defaultDelay;
}
Robot::RobotState Robot::state() {
  return currentState;
}
bool Robot::randomBool() {
  int r = random(0,100);
  return r % 2 == 0;
}

void Robot::refresh() {
  
  // Check Delay 
  if (currentState != rs_deadstop) {
    unsigned long t = micros();
    Serial.print(stateInitTime);
    Serial.print(" ");
    Serial.print(t);
    Serial.print(" ");
    Serial.print(t - stateInitTime);
    if (t - stateInitTime > stateDelayTime * 1000) {
      deadStop();
    }
  }
  
  switch (currentState) {
    case rs_forward:
      motors.setSpeeds(speed, speed);
      break;
    case rs_nudgeleft: 
      motors.setLeftSpeed(speed);
      motors.setRightSpeed(speed * NUDGE_REDUCTION);
      break;
    case rs_nudgeright:
      motors.setLeftSpeed(speed * NUDGE_REDUCTION);
      motors.setRightSpeed(speed);
      break;
    case rs_turnleft:
      motors.setLeftSpeed(-speed);
      motors.setRightSpeed(speed);
      break;
    case rs_turnright:
      motors.setLeftSpeed(speed);
      motors.setRightSpeed(-speed);
      break;
    case rs_deadstop:
      motors.setLeftSpeed(0);
      motors.setRightSpeed(0);
      break;
  }  
}

void Robot::forward() {
  currentState = rs_forward;
  resetDefaultDelay();
}
void Robot::turnLeft() {
  turnLeft(80);
}
void Robot::turnLeft(unsigned long delay) {
  currentState = rs_turnleft;
  resetDelay(delay);
}
void Robot::turnRight() {
  turnRight(80);
}
void Robot::turnRight(unsigned long delay) {
  currentState = rs_turnright;
  resetDelay(delay);
}
void Robot::nudgeLeft() {
  currentState = rs_nudgeleft;
  resetDefaultDelay();
}
void Robot::nudgeRight() {
  currentState = rs_nudgeright;
  resetDefaultDelay();
}
void Robot::deadStop() {
  currentState = rs_deadstop;
}

// Maneuvers are blocking--no sensor input or state will be  
// will be examined until the maneuver has completed. 

void Robot::taanabManeuver() {
  if (randomBool()) {
    motors.setLeftSpeed(-speed);
    motors.setRightSpeed(-speed*NUDGE_REDUCTION);
  } else {
    motors.setRightSpeed(-speed);
    motors.setLeftSpeed(-speed*NUDGE_REDUCTION);
  }
  delay(450); // Whoa!
  forward();
}

void Robot::backupManeuver() {
  motors.setLeftSpeed(-speed);
  motors.setRightSpeed(-speed);
  delay(80); // note, this is actually dangerous!!!
  if (randomBool()) {
    turnLeft(Robot::TURN_DELAY_180);
  } else {
    turnRight(Robot::TURN_DELAY_180);
  }
}

