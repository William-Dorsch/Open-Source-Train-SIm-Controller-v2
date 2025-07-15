#include <Keyboard.h>

const int buzzer = 2;
const int sandOn = 6;
const int sandMomentary = 5;
const int sandOff = 7;
const int door = 10;
const int bell = 5;
const int horn = 4;
const int reverser = A4;
const int throttle = A3;
const int brake = A0;
const int debounceDelay = 15; // Increased delay for debouncing
unsigned long lastBellPress = 0;

int sandState = 0;
int lastThrottlePosition = -1; // Initialize to an invalid notqqqqwwensure first reading is registered

// Define potentiometer center values for each notch
const int notchCenters[] = {475, 506, 535, 565, 594, 625, 654, 682, 704};
const int numNotches = sizeof(notchCenters) / sizeof(notchCenters[0]);
bool eBrakeActivated = false; // Flag to check if emergency brake has been activated
// Define a tolerance range around each notch center
const int tolerance = 10;

int lastBrakePosition = -1; // Initialize to an invalid position to ensure the first reading is registered

// Define potentiometer limits
const int brakeMin = 325; // Fully released
const int brakeMax = 580; // Fully applied
const int eBrakeThreshold = 750; // Emergency brake activation threshold

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  Serial.begin(9600);
  Keyboard.begin(); // Initialize keyboard functionality
}

void loop() {
  // put your main code here, to run repeatedly:


  int potValue = analogRead(throttle); // Read potentiometer value
  //Serial.println(potValue);
  // Find the current throttle position based on the closest notch center
  int throttlePosition = -1;
  for (int i = 0; i < numNotches; i++) {
    if (potValue >= notchCenters[i] - tolerance && potValue <= notchCenters[i] + tolerance) {
      throttlePosition = i;
      break;
    }
  }

  // Only send keypress if throttle position has changed
  if (throttlePosition != -1 && throttlePosition != lastThrottlePosition) {
  while (throttlePosition > lastThrottlePosition) {
    Keyboard.press('w');
    delay(20);
    Keyboard.release('w');
    lastThrottlePosition++;  // Move toward the current position
  }

  while (throttlePosition < lastThrottlePosition) {
    Keyboard.press('s');
    delay(20);
    Keyboard.release('s');
    lastThrottlePosition--;  // Move toward the current position
  }
}


  int BpotValue = analogRead(brake); // Read potentiometer value
  Serial.println(BpotValue);

  // Map potentiometer range (238-648) to brake positions (0-20)
  int brakePosition = map(BpotValue, brakeMin, brakeMax, 0, 20);
  brakePosition = constrain(brakePosition, 0, 20); // Ensure brake position stays within bounds

  // Check for emergency brake activation
  if (BpotValue >= eBrakeThreshold && !eBrakeActivated) {
    Keyboard.press(KEY_BACKSPACE); // Apply emergency brake
    delay(50); // Hold key press briefly
    Keyboard.release(KEY_BACKSPACE);
    eBrakeActivated = true; // Set the flag to prevent repeated presses
  } 
  else if (BpotValue < eBrakeThreshold) {
    eBrakeActivated = false; // Reset flag if below threshold
  }

  // Check if brake position has changed
  if (brakePosition != lastBrakePosition) {
    if (brakePosition > lastBrakePosition) {
      for (int i = lastBrakePosition + 1; i <= brakePosition; i++) {
        Keyboard.press('e'); // Press 'e' for each increment
        delay(20);
        Keyboard.release('e');
        delay(10); // Short delay between keystrokes
      }
    } 
    else if (brakePosition < lastBrakePosition) {
      for (int i = lastBrakePosition - 1; i >= brakePosition; i--) {
        Keyboard.press('q'); // Press 'q' for each decrement
        delay(20);
        Keyboard.release('q');
        delay(10); // Short delay between keystrokes
      }
    }

    lastBrakePosition = brakePosition; // Update last brake position
  }


  delay(0); // Small delay to stabilize readings

  if (digitalRead(horn) == LOW){
    Keyboard.press('n');
  } else if (digitalRead(horn) == HIGH){
    Keyboard.release('n');
  }

  if (digitalRead(bell) == LOW) {
        if (millis() - lastBellPress > debounceDelay) {
            Keyboard.press('b');
            lastBellPress = millis();
        }
    } else {
        Keyboard.release('b');
    }
  /*
  if (digitalRead(sandOn) == LOW){
    digitalWrite(2, HIGH);
    sandState = 1;
  } else if (digitalRead(sandOff) == LOW){
    digitalWrite(2, LOW);
    sandState = 0;
  } 

  if (sandState == 1){
    Keyboard.press(KEY_LEFT_SHIFT);
  } else if (sandState == 0){
    Keyboard.release(KEY_LEFT_SHIFT);
  }

  if (sandState == 0){
    if (digitalRead(sandMomentary) == LOW){
    Keyboard.press(KEY_LEFT_SHIFT);
    } else if (digitalRead(sandMomentary) == HIGH){
    Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  */
  if (digitalRead(door) == LOW){
    Keyboard.press('t');
  } else if (digitalRead(door) == HIGH){
    Keyboard.release('t');
  }
  delay(1);
  


}
