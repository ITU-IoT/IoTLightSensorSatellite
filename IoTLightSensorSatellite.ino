#include <WiFi.h>
#include <HttpClient.h>

#define REQUEST_TIMEOUT 10000 //10 seconds
#define WIFI_SSID "NETGEAR" //Wifi SSID
#define WIFI_PASS "hackerMAN158" //Wifi password
#define SERVER "192.168.1.11" //Server to get sensor data
#define PORT 5000 //Server port to get sensor data
#define LIGHT_SENSOR_PIN 0 //Analog pin number

WiFiClient client;
int status = WL_IDLE_STATUS;
int lastRequestTime = millis();

void setup() {
  //Initialise serial communication:
  Serial.begin(9600);
  
  //Check if shield is attached:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  //Connecting to WIFI:
  if(status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASS);
    if(status != WL_CONNECTED) {
      Serial.println("Could not connect to WIFI!");
      while(true);
    }
    delay(1000);
  }
}

void loop() {
  int timeNow = millis();
  int timeSinceLastRequest = timeNow - lastRequestTime;
  if(timeSinceLastRequest > REQUEST_TIMEOUT) {
    postSensorData();
    lastRequestTime = timeNow;
  }
  
  // printing response
  if(client.available()) {
    printResponse();    
  }
}

void printResponse() {
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
}

void postSensorData() {
  client.stop(); //Freeing up sockets
  
  if(client.connect(SERVER, PORT)) {
    int sensorValue = analogRead(LIGHT_SENSOR_PIN); //Resolution: 0-1024 (0V-5V, 0 lux - 1000 lux)
    String data = "{\n\t\"value\": "+String(sensorValue)+"\n}";

    //Printing for debugging:
    Serial.print("requesting to ");
    Serial.println(SERVER);
    Serial.println(data);

    //Writing request:
    client.println("POST /sensor/light HTTP/1.1");
    client.println("Host: 192.168.1.159:5000");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Cache-Control: no-cache");
    client.println("Connection: close");
    client.println();
    client.println(data);
  }
}

