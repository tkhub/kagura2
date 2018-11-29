#include <Arduino.h>
#line 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
#line 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
#include <SPI.h>
#include <Wire.h>
//#include <FaBoLCDmini_AQM0802A.h>

/********  PIN CONFIG  *****************/
/*  MOSI          SPI           --1 ----1--   SCK     SCK
    NC            RXLED/SS      --2     2--   MISO    MISO
    COM_TX        D1/TX         --3     3--   12V     ----
    COM_RX        D0/RX         --4     4--   GND     ----
    ----          RESET         --5     5--   RESET   ----
    ----          GND           --6     6--   5VOUT   ----
    SDA           D2/SDA        --7     7--   NC
    SCL           D3/SCL        --8     8--   NC
    IR_ADC        D4/A6         --9     9--   A5      SPD_PID_ADC
    DIV_ENPID_DO  D5(PWM)       --10    10--  A4      STR_PID_ADC
    IL_ADC        D6(PWM)/A7    --11    11--  A3      TG_L_ADC
    DIV_ENMTR_DO   D7            --12    12--  A2      TG_R_ADC
    SW_ADC        IO8/A8        --13    13--  A1      SSR_L_ADC
    BATTERY_ADC   IO9(PWM)/A9   --14    14--  A0      SSR_R_ADC
    CS            IO10(PWM)/A10 --15    15--  VREF    ----
    LOADDAC       IO11(PWM)     --16    16--  3.3VOUT ----
    CORNER_MK     IO12/A11      --17----17--  IO13    GOAL_MK
*/
/*****************************************************************************/

#line 26 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
void setup();
#line 74 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
void spisetup();
#line 92 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
bool goalmkread(void);
#line 104 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
bool cornermkread(void);
#line 126 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
void loop();
#line 2 "c:\\home\\Dropbox\\Arduino\\kagura2\\answ.ino"
void enpid(void);
#line 7 "c:\\home\\Dropbox\\Arduino\\kagura2\\answ.ino"
void dipid(void);
#line 12 "c:\\home\\Dropbox\\Arduino\\kagura2\\answ.ino"
void enmtr(void);
#line 17 "c:\\home\\Dropbox\\Arduino\\kagura2\\answ.ino"
void dimtr(void);
#line 3 "c:\\home\\Dropbox\\Arduino\\kagura2\\dac.ino"
void dacoutput(byte ch, u16 dat);
#line 49 "c:\\home\\Dropbox\\Arduino\\kagura2\\dac.ino"
void targetspeed(float t_spd);
#line 10 "c:\\home\\Dropbox\\Arduino\\kagura2\\ioext.ino"
void ioextsetinout(byte data);
#line 15 "c:\\home\\Dropbox\\Arduino\\kagura2\\ioext.ino"
void ioextoutput(byte data);
#line 24 "c:\\home\\Dropbox\\Arduino\\kagura2\\ioext.ino"
byte ioextinput(void);
#line 15 "c:\\home\\Dropbox\\Arduino\\kagura2\\sw.ino"
byte swread(void);
#line 26 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
void setup() {
  // put your setup code here, to run once:
  //  peripheral init
  //  GOAL Marker  
  pinMode(13, INPUT);
  //  CORNER_MK 
  pinMode(12, INPUT);  
  
  //  CS setup
  pinMode(10,OUTPUT);
  //  LOADDACsetup
  pinMode(11,OUTPUT);
  
  pinMode(5, OUTPUT);
  
  pinMode(7, OUTPUT);

  //  CS Negate
  digitalWrite(10,HIGH);
  //  LOADDAC Negate
  digitalWrite(11,LOW);
  // DIV_ENPID_DO   
  dipid();
  // DIV_ENMTR_DO  
  dimtr();

  Serial.begin(9600);
  Serial.print("Serial Init");

  spisetup();
  Serial.print("SPI Init");

//  Wire.begin();

  // Software init
  dacoutput(0,2048);
  dacoutput(1,2048);
  dacoutput(2,2048);
  dacoutput(3,2048);
  Serial.print("DAC OUT 0V");
  
  //lcd.begin();
//  lcd.print("hello!");

  Serial.print("Kagura2 Start");

}

void spisetup()
{
  /* SPI初期化 */
  SPI.begin();
  /* MSB FAST */
  SPI.setBitOrder(MSBFIRST);
  /* 1MHz CLK */
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  /* クロック負パルス 立ち上がりエッジサンプリング */
  SPI.setDataMode(SPI_MODE3);
}

#define SW_UP   3
#define SW_DOWN 4
#define SW_GO   1
#define SW_ESC  2 
#define SW_NON  0

bool  goalmkread(void)
{
  if (digitalRead(13))
  {
    return false;
  }
  else
  {
    return true;
  }
}

bool  cornermkread(void)
{
  if (digitalRead(12))
  {
    return false;
  }
  else
  {
    return true;
  }
}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/**                        loop                                            ***/
/*****************************************************************************/
/*****************************************************************************/

u16 mode = 0;
byte modesw = SW_NON;
byte modeswtmp = SW_NON;
u32  nrmlclk = 0;
void loop() {
  // put your main code here, to run repeatedly:
  u16 tmp;
  u16 adval;
  nrmlclk++;
  Serial.print(nrmlclk);
  if ((modeswtmp == SW_NON) && (modesw != SW_NON))
  {
    switch (mode)
    {
      case 0:
      // 0.0
        dipid();
        targetspeed(0.0);
        Serial.print("0.0, DISABLE PID,");
        if (modesw == SW_UP)
        {
          mode = 1;
        }
        else if (modesw == SW_DOWN)
        {
          mode = 4;
        }
      break;
      case 1:
      // 0.1
        enpid();
        targetspeed(0.1);
        Serial.print("0.1,");
        if (modesw == SW_UP)
        {
          mode = 2;
        }
        else if (modesw == SW_DOWN)
        {
          mode = 0;
        }
      break;

      case 2:
      // 0.5
        targetspeed(0.5);
        Serial.print("0.5,");
        if (modesw == SW_UP)
        {
          mode = 3;
        }
        else if (modesw == SW_DOWN)
        {
          mode = 1;
        }
      break;

      case 3:
      // 1.5
        targetspeed(1.5);
        Serial.print("1.5,");
        if (modesw == SW_UP)
        {
          mode = 3;
        }
        else if (modesw == SW_DOWN)
        {
          mode = 2;
        }
      break;

      case 4:
      // -0.1
        enpid();
        targetspeed(-0.1);
        Serial.print("-0.1,");
        if (modesw == SW_UP)
        {
          mode = 0;
        }
        else if (modesw == SW_DOWN)
        {
          mode = 5;
        }
      break;

      case 5:
      // -0.5
        targetspeed(-0.5);
        Serial.print("-0.5,");
        if (modesw == SW_UP)
        {
          mode = 4;
        }
        else if (modesw == SW_DOWN)
        {
          mode = 6;
        }
      break;

      case 6:
      // -1.5
        targetspeed(-1.5);
        Serial.print("-1.5,");
        if (modesw == SW_UP)
        {
          mode = 5;
        }
        else if (modesw == SW_DOWN)
        {
          mode = 6;
        }
      break;

      default:
      break;
    }

    if (modesw == SW_GO)
    {
      enmtr();
      Serial.print("ENPID,");
    }
    else if (modesw == SW_ESC)
    {
      dimtr();
      Serial.print("DISABLE PID,");
    }

    Serial.print(mode)  ;
    Serial.print('\n');
      
  }

  Serial.print("\n");
 // ioextoutput(mode);
  modeswtmp = modesw;
  modesw = swread();
  delay(50);
}

#line 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\answ.ino"
/*****************************************************************************/
void enpid(void)
{
    digitalWrite(5,LOW);
}

void dipid(void)
{
    digitalWrite(5,HIGH);
}

void enmtr(void)
{
    digitalWrite(7,LOW);
}

void dimtr(void)
{
    digitalWrite(7,HIGH);
}
#line 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\dac.ino"
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
#line 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\ioext.ino"
/*****************************************************************************/
/*                    PCF8574 */
/*****************************************************************************/
#include <Wire.h>
#define IOEXT_ADR  0x38

byte ioext_inoutconf;
byte ioext_outputtmp;

void ioextsetinout(byte data)
{
    ioext_inoutconf = data;
}

void ioextoutput(byte data)
{
    ioext_outputtmp = data;
    Wire.beginTransmission(IOEXT_ADR);
//    Wire.write(data && ioext_inoutconf);
    Wire.write(data);
    Wire.endTransmission();
}

byte ioextinput(void)
{
    byte inputdata;
    Wire.requestFrom(IOEXT_ADR, 1);
    Wire.write(ioext_outputtmp && ioext_inoutconf);
    while (Wire.available())
    {
        inputdata = Wire.read();
    }
    return inputdata;
}
#line 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\sw.ino"
/*****************************************************************************/
 

#define SW_UP   3
#define SW_DOWN 4
#define SW_GO   1
#define SW_ESC  2 
#define SW_NON  0

#define SW_GO_TH    (u16)(100)     /*  */
#define SW_ESC_TH   (u16)(520)     /*  */
#define SW_UP_TH    (u16)(700)     /* */
#define SW_DOWN_TH  (u16)(800)    /*  */

byte swread(void)
{
    u16 tmp, adc0, adc1;
    adc0 = analogRead(8);
    delay(4);
    adc1 = analogRead(8);
    tmp = (adc0 + adc1) / 2;
    if (tmp < SW_GO_TH)
    {
       return SW_GO;
    }
    else if (tmp < SW_ESC_TH)
    {
        return SW_ESC;
    }
    else if (tmp < SW_UP_TH)
    {
        return SW_UP;
    }
    if (tmp < SW_DOWN_TH)
    {
       return SW_DOWN;
    }
    else
    {
        return SW_NON;
    }
}
