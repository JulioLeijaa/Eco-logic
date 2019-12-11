#include <Wire.h>              //Conexiones Arduino:
#include <LCD.h>               //SCL---A5 SDA----A4
#include <LiquidCrystal_I2C.h> //VCC---5V GND----GND
#include <DHT.h>               //DHT---2  SUELO--A0
#include <SoftwareSerial.h>    //SERIAL 6----D6 5----D5
#include <ArduinoJson.h>       //LEDverde---13 LEDrojo---12

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7);
SoftwareSerial serial(5,6);  // (Rx, Tx)

int LEDverde = 13; int LEDrojo = 12;
double humedad; double temperatura;
int humplantas;
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  serial.begin(9600);
  Serial.begin(9600);
  dht.begin();
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);
  lcd.clear();

  pinMode(LEDverde, OUTPUT);
  pinMode(LEDrojo, OUTPUT);
}

void leds(int humplantas){
    if (humplantas <= 50){
      digitalWrite(LEDverde, HIGH); 
      digitalWrite(LEDrojo, LOW);
      } else if (humplantas >50){
      digitalWrite(LEDverde, LOW); 
      digitalWrite(LEDrojo, HIGH);  
        }
  }
  
void imprimeLCD(double humedad, double temperatura, int humplantas) {
  String hump = "HUMEDAD: "; hump += humplantas; hump += "%";
  String hum = "Hum.  "; hum += humedad; hum+= " %";
  String tem = "Temp. "; tem += temperatura; tem += " C";
  lcd.setCursor(1,0); lcd.print("BIENVENIDOS A");
  lcd.setCursor(3,1); lcd.print("ECO-LOGIC");
  delay(3000); lcd.clear();
  lcd.setCursor(3,0); lcd.print("CLIMA DE");
  lcd.setCursor(2,1); lcd.print("EL AMBIENTE");
  delay(3000); lcd.clear();
  lcd.setCursor(1,0); lcd.print(hum);
  lcd.setCursor(1,1); lcd.print(tem);
  delay(3000); lcd.clear();
  lcd.setCursor(1,0); lcd.print("INFORMACION DE");
  lcd.setCursor(3,1); lcd.print("LAS PLANTAS");
  delay(3000); lcd.clear();
  lcd.setCursor(1,0); lcd.print(hump);
  delay(3000); lcd.clear();
}

void enviarDatos(double humedad, double temperatura, int humplantas){
  StaticJsonBuffer<1000> jsonBuffer;
 JsonObject& Objetojson = jsonBuffer.createObject();
  Objetojson["humedad"] = humedad;
  Objetojson["temperatura"] = temperatura;
  Objetojson["humplantas"] = humplantas;
  if(serial.available()>0){
 Objetojson.printTo(serial);
 } 
}

void loop() {
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();
  humplantas = analogRead(A0);
  humplantas = humplantas / 10;
  leds(humplantas);
  imprimeLCD(humedad, temperatura, humplantas);
  enviarDatos(humedad, temperatura, humplantas);
}
