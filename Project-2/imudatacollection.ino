/*
  ------------------------------------------------------------
  File: imudatacollection.ino
  Author: Anushka Satav
  Description:
      This program records accelerometer data from the onboard
      BMI270 IMU on the Arduino Nano 33 BLE Sense Rev2.
      The readings are streamed to the Serial Monitor in CSV
      format and will later be saved on the computer as .csv
      files for posture analysis.

      Only the accelerometer is used because posture
      classification depends on board orientation (gravity vector).

  How to Use:
      1. Upload this file to the Arduino.
      2. Open Serial Monitor at 115200 baud.
      3. Move the board into a posture and record for ~60 seconds.
      4. Copy Serial output → Save as .csv file.

  Notes:
      - Sampling frequency: ~50 Hz (20 ms per sample)
      - Accelerometer units: g
  ------------------------------------------------------------
*/

#include <Arduino_BMI270_BMM150.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial Monitor
  }

  Serial.println("Initializing BMI270 accelerometer...");

  if (!IMU.begin()) {
    Serial.println("ERROR: IMU failed to initialize.");
    while (1);
  }

  delay(1000);

  Serial.print("Accelerometer sample rate: ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  Serial.println();
  Serial.println("time_ms,ax,ay,az");  // CSV header
}

void loop() {
  float ax, ay, az; // accelerometer readings (g)

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);

    Serial.print(millis()); Serial.print(",");
    Serial.print(ax, 6);    Serial.print(",");
    Serial.print(ay, 6);    Serial.print(",");
    Serial.println(az, 6); 
  }

  delay(20); // ~50 Hz sample rate
}
