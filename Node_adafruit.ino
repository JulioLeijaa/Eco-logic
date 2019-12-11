#include <ESP8266WiFi.h>            //conexiones esp8266
#include <WiFiClient.h>             //D0---bomba(IN1 RELAY)
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <SoftwareSerial.h>
SoftwareSerial serial(12,14);    // (Rx, tx) d6 d5
#include <ArduinoJson.h>
#define WLAN_SSID "Leija" //Q6_Cesar---Servidor---Red Telcel LO---Irasema---LAPTOP-M1VFKTVQ 8181
#define WLAN_PASS "123456789" //12345abc---Orimar1741,---jc30072000---123456789---3fD246;8
/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "Cesar_utt"
#define AIO_KEY       "05037193ee4a460eb2e5ba8bc1e91a45"
/************ Global State  ******************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
/****************************** Feeds ***************************************/
// Setup a feed called 'potValue' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish phumedad = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humedad",MQTT_QOS_1);
Adafruit_MQTT_Publish ptemperatura = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperatura",MQTT_QOS_1);
Adafruit_MQTT_Publish phumedadplantas = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humedadplantas",MQTT_QOS_1);
// Setup a feed called 'ledBrightness' for subscribing to changes.
/*************************** Sketch Code ************************************/
// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

double humedad; double temperatura; int humplantas;

void setup() {
  Serial.begin(115200);
  serial.begin(9600);
  while (!Serial) continue;
  
  Serial.println();
  Serial.print("Connecting to ");Serial.println(WLAN_SSID);
  //WiFi.encryptionType(ENC_TYPE_CCMP); WiFi.begin(WLAN_SSID, WLAN_PASS); WiFi.printDiag(Serial);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println(); Serial.println("WiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
 
  }
void publicaHumedad(double humedad){
  if(! phumedad.publish(humedad)){Serial.println(F("Fallo publicacion de humedad"));} else{Serial.print(F("OK! "));Serial.print("Humedad:");Serial.println(humedad);}
  }
  
void publicaHumplantas(int humplantas){
  if(! phumedadplantas.publish(humplantas)){Serial.println(F("Fallo publicacion de Humedadplantas"));} else{Serial.print(F("OK! "));Serial.print("Humedadplantas:");Serial.println(humplantas);}
  }
  
void publicaTemperatura(double temperatura){
  if(! ptemperatura.publish(temperatura)){Serial.println(F("Fallo publicacion temperatura"));} else{Serial.print(F("OK! "));Serial.print("Temperatura: ");Serial.println(temperatura);}
  }
  
void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& Objetojson = jsonBuffer.parseObject(serial);
  if (Objetojson == JsonObject::invalid())
    return;

  humedad= Objetojson["humedad"];  //Serial.print("humedad: ");Serial.print(humedad); Serial.println(" %");
  temperatura= Objetojson["temperatura"]; //Serial.print("Temperatura: ");Serial.print(temperatura); Serial.println(" C");
  humplantas = Objetojson["humplantas"]; //Serial.print("Humedad de las plantas ");Serial.print(humplantas); Serial.println(" %");
  
  MQTT_connect();
  publicaHumedad(humedad);
  publicaTemperatura(temperatura);
  publicaHumplantas(humplantas);
  Serial.println();
  mqtt.processPackets(500);
  delay(10000);
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return; // Stop if already connected.
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
