#include <SPI.h>
#include <ICM20602.h>

#define CS_PIN 5  // Chân CS chọn trong README

// Khai báo dùng SPI
ICM20602 icm(SPI, CS_PIN);

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo SPI bus trước
  SPI.begin(18, 19, 23, CS_PIN); // SCK, MISO, MOSI, CS

  // Khởi tạo cảm biến
  if (!icm.begin()) {
    Serial.println("Failed to init SPI ICM20602");
    while(1);
  }
  
  Serial.println("ICM20602 SPI Ready!");
}