/*
  ------------------------------------------------------------
  Posture Detection Using BMI270 IMU - Arduino Nano 33 BLE Sense Rev2
  Author: Anushka Satav
  Description:
      This program reads accelerometer data from the BMI270 IMU
      on the Arduino Nano 33 BLE Sense Rev2. Based on the
      orientation of the board, the system detects the sleeping
      posture and blinks the onboard LED accordingly:

         • Supine  (lying face up)      → 1 blink
         • Prone   (lying face down)    → 2 blinks
         • Side    (left or right)      → 3 blinks

      NOTE: This code is part of Phase 4 of the project.
      Phases 1–3 (data collection, storage, and algorithm
      design) were completed earlier using Python analysis.

      Thresholds are taken from real collected data:
        - Supine:  az ≈ +0.96 g
        - Prone:   az ≈ −0.91 g
        - Side:    ay ≈ ±0.91 g

      Simple thresholds:
        az > +0.70 → Supine
        az < −0.70 → Prone
        |ay| > 0.70 → Side
  ------------------------------------------------------------
*/

#include <Arduino_BMI270_BMM150.h>

#define LED_PIN 13

// Thresholds calculated from recorded data
const float SUPINE_TH =  0.70;
const float PRONE_TH  = -0.70;
const float SIDE_TH   =  0.70;

// Used to avoid constant blinking
int lastPosture = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  while (!Serial) {}

  if (!IMU.begin()) {
    Serial.println("IMU failed to initialize.");
    while (1);
  }

  Serial.println("Posture Detection Started...");
}

// ------------------------------------------------------------
// Helper function: blink LED “count” times
// ------------------------------------------------------------
void blinkTimes(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
  }
}

void loop() {

  float ax, ay, az;

  // Read accelerometer if data is available
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);

    // Debug prints (optional)
    Serial.print("ax: "); Serial.print(ax, 3);
    Serial.print("  ay: "); Serial.print(ay, 3);
    Serial.print("  az: "); Serial.println(az, 3);

    int posture = 0;  // 0 = unknown

    // --------------------------
    // Simple posture detection
    // --------------------------
    if (az > SUPINE_TH) {
      posture = 1;    // Supine
    }
    else if (az < PRONE_TH) {
      posture = 2;    // Prone
    }
    else if (ay > SIDE_TH || ay < -SIDE_TH) {
      posture = 3;    // Side (left/right)
    }

    // Only blink when posture changes
    if (posture != lastPosture && posture != 0) {

      if (posture == 1) blinkTimes(1);
      if (posture == 2) blinkTimes(2);
      if (posture == 3) blinkTimes(3);

      lastPosture = posture;
      delay(1500);  // small delay to prevent rapid retrigger
    }
  }

  delay(50);  // ~20 Hz update rate
}
