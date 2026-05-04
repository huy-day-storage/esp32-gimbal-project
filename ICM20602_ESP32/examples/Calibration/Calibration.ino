#include <Wire.h>
#include <ICM20602.h>

ICM20602 icm;

#define CALIB_SAMPLES 2000

float accelScale = 8192.0;   // ±4g
float gyroScale  = 65.5;     // ±500 dps

float ax_offset = 0;
float ay_offset = 0;
float az_offset = 0;
float gx_offset = 0;
float gy_offset = 0;
float gz_offset = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== ICM20602 CALIBRATION ===");

  if (!icm.begin(21, 22)) {
    Serial.println("❌ ICM20602 NOT FOUND");
    while (1);
  }

  icm.setAccelRange(4);
  icm.setGyroRange(500);
  icm.setDLPF(3);
  icm.setSampleRate(500);

  Serial.println("⚠️  KEEP SENSOR STILL...");
  delay(3000);

  long ax_sum = 0, ay_sum = 0, az_sum = 0;
  long gx_sum = 0, gy_sum = 0, gz_sum = 0;

  for (int i = 0; i < CALIB_SAMPLES; i++) {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    icm.readRaw(ax, ay, az, gx, gy, gz);

    ax_sum += ax;
    ay_sum += ay;
    az_sum += az;

    gx_sum += gx;
    gy_sum += gy;
    gz_sum += gz;

    delay(2);
  }

  ax_offset = (float)ax_sum / CALIB_SAMPLES;
  ay_offset = (float)ay_sum / CALIB_SAMPLES;
  az_offset = ((float)az_sum / CALIB_SAMPLES) - accelScale; // trừ 1g

  gx_offset = (float)gx_sum / CALIB_SAMPLES;
  gy_offset = (float)gy_sum / CALIB_SAMPLES;
  gz_offset = (float)gz_sum / CALIB_SAMPLES;

  Serial.println("=== CALIBRATION DONE ===");

  Serial.println("Copy these offsets to your main code:");
  Serial.print("ax_offset = "); Serial.println(ax_offset);
  Serial.print("ay_offset = "); Serial.println(ay_offset);
  Serial.print("az_offset = "); Serial.println(az_offset);
  Serial.print("gx_offset = "); Serial.println(gx_offset);
  Serial.print("gy_offset = "); Serial.println(gy_offset);
  Serial.print("gz_offset = "); Serial.println(gz_offset);
}

void loop() {
  // Không làm gì
}
