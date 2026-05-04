#include "ICM20602.h"

// --- Constructor cho I2C (Mặc định) ---
ICM20602::ICM20602(TwoWire &w) {
    _wire = &w;
    _spi = NULL;
    _useSPI = false;
    _cs = -1;
    accelScale = 8192.0; // Mặc định 4g
    gyroScale  = 65.5;   // Mặc định 500dps
}

// --- Constructor cho SPI ---
ICM20602::ICM20602(SPIClass &s, int csPin) {
    _wire = NULL;
    _spi = &s;
    _useSPI = true;
    _cs = csPin;
    accelScale = 8192.0;
    gyroScale  = 65.5;
}

// --- Khởi tạo cho I2C ---
bool ICM20602::begin(uint8_t sda, uint8_t scl, uint32_t freq) {
    if (_useSPI) return false;
    _wire->begin(sda, scl);
    _wire->setClock(freq); // Sử dụng tham số freq thay vì ghi cứng 400000
    return begin();
}

// --- Hàm khởi tạo chung (Cấu hình thanh ghi) ---
bool ICM20602::begin() {
    if (_useSPI) {
        pinMode(_cs, OUTPUT);
        digitalWrite(_cs, HIGH);
        // Không gọi _spi->begin() ở đây để người dùng tự init bus SPI trong main
    }
    
    delay(50);

    // 1. Soft Reset
    writeReg(REG_PWR_MGMT_1, 0x80);
    delay(100);

    // 2. Chế độ SPI: Vô hiệu hóa I2C interface để đảm bảo ổn định
    if (_useSPI) {
        // Thanh ghi USER_CTRL (0x6A) bit I2C_IF_DIS = 1
        writeReg(0x6A, 0x10); 
    }

    // 3. Chọn nguồn xung (Clock Source)
    writeReg(REG_PWR_MGMT_1, 0x01); // Auto select hoặc Gyro X
    delay(10);

    // 4. Kiểm tra WHO_AM_I
    if (readReg(REG_WHO_AM_I) != 0x12) {
        return false;
    }

    // 5. Cấu hình mặc định cho xe cân bằng
    setAccelRange(4);
    setGyroRange(500);
    setDLPF(3);
    setSampleRate(500);

    // Cho phép ngắt khi có dữ liệu mới (tùy chọn)
    writeReg(REG_INT_ENABLE, 0x01);

    return true;
}

// --- Giao tiếp ghi thanh ghi ---
void ICM20602::writeReg(uint8_t reg, uint8_t data) {
    if (_useSPI) {
        _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
        digitalWrite(_cs, LOW);
        _spi->transfer(reg & 0x7F); // Bit 0 là Write (0)
        _spi->transfer(data);
        digitalWrite(_cs, HIGH);
        _spi->endTransaction();
    } else {
        _wire->beginTransmission(ICM20602_ADDR);
        _wire->write(reg);
        _wire->write(data);
        _wire->endTransmission();
    }
}

// --- Giao tiếp đọc thanh ghi ---
uint8_t ICM20602::readReg(uint8_t reg) {
    uint8_t val = 0xFF;
    if (_useSPI) {
        _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
        digitalWrite(_cs, LOW);
        _spi->transfer(reg | 0x80); // Bit 0 là Read (1)
        val = _spi->transfer(0x00);
        digitalWrite(_cs, HIGH);
        _spi->endTransaction();
    } else {
        _wire->beginTransmission(ICM20602_ADDR);
        _wire->write(reg);
        _wire->endTransmission(false);
        _wire->requestFrom(ICM20602_ADDR, (uint8_t)1);
        val = _wire->read();
    }
    return val;
}

// --- Đọc Burst (Nhiều byte liên tục) ---
void ICM20602::burstRead(uint8_t reg, uint8_t* buffer, uint8_t len) {
    if (_useSPI) {
        _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
        digitalWrite(_cs, LOW);
        _spi->transfer(reg | 0x80);
        for (uint8_t i = 0; i < len; i++) {
            buffer[i] = _spi->transfer(0x00);
        }
        digitalWrite(_cs, HIGH);
        _spi->endTransaction();
    } else {
        _wire->beginTransmission(ICM20602_ADDR);
        _wire->write(reg);
        _wire->endTransmission(false);
        _wire->requestFrom(ICM20602_ADDR, len);
        uint8_t i = 0;
        while (_wire->available() && i < len) {
            buffer[i++] = _wire->read();
        }
    }
}

// --- Cài đặt dải đo Accelerometer ---
void ICM20602::setAccelRange(uint8_t g) {
    uint8_t cfg = 0;
    switch (g) {
        case 2:  cfg = 0x00; accelScale = 16384.0; break;
        case 4:  cfg = 0x08; accelScale = 8192.0;  break;
        case 8:  cfg = 0x10; accelScale = 4096.0;  break;
        case 16: cfg = 0x18; accelScale = 2048.0;  break;
        default: cfg = 0x08; accelScale = 8192.0;  break;
    }
    writeReg(REG_ACCEL_CONFIG, cfg);
}

// --- Cài đặt dải đo Gyroscope ---
void ICM20602::setGyroRange(uint16_t dps) {
    uint8_t cfg = 0;
    switch (dps) {
        case 250:  cfg = 0x00; gyroScale = 131.0; break;
        case 500:  cfg = 0x08; gyroScale = 65.5;  break;
        case 1000: cfg = 0x10; gyroScale = 32.8;  break;
        case 2000: cfg = 0x18; gyroScale = 16.4;  break;
        default:   cfg = 0x08; gyroScale = 65.5;  break;
    }
    writeReg(REG_GYRO_CONFIG, cfg);
}

// --- Bộ lọc thông thấp kỹ thuật số (DLPF) ---
void ICM20602::setDLPF(uint8_t cfg) {
    if (cfg > 6) cfg = 3;
    writeReg(REG_CONFIG, cfg);
    writeReg(REG_ACCEL_CONFIG2, cfg);
}

// --- Tần số lấy mẫu ---
void ICM20602::setSampleRate(uint16_t rate) {
    if (rate == 0) rate = 500;
    if (rate > 1000) rate = 1000;
    uint16_t divider = 1000 / rate - 1;
    writeReg(REG_SMPLRT_DIV, divider);
}

bool ICM20602::dataReady() {
    return (readReg(REG_INT_STATUS) & 0x01);
}

// --- Đọc dữ liệu thô ---
void ICM20602::readRaw(int16_t &ax, int16_t &ay, int16_t &az,
                       int16_t &gx, int16_t &gy, int16_t &gz) {
    uint8_t buffer[14];
    burstRead(REG_ACCEL_XOUT_H, buffer, 14);

    ax = (int16_t)(buffer[0] << 8 | buffer[1]);
    ay = (int16_t)(buffer[2] << 8 | buffer[3]);
    az = (int16_t)(buffer[4] << 8 | buffer[5]);
    // buffer[6,7] là nhiệt độ (skip)
    gx = (int16_t)(buffer[8] << 8 | buffer[9]);
    gy = (int16_t)(buffer[10] << 8 | buffer[11]);
    gz = (int16_t)(buffer[12] << 8 | buffer[13]);
}

// --- Đọc dữ liệu đã quy đổi đơn vị ---
void ICM20602::readScaled(float &ax, float &ay, float &az,
                          float &gx, float &gy, float &gz) {
    int16_t rax, ray, raz, rgx, rgy, rgz;
    readRaw(rax, ray, raz, rgx, rgy, rgz);

    ax = (float)rax / accelScale;
    ay = (float)ray / accelScale;
    az = (float)raz / accelScale;

    gx = (float)rgx / gyroScale;
    gy = (float)rgy / gyroScale;
    gz = (float)rgz / gyroScale;
}