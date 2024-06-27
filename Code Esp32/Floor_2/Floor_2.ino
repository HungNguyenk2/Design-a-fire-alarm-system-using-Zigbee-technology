#include <SoftwareSerial.h>
#include <string.h>

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
    ZigbeeSerial.println(send_data);
}

void NhanThongBaoChay() {
    if (ZigbeeSerial.available() > 0) {
        // Đọc dữ liệu từ tầng 1
        String data = ZigbeeSerial.readStringUntil('*');
        Serial.println("Co chay tu tang 1");
        Serial.println(data);

        // Gửi ACk tới 
        if (data == "From_1" || data == "From_3") {
            delay(100);
            ZigbeeSerial.println("Ack");
            // Gửi dữ liệu đi
            ZigbeeSerial.println(data);
            batchuong();
        }
    }
}

// Output functions
void batchuong() {
    digitalWrite(Batchuong, HIGH);
    digitalWrite(Xanuoc, LOW);
}


void checkfire(int fire) {
    Serial.print("Gia tri lua: ");
    Serial.println(fire);
    if (fire > 500) {
       TruyenThongBaoChay_For_From2();
       batchuong();
    } 
}

void loop() {
    NhanThongBaoChay();
    int fireValue = analogRead(A0); // Assuming A1 is used for the fire sensor
    checkfire(fireValue);
    delay(1000);
}