#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial ZigbeeSerial(11, 10); // Rx :11, Tx :10

#define Batchuong 7
#define Xanuoc 8

// Setup cổng A0 đọc tín hiệu khói
// Setup cổng A1 đọc tín hiệu lửa (sensor ánh sáng)

String data_3 = "From_3";

void setup() {
    ZigbeeSerial.begin(9600);
    Serial.begin(9600);
    pinMode(Batchuong, OUTPUT);
    pinMode(Xanuoc, OUTPUT);
} 

void TruyenThongBaoChay(){

  // gửi dữ liệu đến tầng 2
	String send_data = data_3 + "*";
	ZigbeeSerial.print(send_data);
  Serial.println("Message sent to floor 2");

  // Đợi phản hồi từ tầng 2
  unsigned long startTime = millis();
  bool receiverAck =  false ;

  while((millis() - startTime)< 10000 ){ // wate 3 seconds
    if (ZigbeeSerial.available() > 0) {
      String Ack = ZigbeeSerial.readStringUntil('*');
      if( Ack = "Ack"){
        receiverAck = true;
        break;
      }
    }
  }

  if (!receiverAck){
    // Nếu không nhạn được ACK từ tầng 2 , gửi trực tiếp đến tầng 1
    Serial.println ("No Ack from floor , sending to floor 1");
    ZigbeeSerial.print(send_data);
  }else{
    Serial.println("Received from 2 succsed");
  }
  delay(100);
}

void batchuong() {
    digitalWrite(Batchuong, HIGH);
    digitalWrite(Xanuoc, LOW);
    // delay(1000);
}
void offdevice() {
    digitalWrite(Batchuong, LOW);
    digitalWrite(Xanuoc, LOW);
}
void NhanThongBaoChay() {
	String i;
	i = ZigbeeSerial.readStringUntil('*'); //  i="From_2_To_1" or "From_3_To_1";
	if((i == "From_2")||(i == "From_1")){
    Serial.print(i);
    Serial.println(" Co chay nhe  ");
		batchuong();
	}else{
    offdevice();
  }

}

// Output functions


void checkfire(int fire) {
    Serial.print("Gia tri lua: ");
    Serial.println(fire);
    if (fire < 700) {
       TruyenThongBaoChay();
       batchuong();
    }else{
      offdevice();
    }
}

void loop() {
    NhanThongBaoChay();
    int fireValue = analogRead(A1); // Assuming A1 is used for the fire sensor
    checkfire(fireValue);
    delay(100);
}