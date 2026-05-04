#include <Wire.h>
#include <ICM20602.h>

ICM20602 icm;

// Scale factors (phải khớp với set range)
float accelScale = 8192.0;   // ±4g
float gyroScale  = 65.5;     // ±500 dps

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== ICM20602 I2C TEST ===");

  if (!icm.begin(21, 22)) {
    Serial.println("❌ ICM20602 NOT FOUND");
    while (1);
  }

  Serial.println("✅ ICM20602 Connected");

  // Cấu hình sensor
  icm.setAccelRange(4);      // ±4g
  icm.setGyroRange(500);     // ±500 dps
  icm.setDLPF(3);            // Bandwidth ~44Hz
  icm.setSampleRate(500);    // 500Hz

  Serial.println("Sensor Configured");
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  icm.readRaw(ax, ay, az, gx, gy, gz);

  // Chuyển sang đơn vị vật lý
  float ax_g = ax / accelScale;
  float ay_g = ay / accelScale;
  float az_g = az / accelScale;

  float gx_dps = gx / gyroScale;
  float gy_dps = gy / gyroScale;
  float gz_dps = gz / gyroScale;

  Serial.print("ACC (g): ");
  Serial.print(ax_g, 3); Serial.print("  ");
  Serial.print(ay_g, 3); Serial.print("  ");
  Serial.print(az_g, 3);

  Serial.print(" | GYRO (dps): ");
  Serial.print(gx_dps, 2); Serial.print("  ");
  Serial.print(gy_dps, 2); Serial.print("  ");
  Serial.println(gz_dps, 2);

  delay(20);
}
