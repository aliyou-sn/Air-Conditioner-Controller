#include <Arduino.h>
#include "DHT.h"
#include <ESP32Time.h>
#include <iostream>
#define DHTPIN D5
#define DHTTYPE DHT11
const int AC = D6;
const int pir = D3;
unsigned long lastTrigger = 0;
bool startTimer = false;
bool motion = false;
bool action = false;
const int Time = 300000;
unsigned long now = millis();
float temp;
void AC_Control();
DHT dht(DHTPIN, DHTTYPE);
ESP32Time rtc(3600);
String Now;
String Start = "07:00:00";
String Stop = "19:00:00";

ICACHE_RAM_ATTR void detectsMovement(){
  if(action && temp>25){
  digitalWrite(AC, HIGH);
  startTimer = true;
  lastTrigger = millis();
  }
}

void readweather(){
  temp = dht.readTemperature();
  Serial.print(F(" Temperature : "));
  Serial.print(temp);
  Serial.println(F("°C "));
  // delay(1000);
}

void gettime(){
  Now = rtc.getTime();
  if(Now == Start){
    action = true;
  }
  if(Now == Stop){
    action = false;
  }
}



void setup() {
  Serial.begin(115200);
  dht.begin();
  rtc.setTime(30, 59, 05, 25, 6, 2023);  // 25th June 2023 05:26:30
  pinMode(pir, INPUT_PULLUP);
  pinMode(AC, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pir), detectsMovement, RISING);
  digitalWrite(AC, LOW);
}

void loop() {
  now = millis();
  gettime();
  AC_Control();
  readweather();
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format 
  struct tm timeinfo = rtc.getTimeStruct();
}

void AC_Control(){
  if((digitalRead(AC) == HIGH) && (motion == false)) {
    motion = true;
    Serial.println("Motion Detected....");
  }
  if(startTimer && ((now - lastTrigger) > Time) && temp < 30){
    digitalWrite(AC, LOW);
    startTimer = false;
    motion = false;
    Serial.println("Motion Stopped....");
  }
  if(startTimer && temp <= 20 ){
    digitalWrite(AC, LOW);
    startTimer = false;
    motion = false;
     Serial.println("Motion Stopped....");
  }
  if(startTimer && !action) {
    digitalWrite(AC, LOW);
    startTimer = false;
    motion = false;
     Serial.println("Motion Stopped....");
  }
  
}

