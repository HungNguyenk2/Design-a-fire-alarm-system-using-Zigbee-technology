#include <DHT.h>
#include <esp_sleep.h>

#define DHTPIN 33  // Chân tín hiệu của cảm biến nhiệt độ
#define A0 32      // Chân tín hiệu của cảm biến khói
#define A1 35      // Chân tín hiệu của cảm biến lửa
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define Baochuong 26 // Chân điều khiển báo chuông
#define Xanuoc 27    // Chân điều khiển xả nước

#define uS_TO_S_FACTOR 1000000ULL  // Hệ số chuyển đổi từ micro giây sang giây

int dem_lua = 0; 
int dem_khoi = 0;
int count = 0;

void setup() {
    Serial.begin(9600);
    dht.begin();
    pinMode(Baochuong, OUTPUT);
    pinMode(Xanuoc, OUTPUT);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    delay(1000);
    Serial.println("ESP32 Awake!");
}
// Hàm điều khiển output

void baochuong() {
    digitalWrite(Baochuong, HIGH);
    digitalWrite(Xanuoc, LOW);
}

void xanuoc() {
    digitalWrite(Baochuong, HIGH);
    digitalWrite(Xanuoc, HIGH);
}

void offdevice() {
    digitalWrite(Baochuong, LOW);
    digitalWrite(Xanuoc, LOW);
}

void loop() {
    float temp = dht.readTemperature();
    int smoke = analogRead(A0);
    int fire = analogRead(A1);

    if (smoke > 5000 || temp > 35 || fire > 3000) {
        if (smoke > 5000) {
            Serial.println(smoke);
            checksmoke(smoke);
        }
        if (temp > 35) {
            Serial.println(temp);
            checknhiet(temp);
        }
        if (fire > 3000) {
            Serial.println(fire);
            checkfire(fire);
        }
    } 
        else {
        count++;
        Serial.println("No problem");
        if (count >= 5) {
            count = 0;
            goToSleep(5); // Deep sleep for 10 seconds
        }
    }
    delay(1000); // Độ trễ để tránh quá tải CPU
}

// Hàm kiểm tra khói
void checksmoke(int smoke) {
    delay(1000);
    Serial.print("Gia tri khoi: ");
    Serial.println(smoke);
    Serial.print("Giá trị bộ đếm khói: ");
    Serial.println(dem_khoi);
    if (smoke > 5000) {
        count=0;
        dem_khoi++;
        if (dem_khoi > 5) {
            xanuoc();
            while (analogRead(A0) > 5000) {
                delay(1000);
                Serial.print("Gia tri khoi: ");
                Serial.println(analogRead(A0));
                xanuoc();
            }
            offdevice();
            dem_khoi = 0;
            dem_lua = 0;
            
        } else {
            baochuong();
            checknhiet(dht.readTemperature());
        }

    } else {
        count++;
        offdevice();
        dem_khoi = 0;
      if (count >= 5) {
      count = 0;
      goToSleep(5);
    }else{
      checkfire(analogRead(A1));
    }
  }

}

// Hàm kiểm tra nhiệt độ
void checknhiet(float temp) {
    delay(2000);
    Serial.print("Gia tri nhiet: ");
    Serial.println(temp);

    if (isnan(temp)) {
        Serial.println("Failed to read from DHT sensor!");
    }
    if (temp > 35) {
        count=0;
        xanuoc();
        dem_khoi = 0;
        dem_lua = 0;
        checknhiet(dht.readTemperature());
    } else{
      checksmoke(analogRead(A0));
    }
}

// Hàm kiểm tra lửa
void checkfire(int fire) {
    delay(1000);
    Serial.print("Gia tri lua: ");
    Serial.println(fire);
    if (fire > 3000) {
        count=0;
        Serial.print("Gia trị bộ đếm lửa: ");
        Serial.println(dem_lua);
        dem_lua++;
        if (dem_lua > 5) {
            baochuong();
            dem_lua = 0;
            dem_khoi = 0;
            while (analogRead(A1) > 3000) {
                delay(1000);
                Serial.print("Gia tri lua: ");
                Serial.println(analogRead(A1));
                xanuoc();
            }
            offdevice();
            checknhiet(dht.readTemperature());
        }else{
          checknhiet(dht.readTemperature());
        }
    } else {
        dem_lua = 0;
        offdevice();
        checksmoke(analogRead(A0));
    }
}

// Hàm để đưa ESP32 vào chế độ deep sleep
void goToSleep(int sleep_time_sec) {
    Serial.println("Going to sleep now");
    esp_sleep_enable_timer_wakeup(sleep_time_sec * uS_TO_S_FACTOR);
    Serial.flush();
    esp_deep_sleep_start();
}
