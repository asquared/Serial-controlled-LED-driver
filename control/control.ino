/* A serial-controlled LED driver, based off ethanspitz's PolyDriverBoard.
 *
 * Copyright 2014 Dan Bruce. Portions copyright 2014 Ethan Spitz.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
 /*
  *
  * Note: You can tell the difference between Dan and Ethan's code because Dan rarely comments.
  * If you see a bunch of code without comments, it's most likely Dan's.
  *
  */

#include <Wire.h>
 
volatile int mode = 0; // current mode
int button = 2; // pin of button
volatile bool bP = false; // button pressed flag to exit current modes
 
int potValue = 0; // value of pot
volatile bool sevSegOff = true;

// values used for mode set/buttton debouncing
volatile long modeSetTime = 0;
  
// definition of registers on the pwm chip
#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define PWM_ADDR 0x40 //address of the PWM chip

void setup(){
  Serial.begin(9600);
  Wire.begin();
  setConfiguration();
}

void loop(){
  String hexch = ""
  char character;
  if (Serial.available())  {
     character = Serial.read();
     hexch.concat(character);
     parse(hexch);
  }
}

void parse(String hexch) {
  int r, g, b, ch;
  r = (15 * hex2int(hexch.substring(0,1))) + hex2int(hexch.substring(1,2));
  g = (15 * hex2int(hexch.substring(2,3))) + hex2int(hexch.substring(3,4));
  b = (15 * hex2int(hexch.substring(4,5))) + hex2int(hexch.substring(5,6));
  ch = (int)hexch.substring(6,7);
  change(r,g,b,ch);
}

int hex2int(char x) {
  switch (x) {
    case a:
      return 10;
    case b:
      return 11;
    case c:
      return 12;
    case d:
      return 13;
    case e:
      return 14;
    case f:
      return 15;
    default:
      return (int)x;
  }
}

void change(int r, int g, int b, int ch) {
  setPWM(ch*3,r/255*4095);
  setPWM(ch*3+1,g/255*4095);
  setPWM(ch*3+2,b/255*4095);
}

/*
 * setPWM brightness on channel given over I2C
 */
void setPWM(int channel, uint16_t brightness)
{
  Wire.beginTransmission(PWM_ADDR);
  Wire.write(LED0_ON_L+4*channel);
  Wire.write(0x00); //turn the LED on at 0
  Wire.write(0x00); //turn the LED on at 0
  
  //turn the LED off when it hits this value (out of 4095)
  Wire.write(brightness); //first four LSB
  Wire.write(brightness>>8); //last four MSB
  Wire.endTransmission();
}


/*
 * Configure the PWM chip for easy suage and external MOSFET drive
 */
void setConfiguration()
{
  Wire.beginTransmission(PWM_ADDR);
  Wire.write(0x00); //enter Mode 1 Register
  Wire.write(0xa1); //enable ocsillator and auto-increment register and restart
  Wire.endTransmission();
  delayMicroseconds(500);//500us delay required after reset
  Wire.beginTransmission(PWM_ADDR);
  Wire.write(0x01); //enter Mode 2 Register
  Wire.write(0x04); //set drive mode for external MOSFETS 
  Wire.endTransmission();
}