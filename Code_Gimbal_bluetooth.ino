#include <SPI.h>
#include <ESP32Servo.h>
#include <Kalman.h> 
#include "ICM20602.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// --- CẤU HÌNH PHẦN CỨNG ---
const int SPI_SCLK = 18, SPI_MISO = 19, SPI_MOSI = 23, ICM_CS = 5;
const int ROLL_PIN = 21, PITCH_PIN = 22;

ICM20602 icm(SPI, ICM_CS);
Kalman kalRoll, kalPitch;
Servo sRoll, sPitch;

// --- HỆ SỐ ĐIỀU KHIỂN TỐI ƯU ---
volatile float Kp = 0.25;      // Hệ số tỉ lệ (Phản ứng nhanh)
volatile float Kd = 0.00;     // Hệ số đạo hàm (Giảm rung/phanh)
volatile float deadband = 2.5; // Tăng giảm vùng chết để điều chỉnh độ nhạy hơn
// --- CẤU HÌNH CÁC CHẾ ĐỘ HOẠT ĐỘNG CỦA GIMBAL ---
// Mode 2: Chế độ Theo dõi (Follow) - Gimbal xoay chậm theo chuyển động của tay cầm.
// Mode 1: Chế độ Khóa (Locked) - Gimbal giữ cố định góc bất kể chuyển động của khung.
// Mode 0: Chế độ Cân bằng (Manual) - Tương tự chế độ theo dõi nhưng chuyển động nhanh và không mềm mại bằng
volatile int gimbalMode = 0; // Mặc định bắt đầu ở chế độ cân bằng

float currentServoPitch = 90.0, currentServoRoll = 90.0;
TaskHandle_t TaskGimbal;

// --- HÀM XỬ LÝ LỆNH BLUETOOTH ---
void handleBluetooth() {
  if (SerialBT.available()) {
    String data = SerialBT.readStringUntil('\n');
    data.trim();
    char cmd = data[0];
    float val = data.substring(1).toFloat();

    switch (cmd) {
      case 'M': gimbalMode = (int)val; break;
      case 'P': Kp = val; break;  // Chỉnh Kp (VD: P0.8)
      case 'D': Kd = val; break;  // Chỉnh Kd (VD: D0.05)
      case 'B': deadband = val; break;   // Chỉnh vùng chết (VD: B1.5)
      case 'S':
        SerialBT.printf("\nKp:%.2f | Kd:%.3f | Mode:%d\n", Kp, Kd, gimbalMode);
        break;
    }
  }
}

// --- TASK LOGIC GIMBAL (CORE 1) ---
void GimbalLogic(void * pvParameters) {
  uint32_t timer = micros();
  for(;;) {
    uint32_t now = micros();
    double dt = (double)(now - timer) / 1000000.0;
    timer = now;

    float ax, ay, az, gx, gy, gz;
    icm.readScaled(ax, ay, az, gx, gy, gz);

    // Tính toán góc Kalman
    float angleRoll  = kalRoll.getAngle(atan2(ay, az) * 57.296, gx, dt);
    float anglePitch = kalPitch.getAngle(atan2(-ax, sqrt(ay * ay + az * az)) * 57.296, gy, dt);

    if (gimbalMode == 0 || gimbalMode == 2) { 
      
        if (abs(anglePitch) > deadband) {
            float targetP = anglePitch * Kp - (gy * Kd);
            currentServoPitch -= targetP;
        }
        if (abs(angleRoll) > deadband) {
            float targetR = angleRoll * Kp - (gx * Kd);
            currentServoRoll -= targetR;
        }
    } else if (gimbalMode == 1) { 
        currentServoPitch += (90 - currentServoPitch) * 0.1; 
        currentServoRoll += (90 - currentServoRoll) * 0.1;
    }

    currentServoPitch = constrain(currentServoPitch, 10, 170);
    currentServoRoll  = constrain(currentServoRoll, 10, 170);

    sRoll.write((int)currentServoRoll);
    sPitch.write((int)currentServoPitch);

    vTaskDelay(2 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Gimbal_Huy_HighSpeed"); 

  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  sRoll.setPeriodHertz(100);
  sPitch.setPeriodHertz(100);
  sRoll.attach(ROLL_PIN, 500, 2400);
  sPitch.attach(PITCH_PIN, 500, 2400);

  SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, ICM_CS);
  if(!icm.begin()){
    while(1); 
  }
  
  icm.setAccelRange(4); 
  icm.setGyroRange(500); 
  icm.setDLPF(0); 

  // Ưu tiên Task cao nhất (Priority 2)
  xTaskCreatePinnedToCore(GimbalLogic, "GimbalTask", 4096, NULL, 2, &TaskGimbal, 1);
}

void loop() {
  handleBluetooth();
  delay(10); 
}
