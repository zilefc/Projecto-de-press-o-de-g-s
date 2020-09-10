/*
 * By Frederico Zile
 * fczile@gmail.com 
 * */
 
#include <SoftwareSerial.h>
#include <String.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
/*
SCL A5
SDA A4
 */
SoftwareSerial sim(12, 13);//D7-13  D8=15 TX RX 
LiquidCrystal_I2C display(0x3F,16,2); 

String numero = "+258848856808";//

const float SensorOffset = 50;
float Pressure;     
float Resistor = 154.09;  //154.09
float Pmax = 500;
float Pmin = 0;
float Vref = 3.3;


void setup() {
   Serial.begin(9600);
   sim.begin(9600);
   display.init();
   display.backlight();
   display.setCursor(3,0);
   display.print("Bem-Vindo!");
   display.setCursor(0,1);
   display.print("Sistema iniciado");
   delay(1000);     
  
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
  
  Serial.print(millis());
  Serial.print(",");
  //Read the voltage across the reference resistor
  Pressure = analogRead(A0);
  Pressure =Pressure +SensorOffset;
  int Percentage=map(Pressure,192,1023,0,100);
  //Pressure=Pressure-SensorOffset;
  Serial.print(Pressure);
  Serial.print(",");
  Pressure =(Pmax-Pmin)*Vref/(1023*Resistor*(0.02-0.004))*(Pressure-1023*0.004*Resistor/Vref);
  Pressure=Pressure+14;
  Serial.println(Pressure);
  delay(200);
  
  display.clear();
  display.setCursor(0,0);
  display.print("Pressure: ");
  display.setCursor(10,0);
  display.print(Pressure);
  display.setCursor(13,0);
  display.print("PSI");
  display.setCursor(0, 1);
  display.print("Percentage: ");
  display.setCursor(12,1);
  display.print(Percentage);
  display.setCursor(15,1);
  display.print("%");
  post();
  esperar();
  
}
 void post()
 {
  Pressure = analogRead(A0);
  Pressure =Pressure +SensorOffset;
  int Percentage=map(Pressure,192,1023,0,100);
  Pressure =(Pmax-Pmin)*Vref/(1023*Resistor*(0.02-0.004))*(Pressure-1023*0.004*Resistor/Vref); 
  Pressure=Pressure+14;
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
  String str="GET https://api.thingspeak.com/update?api_key=XSZW2LU9ZDBTPSQA&field1=0" + String(Pressure) +"&field2="+String(Percentage);
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
  while(sim.available()!=0)
  Serial.write(sim.read());
  delay(5000); 
}
void mensagem()
{
 Pressure = analogRead(A0);
 int Percentage=map(Pressure,192,1023,0,100);
 Pressure =(Pmax-Pmin)*Vref/(1023*Resistor*(0.02-0.004))*(Pressure-1023*0.004*Resistor/Vref);
 Pressure=Pressure+14;
 delay(100);   
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + numero + "\"\r");
  delay(1000);
  sim.print("A pressao eh ");
  sim.print(Pressure);
  sim.print("PSI");
  sim.print(" equivalente a ");
  sim.print(Percentage);
  sim.println("% .");
  delay(100);
  sim.println((char)26);
  delay(1000);
}
void esperar()
{
  for(int x=1;x<40;x++)
  {
    delay(30000);
    Serial.println("Meio segundo");
    }
  }
