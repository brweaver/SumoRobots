#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

#define TURN_SPEED 200
#define RUN_SPEED  400

#define MAX_TURN_TIME 800
#define MIN_TURN_TIME 450

#define RUN_FREQ 50
#define LIGHT 200



void setup() {
 Wire.begin();

  // see jumper settings wrt three vs five sensor config
  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();

  // See QTRSensors.h: this is required to set up the values array
  calibrateLineSensors();

  randomSeed(millis());

  lcd.clear();
  lcd.print("Press A");
  
  while (!buttonA.getSingleDebouncedRelease()) {
    delay(50);   
  }
  lcd.clear();

  // GO!
  motors.setSpeeds(RUN_SPEED, RUN_SPEED);
}

// swiped verbatim from LineAndProximitySensors
void calibrateLineSensors()
{
  // To indicate we are in calibration mode, turn on the yellow LED
  // and print "Line cal" on the LCD.
  ledYellow(1);
  lcd.clear();
  lcd.print("Line cal");

  char b[10]; 
  for (uint16_t i = 150; i > 0; i--) {
    lcd.gotoXY(0, 1);
    sprintf(b, "%3d", i);
    lcd.print(b);
    lineSensors.calibrate();
  }

  ledYellow(0);
  lcd.clear();
}

static uint16_t lastSampleTime = 0;

void loop() {


  uint16_t now = (uint16_t)millis();
  if ((now - lastSampleTime) >= RUN_FREQ) {
    lastSampleTime = now;

    // check for line hit
    uint16_t sensors[3] = {0, 0, 0};    // we used the 3 sensor setup
    lineSensors.readCalibrated(sensors);

    if ((sensors[0] < LIGHT) || (sensors[1] < LIGHT) || (sensors[2] < LIGHT)) {
      // stop
      motors.setSpeeds(0, 0);

      // beep horn
      buzzer.playNote(NOTE_A(4), 200, 15);

      // turn left or right depending on which sensor... 
      int turn_time = MIN_TURN_TIME + random(MAX_TURN_TIME-MIN_TURN_TIME);
      int left = TURN_SPEED;
      int right = TURN_SPEED;
      if (sensors[0] < LIGHT) {
        right = -TURN_SPEED;
      } else {
        left = - TURN_SPEED;
      }
      motors.setSpeeds(left, right);
      delay(turn_time);

      // go fast
      motors.setSpeeds(RUN_SPEED, RUN_SPEED);
    }
  }

}
