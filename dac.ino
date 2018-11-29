#include <SPI.h>

void dacoutput(byte ch, u16 dat)
{
  u16 dat_tmp = 0;
  u8 shortdat = 0;
  //  CS LOW
  digitalWrite(10, LOW);
  //  LOADDAC HIGH
  digitalWrite(11, HIGH);
  dat_tmp = dat;
  switch(ch)
  {
    case 0:
      dat_tmp = dat_tmp | 0x0000;
    break;

    case 1:
      dat_tmp = dat_tmp | 0x4000;
    break;

    case 2:
      dat_tmp = dat_tmp | 0x8000;
    break;

    case 3:
      dat_tmp = dat_tmp | 0xC000;
    break;
  }
  shortdat = (dat_tmp >> 8) & 0x00FF;
  SPI.transfer(shortdat); 
  shortdat = dat_tmp & 0x00FF;
  SPI.transfer(shortdat); 
  //  CS HIGH
  digitalWrite(10,HIGH);
  //  LOADDAC LOW 
  digitalWrite(11, LOW);
}


// 6V = 0.77V = 8810rpm * 6V / 3V
// 3874rpm = 0.887V
// 24.5mm
// 1.5819m/s = 0.887V 
//  speed/volt = 1.5819/0.887
//  volt / speed = 0.56072
//  DAC 2.5V 4096
// vout = -2.5+(2.5+2.5)N/4
void targetspeed(float t_spd)
{
  u16 tmp;
  float ftmp;
  ftmp = (t_spd * -459.32544)+2048;
  tmp = (u16)ftmp;
//  Serial.print(tmp);
  dacoutput(0, tmp);
}