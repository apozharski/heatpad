#include <Arduino.h>
#include <Wire.h>
#include <RGBLCD.hpp>
#include <types/BasicOnOff.hpp>
#include <stdint.h>
//#include <PID_v1.h>

#define RED   2
#define GREEN 3
#define BLUE  5

nhd::RGBLCD<2,16> lcd(RED, GREEN, BLUE);

void setup() {  
  Wire.begin();
  Serial.begin(9600);
  lcd.init();
  delay(1000);
}
int state = 0;
void loop() {
  
}
