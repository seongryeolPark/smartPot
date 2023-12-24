#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"

#define DELAY 10000
#define BT_RXD 8
#define BT_TXD 7
#define B1A 10
#define B1B 9
#define relaypin 12

RTC_DS3231 rtc;

int set_humidity = 50;
int set_illuminance = 1000;
int CDS = A0;
int CDS2 = 4;
int super = 1;
int starth = 18;
int startm = 30;
int finishh = 22;
int finishm = 0;

SoftwareSerial bluetooth(BT_TXD, BT_RXD);

void setup() {
  #ifndef ESP8266
    while(!Serial);
  #endif

  delay(3000);
  
  Serial.begin(9600);
  bluetooth.begin(9600);
  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);
  pinMode(relaypin, OUTPUT);
  pinMode(CDS, INPUT);

  if(!rtc.begin()){
    Serial.println("Couldn't find RTC");
    while(1);
  }
  
  if(rtc.lostPower()){
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();
  int CDS = analogRead(CDS);
  int CDS2 = digitalRead(CDS2);
  int current_humidity = 120 - (analogRead(A1) / 10);

  for (int i = 0; i < 50; i++) {
    Serial.println();
  }

    Serial.println("1.set_humidity 2. LED_time 3. 24h_mode");

  if(Serial.available() > 0){
    String input = Serial.readStringUntil(' ');

    String input2 = Serial.readStringUntil(' ');

    String input3 = Serial.readStringUntil('\n');

    switch(input.toInt()){
      case 1:
        set_humidity = input2.toInt();
        break;
      case 2:
        starth = (input2.toInt() / 100);
        startm = (input2.toInt() % 100);
        finishh = (input3.toInt() / 100);
        finishm = (input3.toInt() % 100);
        break;
      case 3:
        if(input2 == 'on' || input2 == 'ON'){
          super = 1;
        }
        else if(input2 = 'off' || input2 == 'OFF'){
          super = 0;
        }
        break;
      default:
        break;
    }
    
  }
  
  Serial.print("Current humidity / Set humidity: ");
  Serial.print(current_humidity);
  Serial.print(" / ");
  Serial.println(set_humidity);
  
  if(current_humidity < set_humidity){
    Serial.println("Soil is dry");
    Serial.println("Water pump ON!");
    digitalWrite(B1A, HIGH);
    digitalWrite(B1B, LOW);
    delay(DELAY);
    digitalWrite(B1A, LOW);
    digitalWrite(B1B, LOW);
    Serial.println("Water pump OFF!");
  }else {
    Serial.println("Enough water");
  }
  
  Serial.print("Current time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print("24h mode: ");
  Serial.println(super);

  Serial.print("Current brightness: ");
  Serial.println(CDS);

  Serial.print("LED set time: ");
  if(starth < 10){
    Serial.print("0");
  }
  Serial.print(starth);
  Serial.print(":");
  if(startm < 10){
    Serial.print("0");
  }
  Serial.print(startm);
  Serial.print(" ~ ");
  if(finishh < 10){
    Serial.print("0");
  }
  Serial.print(finishh);
  Serial.print(":");
  if(finishm < 10){
    Serial.print("0");
  }
  Serial.println(finishm);

if((((starth*60)+startm) <= ((now.hour()*60)+now.minute())) && (((now.hour()*60)+now.minute()) <= ((finishh*60) + finishm)) && (digitalRead(relaypin) == LOW)){
  digitalWrite(relaypin, HIGH);
}else{
  if((super == 1) && (CDS >= set_illuminance)){
    digitalWrite(relaypin, HIGH);
  }else{
    digitalWrite(relaypin, LOW);
  }
}

  if(digitalRead(relaypin) == LOW ){
    Serial.println("LED state: OFF");
  }else{
    Serial.println("LED state: ON");
  }
  
  for(int i = 0; i < 3; i++){
    Serial.println();
  }
  delay(DELAY); 
}
