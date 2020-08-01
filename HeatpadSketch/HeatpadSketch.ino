#include <Arduino.h>
#include <Wire.h>
#include <LCD.hpp>
//#include <PID_v1.h>

#define RED   2
#define GREEN 3
#define BLUE  5

nhd::LCD<2,16> lcd;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  analogWrite(RED,255);
  analogWrite(GREEN,0);
  analogWrite(BLUE,0);
  
  lcd.init();
}

void loop() {
  
}
