#define BLYNK_TEMPLATE_ID "TMPL69m0tG7kg"
#define BLYNK_TEMPLATE_NAME "Project TKHT"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h> 
#include <DHT.h>
#include <HardwareSerial.h>
#include <string.h>
#include <Wire.h>
#include <esp_sleep.h>

#define uS_TO_S_FACTOR 1000000ULL  // Hệ số chuyển đổi từ micro giây sang giây
#define DHTPIN 33  // Chân tín hiệu của cảm biến nhiệt độ
#define A0 32      // Chân tín hiệu của cảm biến khói
#define A1 35      // Chân tín hiệu của cảm biến lửa
#define DHTTYPE DHT11
#define LUA 3200
#define KHOI 3500
#define NHIET 33.5 

DHT dht(DHTPIN, DHTTYPE);
 
#define Baochuong 26 // Chân điều khiển báo chuông
#define Xanuoc 27    // Chân điều khiển xả nước

char auth[] = "09b_f0OBfHzFsh19WrAER1ju0_HPIemx";
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "snn";
char pass[] = "namnamnam";

int count = 0;
int dem_lua = 0; 
int dem_khoi = 0;

HardwareSerial ZigbeeSerial (2);  // 16 : Rx 17 :Tx   của ESP32 

void setup() {
    ZigbeeSerial.begin(9600);
    Serial.begin(9600);
    dht.begin();
    pinMode(Baochuong, OUTPUT);
    pinMode(Xanuoc, OUTPUT);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    Blynk.begin(auth, ssid, pass);
    Serial.println("ESP32 Awake!");
    delay(1000);
} 
String data = "From_1";
// Hàm điều khiển output

void baochuong() {
    digitalWrite(Baochuong, HIGH);
    digitalWrite(Xanuoc, LOW);
    delay(1000);
}

void xanuoc() {
    digitalWrite(Baochuong, HIGH);
    digitalWrite(Xanuoc, HIGH);
    delay(1000);
}

void offdevice() {
    digitalWrite(Baochuong, LOW);
    digitalWrite(Xanuoc, LOW);
}

void TruyenThongBaoChay(){

  // gửi dữ liệu đến tầng 2
	String send_data = data + "*";
	ZigbeeSerial.print(send_data);
  Serial.println("Message sent to floor 2");

  // Đợi phản hồi từ tầng 2
  unsigned long startTime = millis();
  bool receiverAck =  false ;

  while((millis() - startTime)< 5000 ){ // wate 5 seconds
    if (ZigbeeSerial.available() > 0) {
      String Ack = ZigbeeSerial.readStringUntil('*');
      if( Ack == "Ack"){
        receiverAck = true;
        break;
      }
    }
  }

  
  if (!receiverAck){
    // Nếu không nhạn được ACK từ tầng 2 , gửi trực tiếp đến tầng 3
    Serial.println ("No Ack from floor , sending to floor 3");
    ZigbeeSerial.print(send_data);
  }
  delay(100);
}


void NhanThongBaoChay() {
	String i;
	i = ZigbeeSerial.readStringUntil('*'); //  i="From_2_To_1" or "From_3_To_1";
	if((i == "From_2")||(i == "From_3")){
    count=0;
    Serial.println(i);
    Serial.println(" Co chay nhe  ");
		baochuong();
	}	
  delay(100);
}

// Hàm kiểm tra khói
void checksmoke(int smoke) {
    Blynk.run();
    Blynk.virtualWrite(V1,smoke);
    NhanThongBaoChay();
    Serial.print("Gia tri khoi: ");
    Serial.println(smoke);    
    Serial.print("Giá trị bộ đếm khói: ");
    Serial.println(dem_khoi);
    if (smoke < KHOI) {   
        count=0;
        dem_khoi++;
        Blynk.virtualWrite(V3,"Floor 1 - Has Problem !!!");
        if (dem_khoi > 5) {  
            xanuoc();
            while (analogRead(A0) < KHOI) { 
                Blynk.virtualWrite(V0,dht.readTemperature());
                Blynk.virtualWrite(V1,analogRead(A0));
                Blynk.virtualWrite(V2,analogRead(A1));
                Blynk.virtualWrite(V3,"Floor 1 - Fireee !!!");
                TruyenThongBaoChay();
                Serial.print("Gia tri khoi: ");
                Serial.println(analogRead(A0)); 
                xanuoc();
                delay(1000);
            }
            offdevice();
            dem_khoi = 0;
            dem_lua  = 0;
            checkfire(analogRead(A1));
        } else {
            baochuong();
            checknhiet(dht.readTemperature());
        }

    }else {
        if(analogRead(A1)<LUA){
        Blynk.virtualWrite(V3,"Floor 1 - Stability!!!");
        }
        count++;
        offdevice();
        dem_khoi = 0;
      if (count >= 5) {
        count = 0;
        goToSleep(5);
      }else{
        delay(1000);
        checkfire(analogRead(A1));
    }
  }
}

// Hàm kiểm tra nhiệt độ
void checknhiet(float t) {
    Blynk.run();
    Blynk.virtualWrite(V0,t);
    NhanThongBaoChay(); 
    Serial.print("Gia tri Nhiet: ");
    Serial.println(t);
  // float h = dht.readTemperature();

  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }
    if (t > NHIET) { 
        Blynk.virtualWrite(V3,"Floor 1 - Fireee!!!");
        Blynk.virtualWrite(V0,dht.readTemperature());
        Blynk.virtualWrite(V1,analogRead(A0));
        Blynk.virtualWrite(V2,analogRead(A1));
        count=0;
        TruyenThongBaoChay();
        xanuoc();
        dem_khoi = 0;
        dem_lua  = 0;
        checknhiet(dht.readTemperature());
    } else {
        delay(1000);
        checksmoke(analogRead(A0));
    }
}

// Hàm kiểm tra lửa
void checkfire(int fire) {
    Blynk.run();
    Blynk.virtualWrite(V2,analogRead(A1));
    NhanThongBaoChay();
    Serial.print("Gia tri lua: ");
    Serial.println(fire);
    if (fire < LUA) {
        Blynk.virtualWrite(V3,"Floor 1 - Has Problem !!!");
        count=0;
        Serial.print("Gia trị bộ đếm lửa: ");
        Serial.println(dem_lua);
        dem_lua++;
        if (dem_lua >= 10) {
            baochuong();
            dem_lua = 0;
            dem_khoi = 0;
            while (analogRead(A1) < LUA) {
                Blynk.virtualWrite(V3,"Floor 1 - Fireee !!!");
                Blynk.virtualWrite(V0,dht.readTemperature());
                Blynk.virtualWrite(V1,analogRead(A0));
                Blynk.virtualWrite(V2,analogRead(A1));
                TruyenThongBaoChay();
                Serial.print("Gia tri Lua: ");
                Serial.println(analogRead(A1)); 
                xanuoc();
                delay(100);
            }
            offdevice();
            checknhiet(dht.readTemperature());
        } else {
            offdevice();
            checknhiet(dht.readTemperature());
        }

    } else {
        Blynk.virtualWrite(V3,"Floor 1 - Stability!!!");
        dem_lua = 0;
        offdevice();
        delay(1000);
        checksmoke(analogRead(A0));
    }

}
void goToSleep(int sleep_time_sec) {
    Serial.println("Going to sleep now");
    esp_sleep_enable_timer_wakeup(sleep_time_sec * uS_TO_S_FACTOR);
    Serial.flush();
    esp_deep_sleep_start();
}

void loop() {
    float temp = dht.readTemperature();
    int smoke = analogRead(A0);
    int fire = analogRead(A1);
    NhanThongBaoChay();
    Blynk.run();
    Blynk.virtualWrite(V3,"Floor 1 - Stability!!!");
    Blynk.virtualWrite(V0,temp);
    Blynk.virtualWrite(V1,smoke);
    Blynk.virtualWrite(V2,fire);

    if (smoke < KHOI || temp < NHIET || fire > LUA) {
        if (smoke > KHOI) {
            Serial.print("Gia tri Khoi: ");
            Serial.println(smoke);
            checksmoke(smoke);
        }
        if (temp > NHIET) {
            Serial.print("Gia tri Nhiet: ");
            Serial.println(temp);
            checknhiet(temp);
        }
        if (fire > LUA) {
            Serial.print("Gia tri Lua: ");
            Serial.println(fire);
            checkfire(fire);
        }
    } 
        else {
        count++;
        Blynk.virtualWrite(V3,"Floor 1 - Stability!!!");
        Serial.println("No problem");
        if (count >= 15) {
            count = 0;
            goToSleep(5); // Deep sleep for 10 seconds
        }
    }
    delay(100); // Độ trễ để tránh quá tải CPU
}
