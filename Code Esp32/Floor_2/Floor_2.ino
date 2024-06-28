#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial ZigbeeSerial(11, 10); // Rx :11, Tx :10

#define Batchuong 7
#define Xanuoc 8

// Setup cổng A0 đọc tín hiệu khói
// Setup cổng A1 đọc tín hiệu lửa (sensor ánh sáng)

String data_2 = "From_2";

void setup() {
    ZigbeeSerial.begin(9600);
    Serial.begin(9600);
    pinMode(Batchuong, OUTPUT);
    pinMode(Xanuoc, OUTPUT);
} 

void TruyenThongBaoChay_For_From2() {
    String send_data = data_2 + "*";
    ZigbeeSerial.print(send_data);
}

void NhanThongBaoChay() {
    if (ZigbeeSerial.available() > 0) {
        // Đọc dữ liệu từ tầng 1
        String data = ZigbeeSerial.readStringUntil('*');
        Serial.print("Co_chay ");
        Serial.println(data);

        // Gửi ACk tới 
        if (data == "From_1") {
            ZigbeeSerial.print("Ack");
            delay(3000);
            // Gửi dữ liệu đi
            ZigbeeSerial.print(data);
            batchuong();
        }
        if (data == "From_3") {
            ZigbeeSerial.print("Ack");
            delay(3000);
            // Gửi dữ liệu đi
            ZigbeeSerial.print(data);
            batchuong();
        }
    }else{
      offdevice();
    }
    delay(100);
}

// Output functions
void batchuong() {
    digitalWrite(Batchuong, HIGH);
    digitalWrite(Xanuoc, LOW);
    delay(1000);
}
void offdevice() {
    digitalWrite(Batchuong, LOW);
    digitalWrite(Xanuoc, LOW);
}

void checkfire(int fire) {
    Serial.print("Gia tri lua: ");
    Serial.println(fire);
    if (fire < 210) {
       TruyenThongBaoChay_For_From2();
       batchuong();
    }else{
      offdevice();
    } 
}

void loop() {
    NhanThongBaoChay();
    delay(1000);
    int fireValue = analogRead(A0); // Assuming A1 is used for the fire sensor
    checkfire(fireValue);
}