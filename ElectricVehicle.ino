#include <LiquidCrystal.h>

// Motor will turn until TICK_COUNT encoder ticks occur. The program waits
// for the button to be pushed then resets and does it again.
#define TICK_COUNT  1000

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(0);

// Encoder information
const int encoderPin = 3;
unsigned long pulseCount = 0;
bool pulseCaught = false;

// Pin to reset the encoder pulse count to 0.
const int resetPin = 10;

// Motor control
const int motorEnablePin = 9;
const int motorPin1 = 11;
const int motorPin2 = 12;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);

  pinMode(resetPin, INPUT_PULLUP);
  pinMode(encoderPin, INPUT_PULLUP);
  pinMode(motorEnablePin, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(encoderPin), handleEncoderPulse, CHANGE);
}

void loop() {
  static int state = false;
  static unsigned long lastChange = 0;
  static int lastResetPinVal = HIGH;
  static int isRunning = false;
  static unsigned long targetCount = TICK_COUNT;
  static bool runCW = true;

  // Flash the built in led on pin 13 each second to indicate all is going well.
  if(millis() - lastChange > 1000)
  {
    state = !state;
    digitalWrite(13, state ? HIGH : LOW);
    lastChange = millis();
  }

  // Read the reset button and take appropriate action
  int resetPinVal = digitalRead(resetPin);
  if(resetPinVal == LOW && resetPinVal != lastResetPinVal) {
    if(isRunning) {
      // Add TICK_COUNT to targetCount and keep going
      targetCount += TICK_COUNT;
    }
    else {
      // Reset everything and start again.
      pulseCount = 0;
      targetCount = TICK_COUNT;
      pulseCaught = true; // Set this to true so the current count gets sent to the LCD below.
      isRunning = true;
      runCW = !runCW;
      // Set motor direction
      digitalWrite(motorPin1, runCW ? HIGH : LOW);
      digitalWrite(motorPin2, runCW ? LOW : HIGH);
      // Enable motor
      //analogWrite(motorEnablePin, 255);
      digitalWrite(motorEnablePin, HIGH);
    }

    // Report status to the LCD
    lcd.setCursor(0, 1);
    // print the target count
    char outString[20];
    sprintf(outString, "Target: %-11d", targetCount);
    lcd.print(outString);
  }
  lastResetPinVal = resetPinVal;

  // Handle a new pulse value from the interrupt code
  if(pulseCaught)
  {
    if(pulseCount >= targetCount) {
      // Stop motor
      // Reverse current for a split second
      digitalWrite(motorPin1, !runCW ? HIGH : LOW);
      digitalWrite(motorPin2, !runCW ? LOW : HIGH);
      delay(100);
      digitalWrite(motorEnablePin, LOW);
      // Set is running flag
      isRunning = false;
    }
    //Report status
    // set the cursor to column 0, display line 2
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 0);
    // Print the current pluse count
    char outString[20];
    sprintf(outString, "Current: %-10d", pulseCount);
    lcd.print(outString);
    pulseCaught = false;
  }
}

void handleEncoderPulse()
{
  pulseCount++;
  pulseCaught = true;
}

