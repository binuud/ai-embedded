#ifndef WIFI_INIT_H
#define WIFI_INIT_H

#include <WiFi.h>


#define PART_BOUNDARY "123456789000000000000987654321"


static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";


void connectToWiFi(const char* ssid, const char* password);
void initWifi(String ssid, String password);

void connectToWiFi(const char* ssid, const char* password) {

  WiFi.begin(ssid, password);
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi.");
  }
}

void printWifiStatus() {
  if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("Wifi Connected with IP address %s\n", WiFi.localIP().toString().c_str());
  } else {
      Serial.println("Wifi Not connected, please check credentials or firewall. Stored credentials failed. Please enter new credentials.");
  }
}

void initWifi(String ssid, String password) {

  if (ssid.length() >0 && password.length() > 0) {
    Serial.println("Found saved WiFi credentials.");
    connectToWiFi(ssid.c_str(), password.c_str());
    printWifiStatus();
  } else {
    Serial.println("No WiFi credentials found. Please enter:");
  }

}




#endif // WIFI_INIT_H