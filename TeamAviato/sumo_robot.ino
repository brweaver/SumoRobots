#include "sumo_robot.h"
#include "robot.h"
#include <Wire.h>
#include <Zumo32U4.h>

#define PRINT_LINE_SENSOR_DATA false
#define PRINT_PROX_SENSOR_DATA false

#define NUM_SENSORS 3
#define MOVING_AVG 10
#define SENSOR_THRESHOLD 500

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4LCD lcd;

Zumo32U4ProximitySensors proxSensors;
Zumo32U4LineSensors lineSensors;

Robot robot;

unsigned int lineSensorValues[NUM_SENSORS];
unsigned int lineSensorValuesAvg[NUM_SENSORS];
unsigned int lineSensorMovingAvg[MOVING_AVG][NUM_SENSORS];
unsigned int movAvgIndex = 0;

void setup() {

  randomSeed(millis());

  if (NUM_SENSORS==3) {
    lineSensors.initThreeSensors();
  } else {
    lineSensors.initFiveSensors();
  }

  proxSensors.initThreeSensors();
  proxSensors.setPulseOnTimeUs(457); // default is 421; changing for the hell of it
  proxSensors.setPulseOffTimeUs(330); // per http://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_proximity_sensors.html#a6d47a41b45a7088916fbeaa5b38c60b3

  //openingAnimation();
  waitForButtonA();

  buzzer.play(">g32>>c32");
  
  // Delay 5-seconds!
  delay(5000);
  
//  robot.taanabManeuver();
}

void loop() {
  
  // Respond to prox result
  ProxResult proxResult = readProxSensorsSimple();  
  switch (proxResult) {
    case Confused:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("Confused");
      robot.forward();
      break;
    case Nothing:
    default:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("No Target");
      robot.forward();
      break;
    case AheadQuarter:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("A");
      robot.forward();
      break;
    case AheadHalf:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("^");
      // move forward
      robot.forward();
      break;
    case AheadFull:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("|");
      robot.forward();
      break;
    case Left:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("<<");
      robot.turnLeft();
      break;
    case NudgeLeft:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("<");
      robot.nudgeLeft();
      break;
    case Right:
      lcd.clear();
      lcd.gotoXY(4, 1);
      lcd.print(">>");
      robot.turnRight();
      break;
    case NudgeRight:
      lcd.clear();
      lcd.gotoXY(5, 1);
      lcd.print(">");
      robot.nudgeRight();
      break;
  }

  LineResult lineResult = readLineSensorSimple();
  switch (lineResult) {
    case Miss:
      lcd.gotoXY(1, 0);
      lcd.print("no");
      break; 
    case HitCenter:
      lcd.gotoXY(1, 0);
      lcd.print("HC");
      robot.backupManeuver();
      break;
    case HitLeft:
      lcd.gotoXY(1, 0);
      lcd.print("HL");
      robot.turnRight(Robot::TURN_DELAY_90);
      break;
    case HitLeftCenter:
      lcd.gotoXY(1, 0);
      lcd.print("CL");
      robot.turnRight(Robot::TURN_DELAY_180);
      break;
    case HitRight:
      lcd.gotoXY(1, 0);
      lcd.print("HR");
      robot.turnLeft(Robot::TURN_DELAY_90);
      break;
    case HitRightCenter:
      lcd.gotoXY(1, 0);
      lcd.print("CR");
      robot.turnLeft(Robot::TURN_DELAY_180);
      break;
    
  }

  robot.refresh();
}

void printProxSensorResults(uint16_t sensor[]) {
    Serial.print(sensor[0]);
    Serial.print(" ");
    Serial.print(sensor[1]);
    Serial.print(" ");
    Serial.print(sensor[2]);
}

String proxResultToString(ProxResult r) {
  switch(r) {
      case AheadQuarter: return "AheadQuarter";
      case AheadHalf: return "AheadHalf";
      case AheadFull: return "AheadFull";
      case NudgeLeft: return "NudgeLeft";
      case Left: return "Left";
      case NudgeRight: return "NudgeRight";
      case Right: return "Right";
      case Nothing: return "Nothing";
      case Confused: return "Confused";
      default: return "oops.";
  }
}

LineResult readLineSensorSimple() {
  lineSensors.read(lineSensorValues);
  updateMovingAvg(lineSensorValues);

  if (PRINT_LINE_SENSOR_DATA) {
    Serial.print("Values: ");
    for (int i = 0; i < NUM_SENSORS; i++) {
      Serial.print(lineSensorValues[i]);
      Serial.print(" ");
    }
    Serial.print(" avg: ");
    for (int i = 0; i < NUM_SENSORS; i++) {
      Serial.print(lineSensorValuesAvg[i] / MOVING_AVG);
      Serial.print(" ");
    }
    Serial.println("");
  }

  bool leftHit = lineSensorHit(0);
  bool centerHit = lineSensorHit(1);
  bool rightHit = lineSensorHit(2);

  if (leftHit && centerHit && rightHit) return HitCenter;
  else if (leftHit && centerHit) return HitLeftCenter;
  else if (rightHit && centerHit) return HitRightCenter;
  else if (centerHit) return HitCenter;
  else if (leftHit) return HitLeft;
  else if (rightHit) return HitRight;
  
  return Miss;
}

bool lineSensorHit(int sensorIndex) {
  if (lineSensorValues[sensorIndex] < SENSOR_THRESHOLD) return true;
  //if (lineSensorValues[sensorIndex] < lineSensorValuesAvg[sensorIndex]*1.1) return true;
  return false;
}

ProxResult readProxSensorsSimple() {
  uint16_t minIRBrightness = 2; 
  uint16_t maxIRBrightness = 410; // 418 is the empirical max, but it's not always triggered. 
  uint16_t numBrightnessLevels = 20;

  uint16_t brightnessLevels[numBrightnessLevels];
  //deriveLinearBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  deriveExponentialBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  
  proxSensors.setBrightnessLevels(brightnessLevels, numBrightnessLevels);
  proxSensors.read();

  uint16_t sumLeftLED[] = {
    proxSensors.countsLeftWithLeftLeds(), 
    proxSensors.countsFrontWithLeftLeds(), 
    proxSensors.countsRightWithLeftLeds()}; 
  uint16_t sumRightLED[] = {
    proxSensors.countsLeftWithRightLeds(), 
    proxSensors.countsFrontWithRightLeds(), 
    proxSensors.countsRightWithRightLeds()};

  const int strongSignal = numBrightnessLevels * 0.9;
  const int modSignal = numBrightnessLevels * 0.6;
  const int weakSignal = numBrightnessLevels * 0.25;

  int diffFrontSignal = sumLeftLED[1] - sumRightLED[1]; 

  if (PRINT_PROX_SENSOR_DATA) {
    Serial.print("Left: ");
    printProxSensorResults(sumLeftLED);
    Serial.print(" Right: ");
    printProxSensorResults(sumRightLED);
  }
  
  // 90% 1/4
  // 80% 1/2 means (roughly) midcircle
  // 70% 1/2
  // 40% means 3/4 
  // 20% means (roughly) full circle

  if (sumLeftLED[1] >= modSignal && sumRightLED[1] >= modSignal) {
      return AheadFull;
  } else if (sumRightLED[2] >= weakSignal) {
    return Right;
  } else if (sumLeftLED[0] >= weakSignal) {
    return Left;
  } else if (diffFrontSignal > weakSignal) {
    return NudgeLeft;
  } else if (diffFrontSignal < -weakSignal) {
    return NudgeRight;
  } else if (sumLeftLED[1] >= modSignal && sumRightLED[1] >= modSignal && abs(diffFrontSignal) < weakSignal) {
    return AheadHalf;
  } else if (sumLeftLED[1] >= weakSignal && sumRightLED[1] >= weakSignal) {
    return AheadFull;
  }
  
  return Confused;
}

void deriveLinearBrightnessLevels(uint16_t min, uint16_t max, uint16_t count, uint16_t levels[]) {
  uint16_t x = (max - min)/(count-1);
  levels[0] = min;
  for (int i = 1; i < count; i++) {
    levels[i] = min + i*x;
  }
}

void deriveExponentialBrightnessLevels(uint16_t min, uint16_t max, uint16_t count, uint16_t levels[]) {
  float a = min;
  float b = pow(max/a, 1.0/(count-1.0));
  levels[0] = min;
  for (int i = 1; i < count; i++) {
    levels[i] = a*pow(b, i);
  }
}

void openingAnimation() {
  lcd.clear();
  lcd.print(">");

  ledRed(1);
  delay(500);

  ledRed(0);
  ledYellow(1);
  lcd.print(">");
  delay(500);

  ledRed(0);
  ledYellow(0);
  ledGreen(1);
  lcd.print(">");
}

void waitForButtonA() {
  lcd.clear();
  lcd.print(F("Press A"));
  lcd.gotoXY(0, 1);
  buttonA.waitForButton();
  lcd.clear();
}

void updateMovingAvg(unsigned int *values) {
  unsigned int i;
  
  for (i = 0; i < NUM_SENSORS; i++) {
    lineSensorValuesAvg[i] -= lineSensorMovingAvg[movAvgIndex][i];
    lineSensorMovingAvg[movAvgIndex][i] = values[i];
    lineSensorValuesAvg[i] += lineSensorMovingAvg[movAvgIndex][i];
  }

  movAvgIndex += 1;
  if (movAvgIndex >= MOVING_AVG) {
    movAvgIndex = 0;
  }
}

