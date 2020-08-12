#include <Arduino.h>
#include <Wire.h>
#include <RGBLCD.hpp>
#include <types/BasicOnOff.hpp>
#include <stdint.h>
#include <PID_v1.h>

#define RED   2 // @todo in next iteration fix this. it is not a PWM pin
#define GREEN 3
#define BLUE  5
#define DETECT 11  //zero cross detect
#define GATE 12    //TRIAC gate
#define PULSE 4   //trigger pulse width (counts)
const uint8_t heat_in = A7;

double currentTemp, targetTemp;
double Kp = 2000, Ki = 25, Kd = 0;
double outputTime;
nhd::RGBLCD<2,16> lcd(RED, GREEN, BLUE);
PID heatPID(&currentTemp, &outputTime, &targetTemp, Kp, Ki, Kd, DIRECT);

int t_to_s(double temp)
{
  return (int)(11.38461 * temp + 153.38461);
  
}

double s_to_t(int sense)
{
  return 0.08783 * (double)sense - 13.47297; 
}

void setup() {  
  Wire.begin();
  Serial.begin(9600);
  lcd.init();
  delay(1000);
  lcd.setColor(0,255,0);
  // set up pid
  heatPID.SetOutputLimits(0,55500 - 1);
  heatPID.SetMode(AUTOMATIC);
  targetTemp = t_to_s(35);

  // set up pins
  pinMode(DETECT, INPUT);     //zero cross detect
  digitalWrite(DETECT, HIGH); //enable pull-up resistor
  pinMode(GATE, OUTPUT);      //TRIAC gate control

  // setup EVSYS
  EVSYS.USERTCB1 = EVSYS_CHANNEL_CHANNEL5_gc;     // Connect TCB1 to strobe
  EVSYS.CHANNEL4 = EVSYS_GENERATOR_TCB1_CMP0_gc;  // Connect TCB2 to TCB1
  EVSYS.USERTCB2 = EVSYS_CHANNEL_CHANNEL4_gc;
  
  // set up TCB1 to single shot mode to set delay
  TCB1.CCMP = TCB1.CNT = 65500;   // This is how you control how long the single shot waits for.
  TCB1.EVCTRL = TCB_CAPTEI_bm; // Enable events with positive edge sensitivity
  TCB1.INTCTRL = TCB_CAPT_bm; 
  TCB1.CTRLB = TCB_CNTMODE_SINGLE_gc | TCB_CCMPEN_bm; //| TCB_ASYNC_bm; // Start in single shot mode async
  TCB1.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm; // Actually start the counter
  
  // set up TCB2 to turn off the pulse
  TCB2.CCMP = TCB2.CNT = 10000;
  TCB2.EVCTRL = TCB_CAPTEI_bm;
  TCB2.INTCTRL = TCB_CAPT_bm; 
  TCB2.CTRLB = TCB_CNTMODE_SINGLE_gc | TCB_CCMPEN_bm;
  TCB2.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm;

  attachInterrupt(digitalPinToInterrupt(DETECT), zeroCrossing, RISING);
}
void loop() {
  currentTemp = analogRead(heat_in);
  heatPID.Compute();
  delay(100);
  Serial.print(s_to_t(currentTemp)); Serial.print(","); Serial.println(((55500 - outputTime)*100.0)/55500);
}

void zeroCrossing()
{
  // TODO get PID value
  TCB1.CCMP = (55500 - (uint16_t)outputTime) + 1;
  EVSYS.STROBE = EVSYS_STROBE05_bm;
}

ISR(TCB1_INT_vect)
{
  digitalWrite(GATE, HIGH);
  TCB1.INTFLAGS = TCB_CAPT_bm;
}

ISR(TCB2_INT_vect)
{
  digitalWrite(GATE, LOW);
  TCB2.INTFLAGS = TCB_CAPT_bm;
}
