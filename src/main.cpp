#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

// --- KONFIGURASI WIFI ---
const char* ssid     = "b401_wifi";
const char* password = "b401juara1";
IPAddress local_IP(192,168,200,3);
IPAddress gateway(192,168,200,1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4);

// --- KONFIGURASI OTA ---
const char* serverIP = "192.168.200.2"; 
const int serverPort = 8000;

const int currentVersion = 2;
const char* versionUrl = "http://192.168.200.2:8000/version.txt";
const char* firmwareUrl = "http://192.168.200.2:8000/firmware.bin";



void performUpdate(WiFiClient &client) {
  // Callback untuk progress (opsional)
  httpUpdate.onProgress([](int cur, int total) {
      Serial.printf("Progress: %d%%\n", (cur * 100) / total);
  });

  t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void checkOTAUpdate() {
  Serial.println("Checking for updates...");
  
  WiFiClient client;
  HTTPClient http;
  
  http.begin(client, versionUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    int newVersion = payload.toInt(); // Parsing angka dari text file

    Serial.print("Server Version: ");
    Serial.println(newVersion);

    if (newVersion > currentVersion) {
      Serial.println("New version found! Starting update...");
      performUpdate(client);
    } else {
      Serial.println("Device is up to date.");
    }
  } else {
    Serial.print("Error checking version. HTTP Code: ");
    Serial.println(httpCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  
  // 1. Koneksi WiFi
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Gagal set Static IP!");
  }
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  
  // 2. Tampilkan Versi Saat Ini
  Serial.print("Current Firmware Version: ");
  Serial.println(currentVersion);

  // 3. Cek Update
  checkOTAUpdate();
}

void loop() {
  // Kode utama proyekmu disini
  // ...
  delay(1000); 
}