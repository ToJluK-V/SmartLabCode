#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SERVER_IP "10.135.253.1"
#define OPEN_FOR_MILISECONDS 1000

const char* ssid = "dd-wrt";
const char* password = "telephone";
long int opened_millis = 0;
String payload = "";
String socket_name = "humidity"; //temperature
uint8_t PowerPin = 2; //пин реле

void setup() {
 
  Serial.begin(115200);    // Initialize serial communications with the PC
//  pinMode(LED_BUILTIN, OUTPUT);
// digitalWrite(LED_BUILTIN, HIGH);
  WiFi.begin(ssid, password);

  // Инициализация выхода реле
  pinMode(PowerPin , OUTPUT);
  digitalWrite(PowerPin , LOW);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting..");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  opened_millis = millis();
}

void loop() {

  Serial.println();
  String Link = "http://" SERVER_IP ":8082/socket_state?" + socket_name + "=9";
  Serial.println("Link: " + Link);

  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
  
    http.begin(client, Link); 

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header and body
    int httpCode = http.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      payload = http.getString();
      if (payload == "On") {
        Serial.println(" Turn On ");
        digitalWrite(PowerPin , HIGH);
      }
       if (payload == "Off") {
        Serial.println(" Turn Off");
        digitalWrite(PowerPin , LOW);
      }
    } 
    else {
      Serial.println(" Access Denied ");
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    } 
    http.end();
    delay(1000);
  }
}
