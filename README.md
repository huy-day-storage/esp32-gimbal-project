# 🚀 Dự Án Gimbal 2 Trục - ESP32 & ICM20602

Chào mừng bạn đến với dự án Gimbal 2 trục sử dụng vi điều khiển **ESP32** và cảm biến **ICM20602**. Dự án này được thiết kế nhằm mô phỏng một thiết bị ổn định camera/điện thoại một cách mượt mà và nhanh chóng bằng thuật toán tối ưu.

## 🛠 Linh Kiện Cần Thiết
* **Vi điều khiển:** ESP32 (S3 hoặc bản thường).
* **Cảm biến:** ICM20602 (Giao tiếp SPI/I2C).
* **Động cơ:** 2 Servo MG996R.
* **Nguồn:** Pin Li-ion hoặc nguồn 5V ổn định.

## 📋 Hướng Dẫn Cài Đặt
Để nạp code và chạy dự án, bạn làm theo các bước sau:

1. **Tải mã nguồn:** Nhấn nút **Code** -> **Download ZIP** trên GitHub và giải nén trên máy tính.
2. **Cài đặt thư viện:** 
   * Sao chép thư mục `ICM20602_ESP32` vào thư mục `libraries` của phần mềm Arduino (Thường nằm ở: `Documents/Arduino/libraries`).
3. **Nạp code:**
   * Mở file `Code_Gimbal_bluetooth.ino` bằng Arduino IDE.
   * Chọn đúng **Board ESP32** của bạn.
   * Kiểm tra lại cổng COM và nhấn **Upload**.

## 🔌 Sơ Đồ Đấu Nối (Gợi ý)
* **ICM20602:** 
    * SCK -> Pin 18
    * MISO -> Pin 19
    * MOSI -> Pin 23
    * CS -> Pin 5
* **Servo:**
    * Pitch -> Pin 22
    * Roll -> Pin 21

## 📱 Kết Nối Bluetooth
Dự án có hỗ trợ điều khiển và tinh chỉnh thông số qua Bluetooth. Bạn có thể sử dụng các ứng dụng Serial Bluetooth Terminal trên điện thoại để gửi lệnh điều chỉnh Kp, Kd trực tiếp.

