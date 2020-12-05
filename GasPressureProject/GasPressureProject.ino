/*
   By Frederico Zile
   fczile@gmail.com
 * */

#include <SoftwareSerial.h>
#include <String.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "Timer.h"

#define DHTPIN 2    // modify to the pin we connected

#define DHTTYPE DHT21   // AM2301 

/*
  SCL A5
  SDA A4
*/
SoftwareSerial sim(12, 13);//D7-13  D8=15 TX RX
LiquidCrystal_I2C display(0x3F, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
Timer tempo;

String numero = "+258848856808";//
int sensorVal;
float voltage, pressure_pascal, pressure_bar, pressure_psi;

/////////////Temperatura////////////////////
float V;
float tempC;
float temp1;
float calibration;
float Rx;
//float C = 71.9336;
//float slope = 139.6071;
float C = 68.583;
float slope = 195.45;
float R0 = 100.0;
float alpha = 0.00385;
int Vin = A0; // Vin is Analog Pin A0



void setup() {
  Serial.begin(9600);
  dht.begin();
  sim.begin(9600);
  display.init();
  display.backlight();
  display.setCursor(3, 0);
  display.print("Bem-Vindo!");
  display.setCursor(0, 1);
  display.print("Sistema iniciado");
  delay(1000);
  pinMode(Vin, INPUT);
  //analogReference(INTERNAL);
  //tempo.every(100, takeReading);

  Serial.println("Time, Raw reading, Corrected reading");
  comeco();
}
void comeco()
{
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + numero + "\"\r");
  delay(1000);
  sim.println("Sistema iniciado. Veja os dados da leitura em tempo real em: https://thingspeak.com/channels/1134042/widgets/213604 ");
  delay(100);
  sim.println((char)26);
  delay(1000);
}
void loop() {
  //tempo.update();
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //temperatura pt100
  V = (analogRead(Vin) / 1023.0) * 1.1;
  Rx = V * slope + C;
  temp1 = (Rx / R0 - 1.0) / alpha;
  calibration = 0.3 + (0.005 * temp1);
  tempC = temp1 - calibration;

  if (isnan(t) || isnan(h))
  {
    Serial.println("Falha na leitura do sensor de temperatura externo");
  }
  else
  {
    Serial.print("Humidade: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("TemperaturaExterna: ");
    Serial.print(t);
    Serial.println(" *C");
    delay(400);

    sensorVal = analogRead(A0);
    Serial.print("Sensor Value: ");
    Serial.print(sensorVal);

    voltage = (sensorVal * 5.0) / 1024.0;
    Serial.print("  Volts: ");
    Serial.print(voltage);

    pressure_pascal = (3.0 * ((float)voltage - 0.47)) * 1000000.0;
    pressure_bar = pressure_pascal / 10e5;
    pressure_psi = pressure_bar * 14.05;
    Serial.print("  Pressure = ");
    Serial.print(pressure_bar);
    Serial.println(" bars");
    Serial.print("Pressure = ");
    Serial.print(pressure_psi);
    Serial.println(" psi");


    display.clear();
    display.setCursor(0, 0);
    display.print("Pressure: ");
    display.setCursor(10, 0);
    display.print(pressure_bar);
    display.setCursor(13, 0);
    display.print("bar");
    display.setCursor(0, 1);
    display.print("temp: ");
    display.setCursor(9, 1);
    display.print(t);
    display.setCursor(14, 1);
    display.println(" °C");
    post();
    esperar();
  }
}
void post()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  sensorVal = analogRead(A0);
  voltage = (sensorVal * 5.0) / 1024.0;
  pressure_pascal = (3.0 * ((float)voltage - 0.47)) * 1000000.0;
  pressure_bar = pressure_pascal / 10e5;
  pressure_psi = pressure_bar * 14.05;
  
  V = (analogRead(Vin) / 1023.0) * 1.1;
  Rx = V * slope + C;
  temp1 = (Rx / R0 - 1.0) / alpha;
  calibration = 0.3 + (0.005 * temp1);
  tempC = temp1 - calibration;

  if (sim.available())
    Serial.write(sim.read());

  sim.println("AT");
  delay(1000);
  sim.println("AT+CPIN?");
  delay(1000);
  sim.println("AT+CREG?");
  delay(1000);
  sim.println("AT+CGATT?");
  delay(1000);
  sim.println("AT+CIPSHUT");
  delay(1000);
  sim.println("AT+CIPSTATUS");
  delay(2000);
  sim.println("AT+CIPMUX=0");
  delay(2000);
  ShowSerialData();
  sim.println("AT+CSTT=\"internet\"");
  delay(1000);
  ShowSerialData();
  sim.println("AT+CIICR");
  delay(3000);
  ShowSerialData();
  sim.println("AT+CIFSR");
  delay(2000);
  ShowSerialData();
  sim.println("AT+CIPSPRT=0");
  delay(3000);
  ShowSerialData();
  sim.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");
  delay(6000);
  ShowSerialData();
  sim.println("AT+CIPSEND");
  delay(4000);
  ShowSerialData();
  String str = "GET https://api.thingspeak.com/update?api_key=GVIF7QQYZWMU1IVU&field1=0" + String(pressure_bar) + "&field2=" + String(pressure_psi) + "&field3=" + String(tempC) + "&field4=" + String(t)+ "&field5=" + String(h);
  Serial.println(str);
  sim.println(str);
  delay(4000);
  ShowSerialData();
  sim.println((char)26);
  delay(5000);
  sim.println();
  ShowSerialData();
  sim.println("AT+CIPSHUT");
  delay(100);
  ShowSerialData();
}
void ShowSerialData()
{
  while (sim.available() != 0)
    Serial.write(sim.read());
  delay(5000);
}
void mensagem()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  sensorVal = analogRead(A0);
  voltage = (sensorVal * 5.0) / 1024.0;
  pressure_pascal = (3.0 * ((float)voltage - 0.47)) * 1000000.0;
  pressure_bar = pressure_pascal / 10e5;
  pressure_psi = pressure_bar * 14.05;

  V = (analogRead(Vin) / 1023.0) * 1.1;
  Rx = V * slope + C;
  temp1 = (Rx / R0 - 1.0) / alpha;
  calibration = 0.3 + (0.005 * temp1);
  tempC = temp1 - calibration;

  delay(100);
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + numero + "\"\r");
  delay(1000);
  sim.print("A pressao eh ");
  sim.print(pressure_psi);
  sim.print("PSI");
  sim.print(" temperaturaExterna ");
  sim.print(t);
  sim.println("C ,");
  sim.print(" temperaturaPT100: ");
  sim.print(tempC);
  sim.println("graus.");
  delay(100);
  sim.println((char)50);
  delay(1000);
}
void esperar()
{
  for (int x = 1; x < 40; x++)
  {
    delay(30000);
    Serial.println("Meio segundo");
  }
}
/*
void takeReading() {
  V = (analogRead(Vin) / 1023.0) * 1.1;
  Rx = V * slope + C;
  temp1 = (Rx / R0 - 1.0) / alpha;
  calibration = 0.3 + (0.005 * temp1);
  tempC = temp1 - calibration;
  Serial.print("Bits: ");
  Serial.println(analogRead(Vin));
  Serial.print("Rx: ");
  Serial.println(Rx);
  Serial.print("Volts: ");
  Serial.println(V);
  Serial.print("temp1: ");
  Serial.println(temp1);
  Serial.print("tempC: ");
  Serial.println(tempC);
  delay(3000);
}
*/
