
// #include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include <WiFiManager.h>
#include <DNSServer.h>
#include <Ticker.h>
#include "DHT.h"

#define pinLedWifi D4
#define DHTPIN D1 //Pin para el sensor
#define DHTTYPE DHT11 //Tipo de dht
Ticker ticker;
DHT dht(DHTPIN, DHTTYPE); 

  // Creamos una instancia de la clase WiFiManager
  WiFiManager wifiManager;
void parpadearLedWifi(){
  byte estado = digitalRead(pinLedWifi);
  digitalWrite(pinLedWifi, !estado);
}



void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  // WiFi.mode(WIFI_STA);
  // WiFiMulti.addAP("JDGH", "1066508343@");
   Serial.println("DHT11 test!");
 dht.begin();
 pinMode(pinLedWifi, OUTPUT);
 
ticker.attach(0.2, parpadearLedWifi);



 
  // Descomentar para resetear configuración
  // wifiManager.resetSettings();
 
  // Cremos AP y portal cautivo
    // wifiManager.resetSettings();
  if(! wifiManager.autoConnect("ESP-IOT")){
    Serial.println("Fallo en la conexión ");
    ESP.reset();
    delay(1000);
  }

 
  Serial.println("Ya estás conectado");
  //Eliminar el temporizador
  ticker.detach();
  //Apagar el led
  digitalWrite(pinLedWifi, HIGH);
}

void loop() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  float p = 7;
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  // String hu = 
  // String te = 
  // String ph = 
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");
  // wait for WiFi connection
  // if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    // client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://iotagroiot-production.up.railway.app/iot/1/cultivo")) {  // HTTPS
      https.addHeader("Content-Type", "application/json");
      // const char *data = "{\r\n    \"temperatura\": 10,\r\n    \"humedad\": 20,\r\n    \"ph\": 11\r\n}";
       String dat= "";
          delay(2000);
       dat = "{\r\n    \"temperatura\": "+String(t)+",\r\n    \"humedad\": "+String(h)+",\r\n    \"ph\": "+String(random(10))+"\r\n}";
    // const String data = "{\"temperatura\":\"" + String(t) + "\",\"humedad\":\"" + String(h) + "\",\"ph\":\"" + String(random(4,10)) + "\"}}";
    const char *data= dat.c_str();
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.POST(data);

          String payload = https.getString();
          // String payload = https.;
          Serial.println(payload);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          // String payload = https.;
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        // https.GET();
        //           String payload = https.getString();
        //   // String payload = https.;
        //   Serial.println(payload);
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  // }

  Serial.println("Wait 10s before next round...");
  delay(6000);
}