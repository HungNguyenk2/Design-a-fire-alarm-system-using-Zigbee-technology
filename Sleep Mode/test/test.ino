#define BLYNK_TEMPLATE_ID "TMPL69m0tG7kg"
#define BLYNK_TEMPLATE_NAME "Project TKHT"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <string.h>



// DHT define

#define A1 35


// Define LED
#define LED 32
WidgetLED LED_ON_APP(V2);
int button;

char auth[] = "09b_f0OBfHzFsh19WrAER1ju0_HPIemx";
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "snn";
char pass[] = "namnamnam";

void setup()
{
  // Debug console
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  pinMode(A1, INPUT);
  Blynk.begin(auth, ssid, pass);
  delay(1000);
}

BLYNK_WRITE(V1) {
  button = param.asInt();
  if(button == 1) {
    digitalWrite(LED, HIGH);
    LED_ON_APP.on();
  } else {
    digitalWrite(LED, LOW);
    LED_ON_APP.off();
  }
}

void loop()
{
  Blynk.run();
  // Read Temp
  int t = analogRead(A1);

  // Check isRead ?
  if (isnan(t)) {
    delay(500);
    Serial.println("Failed to read from DHT sensor!\n");
    return;
  }

  Blynk.virtualWrite(V0,t);

  Serial.print("Nhiet do: ");
  Serial.println(t);
  delay(2000);
}