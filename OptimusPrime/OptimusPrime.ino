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



#define MAX_TURN_TIME 600
#define MIN_TURN_TIME 250

#define RUN_FREQ 25
#define LIGHT 200

#define PROX_THRESHOLD 2

#define TURN_SPEED 200
#define RUN_SPEED  400
#define SPEED_ADJ  150
#define CHANGE_DIR_DELAY (RUN_FREQ * 2)


void setup() {
  Wire.begin();

  // see jumper settings wrt three vs five sensor config
  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();

  // See QTRSensors.h: this is required to set up the values array
  calibrateLineSensors();

  lcd.clear();
  lcd.print("Press A");

  randomSeed(millis());

  while (!buttonA.getSingleDebouncedRelease()) {
    delay(50);   
  }
  lcd.clear();

  // meet rules call for a 5 second delay
  const char* b[] = {"5","4","3","2","1"};
  for (int i=0; i < 5; i++) {
    lcd.gotoXY(4, 1);
    lcd.print(b[i]);
    delay(1000);
  }
  lcd.clear();

  // evade!
  motors.setSpeeds(RUN_SPEED, -RUN_SPEED);
  delay(200);

  // GO!
  motors.setSpeeds(RUN_SPEED, RUN_SPEED);
}

// swiped verbatim from LineAndProximitySensors
void calibrateLineSensors() {
  // To indicate we are in calibration mode, turn on the yellow LED
  // and print "Line cal" on the LCD.
  ledYellow(1);
  lcd.clear();
  lcd.print("Line cal");

  char b[10]; 
  for (uint16_t i = 175; i > 0; i--) {
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

      // go left or right depending on which sensor... 
      int turn_time = MIN_TURN_TIME + random(MAX_TURN_TIME-MIN_TURN_TIME);
      int left = TURN_SPEED;
      int right = TURN_SPEED;

      // both - back up a little before turning
      if ((sensors[0] < LIGHT) && (sensors[2] < LIGHT)) {
        motors.setSpeeds(-TURN_SPEED, -TURN_SPEED);
        delay(turn_time);
      }

      if (sensors[0] < LIGHT) {
        right = -TURN_SPEED;
      } else {
        left = - TURN_SPEED;
      }
      motors.setSpeeds(left, right);
      delay(turn_time);  
    }

    int left = RUN_SPEED;
    int right = RUN_SPEED;

    proxSensors.read();
    uint8_t frontLeftValue = proxSensors.countsFrontWithLeftLeds();
    uint8_t leftLeftValue = proxSensors.countsLeftWithLeftLeds();
    uint8_t frontRightValue = proxSensors.countsFrontWithRightLeds();
    uint8_t rightRightValue = proxSensors.countsRightWithRightLeds();
    uint8_t totalLeftValue = frontLeftValue + leftLeftValue;
    uint8_t totalRightValue = frontRightValue + rightRightValue;
    boolean changeDirection = totalLeftValue >= PROX_THRESHOLD || totalRightValue >= PROX_THRESHOLD;
    if (changeDirection) {
      if (totalLeftValue > totalRightValue) {
        if (leftLeftValue > frontLeftValue) {
          left -= SPEED_ADJ * 2;
        } else {
          left -= SPEED_ADJ;
        }
      } else {
        if (rightRightValue > frontRightValue) {
          right -= SPEED_ADJ * 2;
        } else {
          right -= SPEED_ADJ;
        }
      }
    }

    // go fast
    motors.setSpeeds(left, right);

    // if we're veering, give a moment to let the turn take effect
    if (changeDirection) {
      delay(CHANGE_DIR_DELAY);
    }
  }
  

}
