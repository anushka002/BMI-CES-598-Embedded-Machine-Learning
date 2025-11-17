#include <Arduino_BMI270_BMM150.h>

String label = "prone";    // supine , prone, side_left, side_right, sitting_up, sitting_down, unknown -> CHANGE THIS BEFORE EACH RECORDING SESSION

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.println("timestamp,ax,ay,az,gx,gy,gz,mx,my,mz,label");  // CSV header
}

void loop() {
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;

  if (IMU.accelerationAvailable() &&
      IMU.gyroscopeAvailable() &&
      IMU.magneticFieldAvailable()) {

    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);
    IMU.readMagneticField(mx, my, mz);

    unsigned long t = millis();

    Serial.print(t); Serial.print(",");
    Serial.print(ax); Serial.print(",");
    Serial.print(ay); Serial.print(",");
    Serial.print(az); Serial.print(",");
    Serial.print(gx); Serial.print(",");
    Serial.print(gy); Serial.print(",");
    Serial.print(gz); Serial.print(",");
    Serial.print(mx); Serial.print(",");
    Serial.print(my); Serial.print(",");
    Serial.print(mz); Serial.print(",");
    Serial.println(label);
  }

  delay(10);   // 100 Hz sampling
}
