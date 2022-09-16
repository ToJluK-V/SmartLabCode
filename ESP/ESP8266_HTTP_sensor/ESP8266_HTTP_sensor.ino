#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_BMP085.h>

#define DHTPIN 12     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22  //
#define ONE_WIRE_BUS_EXT_TEMP 13
#define POWER_PIN 13
#define CO2_analog 0 //CO2 sensor analog out pin

// Replace the next variables with your SSID/Password combination

WiFiClient espClient;
HTTPClient http;
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS_EXT_TEMP);
DallasTemperature sensors(&oneWire);
Adafruit_BMP085 bmp;

String serverName = "http://192.168.21.130:8082/505"; //"http://10.135.254.53:8082/505";
int value = 0;
long int adc_co2 = 0;
float temperature = 0;
float humidity = 0;
float co2 = 0;
int pressure = 0;
long int cycle = 0;


void setup_wifi() {
  delay(10);
  Serial.print("Wi-Fi init...");
  WiFi.begin("dd-wrt", "telephone"); //"dd-wrt", "telephone"//
  for (int i = 0; (i < 20) && (WiFi.status() != WL_CONNECTED); i++) {
    delay(500);
    Serial.print(".");
    if (i >= 18) {
      Serial.println(" Cannot connect Wi-Fi. Resetting...");
      ESP.reset();
    }
  }
  Serial.println(WiFi.localIP());
  Serial.println(" Wi-Fi init end.");
}

void setup() {
  sensors.begin();
  //  pinMode(ONE_WIRE_BUS_EXT_TEMP, INPUT_PULLUP);
  Serial.begin(115200);
  setup_wifi();
  dht.begin();

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
}

void loop() {

  Serial.println(String("loop: ") + cycle);
  // Temperature in Celsius
  temperature = dht.readTemperature(); //dht22 температура в помещении
  if (!isnan(temperature)) {
    Serial.println("dht22_temperature = " + String(temperature));
    for (int i = 0; i < 10; i++) {
      Serial.print(".");
      if (sendGet("dht22_temperature", String(temperature)) == 0) {
        Serial.println(" HTTP OK");
        break;
      }
      else {
        if (i == 5) {
          Serial.println(" Cannot send data via MQTT");
        }
        if (i >= 9) {
          Serial.println(" Reconneting MQTT doesn't help. Resetting");
          ESP.reset();
        }
        delay(1000);
      }
    }
  }
  else {
    Serial.println("dht22_temperature ERROR");
  }

  humidity = dht.readHumidity(); //dht22 влажность в помещении 
  if (!isnan(humidity)) {
    Serial.println("dht22_humidity = " + String(humidity));
    for (int i = 0; i < 10; i++) {
      Serial.print(".");
      if (sendGet("dht22_humidity", String(humidity)) == 0) {
        Serial.println(" HTTP OK");
        break;
      }
      else {
        if (i == 5) {
          Serial.println(" Cannot send data via MQTT");
        }
        if (i >= 9) {
          Serial.println(" Reconneting MQTT doesn't help. Resetting");
          ESP.reset();
        }
        delay(1000);
      }
    }
  }
  else {
    Serial.println("dht22_humidity ERROR");
  }


  adc_co2 = adc_co2 / 60;
  if ((adc_co2 > 300) && (cycle > 3)) //датчик CO2
  {
    Serial.println("mhz14a_co2 = " + String(adc_co2));
    for (int i = 0; i < 10; i++) {
      Serial.print(".");
      if (sendGet("mhz14a_co2", String(adc_co2)) == 0) {
        Serial.println(" HTTP OK");
        break;
      }
      else {
        if (i == 5) {
          Serial.println(" Cannot send data via MQTT");
        }
        if (i >= 9) {
          Serial.println(" Reconneting MQTT doesn't help. Resetting");
          ESP.reset();
        }
        delay(1000);
      }
    }

  }
  else {
    Serial.println("adc_co2 ERROR. adc_co2 = " + String(adc_co2));
  }
  adc_co2 = 0;

  sensors.requestTemperatures();
  delay(750);
  temperature = sensors.getTempCByIndex(0); //ds18b20 температура на улице
//temperature = 44;
  if (temperature > -100) {
    Serial.println("ds18b20_temperature = " + String(temperature));
    for (int i = 0; i < 10; i++) {
      Serial.print(".");
      if (sendGet("ds18b20_temperature", String(temperature)) == 0) {
        Serial.println(" HTTP OK");
        break;
      }
      else {
        if (i == 5) {
          Serial.println(" Cannot send data via MQTT");
        }
        if (i >= 9) {
          Serial.println(" Reconneting MQTT doesn't help. Resetting");
          ESP.reset();
        }
        delay(1000);
      }
    }

  }
  else {
    Serial.println("Ds18b20 ERROR");
  }

  temperature = bmp.readTemperature(); //bmp180?
  if (temperature > -100) {
    Serial.println("bmp180_temperature = " + String(temperature));
    for (int i = 0; i < 10; i++) {
      Serial.print(".");
      if (sendGet("bmp180_temperature", String(temperature)) == 0) {
        Serial.println(" HTTP OK");
        break;
      }
      else {
        if (i == 5) {
          Serial.println(" Cannot send data via MQTT");
        }
        if (i >= 9) {
          Serial.println(" Reconneting MQTT doesn't help. Resetting");
          ESP.reset();
        }
        delay(1000);
      }
    }

  }
  else {
    Serial.println("bmp180_temperature ERROR");
  }

  pressure = bmp.readPressure();//bmp180 давление
  if (pressure > 0) {
    Serial.println("bmp180_pressure = " + String(pressure));
    for (int i = 0; i < 10; i++) {
      Serial.print(".");
      if (sendGet("bmp180_pressure", String(pressure)) == 0) {
        Serial.println(" HTTP OK");
        break;
      }
      else {
        if (i == 5) {
          Serial.println(" Cannot send data via MQTT");
        }
        if (i >= 9) {
          Serial.println(" Reconneting MQTT doesn't help. Resetting");
          ESP.reset();
        }
        delay(1000);
      }
    }

  }
  else {
    Serial.println("bmp180_pressure ERROR");
  }

  for (int h = 0; h < 200; h++) //?
  {
    delay (100);
    adc_co2 += analogRead(CO2_analog);
    Serial.print(".");
  }
  cycle++;
  Serial.println("");
}


int sendGet(String measurement, String value) {
  int result = -1;

  String serverPath = serverName + "?" + measurement + "=" + value;
  http.begin(espClient,serverPath.c_str()); // http.begin(serverPath.c_str()); 
 
Serial.println("serverPath: " + serverPath);
  // Send HTTP GET request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    result = 0;
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  delay(100);
  Serial.println("HTTP end");
  return result;

}
