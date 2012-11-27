
/* laser pins & mapping */

//Pin connected to ST_CP of 74HC595
int latchPin = 11;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 13;
byte laser[]={7,7+8,0+8,6+8,1+8,5+8,2+8,4+8,3+8,0};


byte laserShift[2];

void setup_lasers();
void laserAllOn();
void laserAllOff();
void laserCommit();
void laserOn(int laserNumber);
void laserOff(int laserNumber);
void shiftOut(int myDataPin, int myClockPin, byte myDataOut);

void setup_lasers()
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(latchPin,LOW);
  digitalWrite(clockPin,LOW);
  digitalWrite(dataPin,LOW);
}

void laserAllOn()
{
  for (int i=0; i<10;i++)
    laserOn(i);    
  laserCommit();
}

void laserAllOff()
{
  for (int i=0; i<10;i++)
    laserOff(i);    
  laserCommit();
}

void laserCommit()
{
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, laserShift[0]);
  shiftOut(dataPin, clockPin, laserShift[1]); 
  digitalWrite(latchPin, 1);
}

void laserOn(int laserNumber)
{
  if (laser[laserNumber]<8)
  {
    laserShift[0]|=1<<laser[laserNumber];
  }else{
    laserShift[1]|=1<<(laser[laserNumber]%8);
  }
}

void laserOff(int laserNumber)
{
  if (laser[laserNumber]<8)
  {
    laserShift[0]&=~(1<<laser[laserNumber]);
  }else{
    laserShift[1]&=~(1<<(laser[laserNumber]%8));
  }
}

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

//internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

//clear everything out just in case to
//prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {	
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}
