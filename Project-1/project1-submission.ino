// ============================================================================
// Author        : Anushka Satav
// Course        : BMI/CEN 598 Embedded Machine Learning - Fall B 2025
// Project       : Project 1 – Embedded Finite State Machine (FSM)
// Hardware      : Arduino Nano 33 BLE Sense Rev2
// Description   : Implements a cyclic color-based FSM using the onboard RGB LED.
//                 - The LED color changes in sequence (DARK → RED → BLUE → GREEN).
//                 - User presses 'C' in Serial Monitor to manually advance states.
//                 - Automatic backward transitions occur after specific timeouts.
// ============================================================================


// --------------------------- Pin Definitions --------------------------------
#define ledR 22   // Red LED Pin
#define ledG 23   // Green LED Pin
#define ledB 24   // Blue LED Pin


// --------------------------- Global Variables -------------------------------

// List of all possible system states (in logical order)
String states[] = {"DARK", "RED", "BLUE", "GREEN"};

// Tracks the current state index (0 = DARK, 1 = RED, 2 = BLUE, 3 = GREEN)
int stateIndex = 0;

// Timestamp of the last state change (used for timing-based transitions)
unsigned long lastChange = 0;


// ============================================================================
// setup() – Initialization Routine
// Called once when the Arduino is powered ON or reset.
// ============================================================================
void setup() {
  // Initialize Serial Communication for debugging and virtual input
  Serial.begin(115200);

  // Configure LED pins as outputs
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);

  // Display initial system message
  Serial.println("-------------- Starting in DARK State --------------");

  // Initialize LED in the starting (DARK) state
  setColorLED(states[stateIndex]);
}


// ============================================================================
// loop() – Main Execution Loop
// Continuously monitors timing and user input to drive state transitions.
// ============================================================================
void loop() {
  // Record the current system time (in milliseconds)
  unsigned long now = millis();


  // --------------------------------------------------------------------------
  // SECTION 1: Automatic State Transitions (Time-driven)
  // --------------------------------------------------------------------------

  // Rule 1: RED → DARK after 5 seconds
  if (states[stateIndex] == "RED" && (now - lastChange) >= 5000) {
    stateIndex = 0; // 0 corresponds to DARK
    Serial.println("Timeout: RED → DARK");
    setColorLED(states[stateIndex]);
    lastChange = now;  // Reset timer
  }

  // Rule 2: BLUE → RED after 4 seconds
  else if (states[stateIndex] == "BLUE" && (now - lastChange) >= 4000) {
    stateIndex = 1; // 1 corresponds to RED
    Serial.println("Timeout: BLUE → RED");
    setColorLED(states[stateIndex]);
    lastChange = now;
  }

  // Rule 3: GREEN → BLUE after 3 seconds
  else if (states[stateIndex] == "GREEN" && (now - lastChange) >= 3000) {
    stateIndex = 2; // 2 corresponds to BLUE
    Serial.println("Timeout: GREEN → BLUE");
    setColorLED(states[stateIndex]);
    lastChange = now;
  }


  // --------------------------------------------------------------------------
  // SECTION 2: Manual State Transitions (User-driven)
  // --------------------------------------------------------------------------

  // Check if user input is available via Serial Monitor
  if (Serial.available() > 0) {
    char key = Serial.read();  // Read the character input

    // If user presses 'C', advance to the next state cyclically
    if (key == 'C') {
      stateIndex = (stateIndex + 1) % 4;  // Cycle: 0→1→2→3→0
      Serial.print("Button pressed: Changed to ");
      Serial.println(states[stateIndex]);

      // Update LED color and reset the state timer
      setColorLED(states[stateIndex]);
      lastChange = now;
    }
  }
}


// ============================================================================
// setColorLED() – LED Control Function
// Purpose : Maps logical color names to physical LED pin states.
// Note    : RGB LED on Nano 33 BLE Sense is COMMON ANODE (active LOW).
// ============================================================================
void setColorLED(String colorName) {

  // DARK state → all LEDs off (set HIGH)
  if (colorName == "DARK") {
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledB, HIGH);
  } 

  // RED state → Red ON, others OFF
  else if (colorName == "RED") {
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledB, HIGH);
  } 

  // GREEN state → Green ON, others OFF
  else if (colorName == "GREEN") {
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, LOW);
    digitalWrite(ledB, HIGH);
  } 

  // BLUE state → Blue ON, others OFF
  else if (colorName == "BLUE") {
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledB, LOW);
  }
}
