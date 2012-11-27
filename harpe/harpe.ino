#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#include "lasers.h"
#include "ext_synth.h"

#define PEDALE 2

#define PIN_BTN  {PEDALE,7,6,4,5,A5,A4,A0,A2,A3,A1};
int btn[] = PIN_BTN;


void setup()  {
  setup_lasers();
  setup_ldr();
  Serial.begin(115200);
  laserAllOn();

}


unsigned char oldVal1 =-1;
unsigned char oldVal2 =-1;
int lastTime1 = 0;
int lastTime2 = 0;
void loop()
{

  unsigned char val = 0;

  //We read the pedal and 5 firsts lasers
  for(int i=0; i<6;i++)
  {
    if (digitalRead(btn[i]))
      val+=1<<i;
  }
 
  if(val!=oldVal1 || (millis()-lastTime1)>1000)
  {
    Serial.write(val);
    oldVal1=val;
    lastTime1=millis();
  }


  //We read the  5 lasts lasers
  val = 1<<7;//tell the pc that this is the 2 group of lasers
  for(int i=6; i<11;i++)
  {
    if (digitalRead(btn[i]))
      val+=1<<(i-5);
  }
 
  if(val!=oldVal2 || (millis()-lastTime2)>1000)
  {
    Serial.write(val);
    oldVal2=val;
    lastTime2=millis();
  }


}


void setup_ldr()
{
  pinMode(A0, INPUT);  
  pinMode(A1, INPUT);  
  pinMode(A2, INPUT);  
  pinMode(A3, INPUT);  
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);


  pinMode(2, INPUT);  
  pinMode(3, INPUT);  
  pinMode(4, INPUT);  
  pinMode(5, INPUT);  
  pinMode(6, INPUT);
  pinMode(7, INPUT);
    
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
  digitalWrite(A3,LOW);
  digitalWrite(A4,LOW);
  digitalWrite(A5,LOW);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);
}

