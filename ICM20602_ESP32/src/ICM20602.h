#ifndef _ICM20602_H_
#define _ICM20602_H_

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h> // [Thêm] Thư viện SPI

#define DEG_TO_RAD 0.01745329251f
#define G_TO_MS2   9.80665f

#define ICM20602_ADDR 0x68

// Register map
#define REG_SMPLRT_DIV     0x19
#define REG_CONFIG         0x1A
#define REG_GYRO_CONFIG    0x1B
#define REG_ACCEL_CONFIG   0x1C
#define REG_ACCEL_CONFIG2  0x1D
#define REG_INT_ENABLE     0x38
#define REG_INT_STATUS     0x3A
#define REG_ACCEL_XOUT_H   0x3B
#define REG_PWR_MGMT_1     0x6B
#define REG_WHO_AM_I       0x75
#define REG_USER_CTRL      0x6A // [Thêm] Cần thiết để reset I2C/SPI mode

class ICM20602 {
public:
    // Constructor cho I2C
    ICM20602(TwoWire &w = Wire);

    // [Thêm] Constructor cho SPI
    ICM20602(SPIClass &s, int csPin);

    // Hàm begin cho I2C
    bool begin(uint8_t sda, uint8_t scl, uint32_t freq = 400000);
    // Hàm begin chung (tự động nhận mode)
    bool begin();

    void setAccelRange(uint8_t g);
    void setGyroRange(uint16_t dps);
    void setDLPF(uint8_t cfg);
    void setSampleRate(uint16_t rate);

    bool dataReady();

    void readRaw(int16_t &ax, int16_t &ay, int16_t &az,
                 int16_t &gx, int16_t &gy, int16_t &gz);

    void readScaled(float &ax, float &ay, float &az,
                    float &gx, float &gy, float &gz);

private:
    TwoWire* _wire;
    SPIClass* _spi;      // [Thêm] Con trỏ SPI
    int _cs;             // [Thêm] Chân CS
    bool _useSPI;        // [Thêm] Cờ đánh dấu đang dùng SPI hay I2C

    float accelScale;
    float gyroScale;

    // Các hàm giao tiếp cấp thấp (Low-level)
    void writeReg(uint8_t reg, uint8_t data);
    uint8_t readReg(uint8_t reg);
    void burstRead(uint8_t reg, uint8_t* buffer, uint8_t len);
};

#endif
