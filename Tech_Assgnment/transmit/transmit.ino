#include <WiFi.h>
#include <HTTPClient.h>

// Definisikan pin trigger dan echo
#define TRIGGER_PIN  23 // GPIO23
#define ECHO_PIN    21 // GPIO21

const char* ssid = "sweethome";  
const char* password = "tanpapassword2023";  
const char* serverName = "http://192.168.0.109:5000/data";  

long durasi, jarak;
unsigned long timeout;

void setup() {
  // Inisialisasi komunikasi serial
  Serial.begin(115200);

  // Konfigurasikan pin trigger dan echo
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Koneksi ke Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Connected");
}

void baca_jarak(){
  digitalWrite(TRIGGER_PIN, LOW);  // Pastikan pin trigger LOW
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  durasi = pulseIn(ECHO_PIN, HIGH);
  jarak = (durasi / 2) / 29.1;  // Hitung jarak dalam cm
  Serial.print(jarak);
  Serial.println(" cm");
}

void kirim_data_jarak() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"distance\": " + String(jarak) + "}";

    int httpResponseCode = http.POST(jsonData);

    if(httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

void loop() {
  Serial.print("Baca jarak ");
  baca_jarak();
  kirim_data_jarak();
  
  delay(1000);  // Tunggu 1 detik sebelum pembacaan berikutnya
}
