

#include <avr/io.h>
#include <avr/wdt.h>

#define PIN_RED 5
#define PIN_GREEN 6
#define PIN_BLUE 3

#define PIN_RVB {PIN_RED,PIN_GREEN,PIN_BLUE}
#define PIN_BTN  {9,10,11,12,13}

int btn[] = PIN_BTN;
int led[] = PIN_RVB;

char Rintensity = 0;
char Gintensity = 0;
char Bintensity = 0;

void setup() {
  pinMode(PIN_RED,OUTPUT);
  pinMode(PIN_GREEN,OUTPUT);
  pinMode(PIN_BLUE,OUTPUT);

  Serial.begin(2*115200);
}


unsigned char oldVal =-1;
int lastTime = 0;
void loop() {
  /*analogWrite(PIN_RED,127+125*cos(2*3.14*millis()/1000)); 
  analogWrite(PIN_GREEN,127+125*cos(2*3.14*millis()/1000)); 
  analogWrite(PIN_BLUE,127+125*cos(2*3.14*millis()/1000)); */
 /*analogWrite(PIN_RED,255);
 analogWrite(PIN_GREEN,255);
 analogWrite(PIN_BLUE,255);*/

	unsigned char val = 0;
	for(int i=0; i<3;i++)
	{
		//digitalWrite(led[i], !digitalRead(btn[i]));
		if (!digitalRead(btn[i]))
			val+=1<<i;
	}
 
	if(val!=oldVal || (millis()-lastTime)>100)
	{
		Serial.write(val);
		oldVal=val;
		lastTime=millis();
	}

	while(Serial.available()>=2)
	{
		byte c = Serial.read(); //commande
		switch (c)
		{
			case 'R' :
				Rintensity =Serial.read();
				analogWrite(PIN_RED,Rintensity);
				break;
			case 'G' :
				Gintensity =Serial.read();
				analogWrite(PIN_GREEN,Gintensity);
				break;
			case 'B' :
				Bintensity =Serial.read();
				analogWrite(PIN_BLUE,Bintensity);
				break;
			default :;
				//do nothing
		}
	}

}
