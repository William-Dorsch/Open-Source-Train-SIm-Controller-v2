const int bell = 5;
const int bail = 4;
const int sandMomentary = 3;
const int sandOn = 3;
const int sandOff = 7;
const int autoBrake = A0;
const int indyBrake = A1;
const int throttle = A3;
const int reverser = A4;
int sandState = 0;
int bellStateUDP = 0;
int hornStateUDP = 0;
int sandStateUDP = 0;
int automaticBrakePositionUDP = 0;
int sandToggleLogic = 0;
int autoValue = 0;
int mappedValue = 0;
int throttlePositionUDP = 0;
int throttlePosition = 0;  // Potentiometer value
int throttleNotch = 0;     // Calculated throttle notch (0-8)
int independentBrakePositionUDP = 0;
int independentBrakeBailUDP = 0;
int indyValue = 0;
int indyMappedValue = 0;
int notchCenters[9] = {475, 506, 535, 565, 594, 625, 654, 682, 704};
int dynoMappedValue = 0;
int reverserStateUDP = 0;
int dynamicBrakePositionUDP = 0;

// Track previous throttle position to determine direction
int previousThrottlePosition = 0;
bool increasingThrottle = true;
unsigned long autoHighStartTime = 0; // time when autoValue first >= 650
bool autoHighTimerRunning = false;   // track if timer is running
const unsigned long holdDuration = 500; // 500 ms
int autoMappedValue = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(bell, INPUT_PULLUP);
  pinMode(bail, INPUT_PULLUP);
  pinMode(sandMomentary, INPUT_PULLUP);
  pinMode(autoBrake, INPUT);
  pinMode(indyBrake, INPUT);
  pinMode(throttle, INPUT);
  pinMode(reverser, INPUT);
  Serial.begin(9600);
}

void loop() {
  autoValue = analogRead(autoBrake);  // Read the potentiometer value (0 to 1023)
  // ignore all the commented stuff, everything works as is I swear
  //Serial.println(autoValue);
   //Map the potentiometer value to the range 0-255, with 360 -> 0 and 585 -> 255
  //mappedValue = map(autoValue, 405, 650, 0, 255);
  //Serial.println(analogRead(A3));
   //Constrain the output to stay within the 0-255 range
  //automaticBrakePositionUDP = constrain(mappedValue, 0, 255);
  int autoMappedValue;
/*
  if (autoValue >= 650) {
    autoMappedValue = 255;
  } else if (autoValue >= 600 && autoValue <= 640) {
    autoMappedValue = 254;
  } else if (autoValue > 555 && autoValue < 605) {
    // Smooth transition from 250 to 254 between 556 and 604
    autoMappedValue = map(autoValue, 556, 604, 251, 253);
  } else if (autoValue >= 405 && autoValue <= 555) {
    autoMappedValue = map(autoValue, 405, 555, 0, 250);
  } else {
    autoMappedValue = 0; // Below 405
  }
  automaticBrakePositionUDP = constrain(autoMappedValue, 0, 255);
*/
  if (autoValue >= 750) {
    if (!autoHighTimerRunning) {
      autoHighStartTime = millis();
      autoHighTimerRunning = true;
    }

    if (millis() - autoHighStartTime >= holdDuration) {
      autoMappedValue = 255;
    } else {
      // Not yet 500ms, keep previous value or set to transitional value
      autoMappedValue = 254; // or a temp value if needed
    }
  } else {
    autoHighTimerRunning = false; // reset timer if condition breaks

    if (autoValue >= 700 && autoValue < 750) {
      autoMappedValue = 254;
    } else if (autoValue >= 580 && autoValue < 700) {
      autoMappedValue = 253;
    } else if (autoValue >= 545 && autoValue < 580) {
      autoMappedValue = 252;
    } else if (autoValue >= 345 && autoValue < 545) {
      autoMappedValue = map(autoValue, 345, 545, 1, 252);
    } else {
      autoMappedValue = 0; // Below 405
    }
  }

  automaticBrakePositionUDP = constrain(autoMappedValue, 0, 255);
  //Serial.println(automaticBrakePositionUDP);
  indyValue = analogRead(indyBrake);

  indyMappedValue = map(indyValue, 635, 315, 0, 255);
  //Serial.println(indyValue);
  independentBrakePositionUDP = constrain(indyMappedValue, 0, 255);

  if (indyValue > 515){
    independentBrakeBailUDP = 1;
  } else {
    independentBrakeBailUDP = 0;
  }
  // Read potentiometer value
    throttlePosition = analogRead(throttle);  // Replace A0 with your throttle pin

    // Determine which notch the throttle is in
    int minDifference = abs(throttlePosition - notchCenters[0]);
    throttleNotch = 0;

    for (int i = 1; i < 9; i++) {
        int difference = abs(throttlePosition - notchCenters[i]);
        if (difference < minDifference) {
            minDifference = difference;
            throttleNotch = i;
        }
    }

    // Set throttlePositionUDP based on notch
    // this can definately be simplified but it works rn so I won't touch it
    switch (throttleNotch) {
        case 0:
            throttlePositionUDP = 152;
            throttlePositionUDP = 0;
            break;
        case 1:
            throttlePositionUDP = 128;
            throttlePositionUDP = 1;
            break;
        case 2:
            throttlePositionUDP = 129;
            throttlePositionUDP = 2;
            break;
        case 3:
            throttlePositionUDP = 132;
            throttlePositionUDP = 3;
            break;
        case 4:
            throttlePositionUDP = 133;
            throttlePositionUDP = 4;
            break;
        case 5:
            throttlePositionUDP = 142;
            throttlePositionUDP = 5;
            break;
        case 6:
            throttlePositionUDP = 143;
            throttlePositionUDP = 6;
            break;
        case 7:
            throttlePositionUDP = 134;
            throttlePositionUDP = 7;
            break;
        case 8:
            throttlePositionUDP = 135;
            throttlePositionUDP = 8;
            break;
    }
    
    
    
  // dynamic brake logic
  int dynoValue = analogRead(throttle);  
  //Serial.println(dynoValue);
  //Map the potentiometer value to the range 0-255, with 360 -> 0 and 585 -> 255
  dynoMappedValue = map(dynoValue, 400, 210, 0, 255);
  //Constrain the output to stay within the 0-255 range
  dynamicBrakePositionUDP = constrain(dynoMappedValue, 0, 255);

  
  // reverser logic
  int reverserPotentiometer = analogRead(reverser);
  if (reverserPotentiometer < 500){ 
    reverserStateUDP = 0;
  } else if (reverserPotentiometer > 545){
    reverserStateUDP = 255;
  } else {
    reverserStateUDP = 128;
  }

  // horn logic
  int hornState = digitalRead(bail);
  if (hornState == LOW) {
    hornStateUDP = 1;
  } else {
    hornStateUDP = 0;
  }
  
  // logic only works for run8 atm (and tsc since I fixed the code over in the c# script)
  int bellState = digitalRead(bell);
  if (bellState == LOW) {
    bellStateUDP = 1;
  } else {
    bellStateUDP = 0;
  }

  /*
  // former toggling sand logic, however I don't use it anymore
  if (digitalRead(sandOn == LOW)){
    sandToggleLogic = 1;
  } else if (digitalRead(sandOff) == LOW){
    sandToggleLogic = 0;
  }

  if (sandToggleLogic == 0){
    if (digitalRead(sandMomentary) == LOW){
      sandStateUDP = 1;
    } else {
      sandStateUDP = 0;
    }
  } else if (sandToggleLogic == 1){
    sandStateUDP = 1;
  }
  */

  // momentary sander logic
  if (digitalRead(sandMomentary) == LOW){
    sandStateUDP = 1;
  } else {
    sandStateUDP = 0;
  } 
  
  // sending data over the serial
  Serial.print("RS:");
  Serial.print(reverserStateUDP);
  Serial.print(",TH:");
  Serial.print(throttlePositionUDP);
  Serial.print(",DB:");
  Serial.print(dynamicBrakePositionUDP);
  Serial.print(",IB:");
  Serial.print(independentBrakePositionUDP);
  Serial.print(",AB:");
  Serial.print(automaticBrakePositionUDP);
  Serial.print(",IL:");
  Serial.print(independentBrakeBailUDP);
  Serial.print(",HR:");
  Serial.print(hornStateUDP);
  Serial.print(",BL:");
  Serial.print(bellStateUDP);
  /*
  // headlight controls if you want to use them
  Serial.println(",HLF:");
  Serial.print(frontHeadlightUDP);
  Serial.print(",HLR:");
  Serial.print(rearHeadlightUDP);

  // Alerter data if you want to use it
  Serial.print(",AL:");
  Serial.print(alerterPressedUDP);
  */
  Serial.print(",SD:");
  Serial.println(sandStateUDP);

  delay(50);

}

