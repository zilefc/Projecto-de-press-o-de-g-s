/*
 * By Frederico Zile
 * fczile@gmail.com 
 * */
 
#include <SoftwareSerial.h>
#include <String.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
/*
Vin 3.3V
GND GND
SCL GPIO 5 (D1)
SDA GPIO 4 (D2)
 */

SoftwareSerial sim(13, 15);//D7  D8 TX RX 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String numero = "+258878780367";

const float SensorOffset = 189;
float Pressure;     
float Resistor = 162;  //reference resistor resistance
float Pmax = 500;           //Max of process variable range
float Pmin = 0;             //Min of process variable range
float Vref = 5;             //Reference voltage for AnalogRead


void setup() {
  sim.begin(115200);
  delay(1000);     
  Serial.begin(115200); 
  Serial.println("Time, Raw reading, Corrected reading");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();
  display.drawFastHLine(0, 32, SSD1306_LCDWIDTH, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(10, 5);
  display.print("sensorValue(PSI)");
  display.setCursor(19, 37);
  display.print("Percentagem (%)");
  display.display();
}

void loop() {
  
  Serial.print(millis());
  Serial.print(",");
  //Read the voltage across the reference resistor
  Pressure = analogRead(A0);
  int Percentage=map(Pressure,192,1023,0,100);
  //Pressure=Pressure-SensorOffset;
  Serial.print(Pressure);
  Serial.print(",");
  //Convert the voltage signal to a pressure reading
  Pressure =(Pmax-Pmin)*Vref/(1023*Resistor*(0.02-0.004))*(Pressure-1023*0.004*Resistor/Vref);
  Serial.println(Pressure);
  delay(200);
  
  display.setCursor(40, 20);
  display.print(Pressure);
  display.setCursor(40, 52);
  display.print(Percentage);
  display.display();
  delay(1000);

  post();
  delay(1000);
  if(Percentage<21){
    mensagem();
    delay(100);
    }
  
}
 void post()
 {
  Pressure = analogRead(A0);
  int Percentage=map(Pressure,192,1023,0,100);
  Pressure =(Pmax-Pmin)*Vref/(1023*Resistor*(0.02-0.004))*(Pressure-1023*0.004*Resistor/Vref); 
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
