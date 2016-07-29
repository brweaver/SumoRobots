


ProxResult readProxSensors() {

  proxSensors.pullupsOn();
  proxSensors.lineSensorEmittersOff();

  uint16_t numSensors = proxSensors.getNumSensors();
  uint16_t pulseOnTimeUs = Zumo32U4ProximitySensors::defaultPulseOnTimeUs;
  uint16_t pulseOffTimeUs = Zumo32U4ProximitySensors::defaultPulseOffTimeUs;
  uint16_t period = Zumo32U4ProximitySensors::defaultPeriod;

  uint16_t minIRBrightness = 2; 
  uint16_t maxIRBrightness = 410; // 418 is the empirical max, but it's not always triggered. ; // Derived empircally--largest value I could get a response from. 
  uint16_t numBrightnessLevels = 20;

  uint16_t brightnessLevels[numBrightnessLevels];
  //deriveLinearBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  deriveExponentialBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  
  uint16_t sumLeftLED[] = {0, 0, 0}; 
  uint16_t sumRightLED[] = {0, 0, 0};

  uint16_t leftLED[numBrightnessLevels][3];
  uint16_t rightLED[numBrightnessLevels][3];
  
  for (int b = 0; b < numBrightnessLevels; b++) {
    uint16_t brightness = brightnessLevels[b];
  
    //uint16_t dataLeftLED[] = {0, 0, 0}; 
    //uint16_t dataRightLED[] = {0, 0, 0};
    
    Zumo32U4IRPulses::start(Zumo32U4IRPulses::Left, brightness, period);
    delayMicroseconds(pulseOnTimeUs);
    for (uint8_t i = 0; i < numSensors; i++)
    {
      if (proxSensors.readBasic(i)) {
        //dataLeftLED[i]++;
        leftLED[b][i]++;
        sumLeftLED[i]++;
      }
      //sumLeftLED[i] += dataLeftLED[i];
    }
    Zumo32U4IRPulses::stop();
    delayMicroseconds(pulseOffTimeUs);
    
    Zumo32U4IRPulses::start(Zumo32U4IRPulses::Right, brightness, period);
    delayMicroseconds(pulseOnTimeUs);
    for (uint8_t i = 0; i < numSensors; i++)
    {
      if (proxSensors.readBasic(i)) {
        //dataRightLED[i]++;
        rightLED[b][i]++;
        sumRightLED[i]++;
      }
      //sumRightLED[i] += dataRightLED[i];
    }
    Zumo32U4IRPulses::stop();
    delayMicroseconds(pulseOffTimeUs);

//    if (shouldPrintProxSensorResults) {
//        Serial.print(" LeftLED: "); 
//        printProxSensorResults(leftLED[b]);    
//        Serial.print(" RightLED: ");
//        printProxSensorResults(rightLED[b]);
//        Serial.print("   Brightness: ");
//        Serial.print(brightness);
//        Serial.println("");
//    }
    
  }

  uint16_t zeroLeftLED[] = {0, 0, 0};
  uint16_t zeroRightLED[] = {0, 0, 0}; 
  for (int i = 0; i < numSensors; i++) {
    for (int b = 0; b < numBrightnessLevels; b++) {
      if (leftLED[b][i] > 0) break;
      zeroLeftLED[i]++;
    }
    for (int b = 0; b < numBrightnessLevels; b++) {
      if (rightLED[b][i] > 0) break;
      zeroRightLED[i]++;
    }
  }

//  if (shouldPrintProxSensorResults) {
//    Serial.print("SUM -- ");
//    Serial.print("LeftLED: ");
//    printProxSensorResults(sumLeftLED);    
//    Serial.print(" RightLED: ");
//    printProxSensorResults(sumRightLED);    
//    Serial.println("");
//  }

  // 
  const int strongSignal = numBrightnessLevels * 0.9;
  const int modSignal = numBrightnessLevels * 0.7;
  const int weakSignal = numBrightnessLevels * 0.3;

  int diffFrontSignal = sumLeftLED[1] - sumRightLED[1]; 

  // 90% 1/4
  // 80% 1/2 means (roughly) midcircle
  // 70% 1/2
  // 40% means 3/4 
  // 20% means (roughly) full circle

  if (sumLeftLED[1] >= strongSignal && sumRightLED[1] >= strongSignal) {
      return AheadQuarter;
  } else if (sumLeftLED[1] >= modSignal && sumRightLED[1] >= modSignal && abs(diffFrontSignal) < weakSignal) {
      return AheadHalf;
  } else if (diffFrontSignal > weakSignal) {
    return NudgeLeft;
  } else if (diffFrontSignal < -weakSignal) {
    return NudgeRight;
  } else if (sumLeftLED[0] > modSignal) {
    return Left;
  } else if (sumRightLED[2] > modSignal) {
    return Right;
  } else if (sumLeftLED[1] >= weakSignal && sumRightLED[1] >= weakSignal) {
    return AheadFull;
  }
  
  return Confused;
}


void calibrateProxSensors() {

  proxSensors.pullupsOn();
  proxSensors.lineSensorEmittersOff();

  uint16_t numSensors = proxSensors.getNumSensors();
  uint16_t pulseOnTimeUs = Zumo32U4ProximitySensors::defaultPulseOnTimeUs;
  uint16_t pulseOffTimeUs = Zumo32U4ProximitySensors::defaultPulseOffTimeUs;
  uint16_t period = Zumo32U4ProximitySensors::defaultPeriod;

  uint16_t minIRBrightness = 2; 
  uint16_t maxIRBrightness = 410; // 418 is the empirical max, but it's not always triggered. ; // Derived empircally--largest value I could get a response from. 
  uint16_t numBrightnessLevels = 20;

  uint16_t brightnessLevels[numBrightnessLevels];
  //deriveLinearBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  deriveExponentialBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  
  //uint16_t brightnessLevels[] = { 300, 320, 340, 360, 410, 415, 416, 417, 418, 419}; //2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
  
//  uint16_t brightnessLevels[numBrightnessLevels];
//  float value = 2;
//  float multiplier = 1.30;
//  for (int i = 0; i < numBrightnessLevels; i++) {
//    brightnessLevels[i] = value; 
//    value = 1 + value*multiplier;
//  }
  
  uint16_t sumLeftLED[] = {0, 0, 0}; 
  uint16_t sumRightLED[] = {0, 0, 0};
    
  for (int b = 0; b < numBrightnessLevels; b++) {
    uint16_t brightness = brightnessLevels[b];
  
    uint16_t dataLeftLED[] = {0, 0, 0}; 
    uint16_t dataRightLED[] = {0, 0, 0};
    
    Zumo32U4IRPulses::start(Zumo32U4IRPulses::Left, brightness, period);
    delayMicroseconds(pulseOnTimeUs);
    for (uint8_t i = 0; i < numSensors; i++)
    {
      if (proxSensors.readBasic(i)) dataLeftLED[i]++;
      sumLeftLED[i] += dataLeftLED[i];
    }
    Zumo32U4IRPulses::stop();
    delayMicroseconds(pulseOffTimeUs);
    
    Zumo32U4IRPulses::start(Zumo32U4IRPulses::Right, brightness, period);
    delayMicroseconds(pulseOnTimeUs);
    for (uint8_t i = 0; i < numSensors; i++)
    {
      if (proxSensors.readBasic(i)) dataRightLED[i]++;
      sumRightLED[i] += dataRightLED[i];
    }
    Zumo32U4IRPulses::stop();
    delayMicroseconds(pulseOffTimeUs);

    Serial.print("LeftLED: ");
    Serial.print(dataLeftLED[0]);
    Serial.print(" ");
    Serial.print(dataLeftLED[1]);
    Serial.print(" ");
    Serial.print(dataLeftLED[2]);
    Serial.print(" RightLED: ");
    Serial.print(dataRightLED[0]);
    Serial.print(" ");
    Serial.print(dataRightLED[1]);
    Serial.print(" ");
    Serial.print(dataRightLED[2]);
    Serial.print("   Brightness: ");
    Serial.print(brightness);
    Serial.println("");
  }
  Serial.print("SUM -- ");
  Serial.print("LeftLED: ");
  Serial.print(sumLeftLED[0]);
  Serial.print(" ");
  Serial.print(sumLeftLED[1]);
  Serial.print(" ");
  Serial.print(sumLeftLED[2]);
  Serial.print(" RightLED: ");
  Serial.print(sumRightLED[0]);
  Serial.print(" ");
  Serial.print(sumRightLED[1]);
  Serial.print(" ");
  Serial.print(sumRightLED[2]);
  Serial.println("");
}
