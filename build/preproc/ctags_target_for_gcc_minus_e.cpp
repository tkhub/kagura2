# 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
# 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino"
# 2 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino" 2
# 3 "c:\\home\\Dropbox\\Arduino\\kagura2\\kagura2.ino" 2
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

void setup() {
  // put your setup code here, to run once:
  //  peripheral init
  //  GOAL Marker  
  pinMode(13, 0x0);
  //  CORNER_MK 
  pinMode(12, 0x0);

  //  CS setup
  pinMode(10,0x1);
  //  LOADDACsetup
  pinMode(11,0x1);

  pinMode(5, 0x1);

  pinMode(7, 0x1);

  //  CS Negate
  digitalWrite(10,0x1);
  //  LOADDAC Negate
  digitalWrite(11,0x0);
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
  SPI.setBitOrder(1);
  /* 1MHz CLK */
  SPI.setClockDivider(0x01);
  /* クロック負パルス 立ち上がりエッジサンプリング */
  SPI.setDataMode(0x0C);
}







bool goalmkread(void)
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

bool cornermkread(void)
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
byte modesw = 0;
byte modeswtmp = 0;
u32 nrmlclk = 0;
void loop() {
  // put your main code here, to run repeatedly:
  u16 tmp;
  u16 adval;
  nrmlclk++;
  Serial.print(nrmlclk);
  if ((modeswtmp == 0) && (modesw != 0))
  {
    switch (mode)
    {
      case 0:
      // 0.0
        dipid();
        targetspeed(0.0);
        Serial.print("0.0, DISABLE PID,");
        if (modesw == 3)
        {
          mode = 1;
        }
        else if (modesw == 4)
        {
          mode = 4;
        }
      break;
      case 1:
      // 0.1
        enpid();
        targetspeed(0.1);
        Serial.print("0.1,");
        if (modesw == 3)
        {
          mode = 2;
        }
        else if (modesw == 4)
        {
          mode = 0;
        }
      break;

      case 2:
      // 0.5
        targetspeed(0.5);
        Serial.print("0.5,");
        if (modesw == 3)
        {
          mode = 3;
        }
        else if (modesw == 4)
        {
          mode = 1;
        }
      break;

      case 3:
      // 1.5
        targetspeed(1.5);
        Serial.print("1.5,");
        if (modesw == 3)
        {
          mode = 3;
        }
        else if (modesw == 4)
        {
          mode = 2;
        }
      break;

      case 4:
      // -0.1
        enpid();
        targetspeed(-0.1);
        Serial.print("-0.1,");
        if (modesw == 3)
        {
          mode = 0;
        }
        else if (modesw == 4)
        {
          mode = 5;
        }
      break;

      case 5:
      // -0.5
        targetspeed(-0.5);
        Serial.print("-0.5,");
        if (modesw == 3)
        {
          mode = 4;
        }
        else if (modesw == 4)
        {
          mode = 6;
        }
      break;

      case 6:
      // -1.5
        targetspeed(-1.5);
        Serial.print("-1.5,");
        if (modesw == 3)
        {
          mode = 5;
        }
        else if (modesw == 4)
        {
          mode = 6;
        }
      break;

      default:
      break;
    }

    if (modesw == 1)
    {
      enmtr();
      Serial.print("ENPID,");
    }
    else if (modesw == 2)
    {
      dimtr();
      Serial.print("DISABLE PID,");
    }

    Serial.print(mode) ;
    Serial.print('\n');

  }

  Serial.print("\n");
 // ioextoutput(mode);
  modeswtmp = modesw;
  modesw = swread();
  delay(50);
}
# 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\answ.ino"
/*****************************************************************************/
void enpid(void)
{
    digitalWrite(5,0x0);
}

void dipid(void)
{
    digitalWrite(5,0x1);
}

void enmtr(void)
{
    digitalWrite(7,0x0);
}

void dimtr(void)
{
    digitalWrite(7,0x1);
}
# 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\dac.ino"
# 2 "c:\\home\\Dropbox\\Arduino\\kagura2\\dac.ino" 2

void dacoutput(byte ch, u16 dat)
{
  u16 dat_tmp = 0;
  u8 shortdat = 0;
  //  CS LOW
  digitalWrite(10, 0x0);
  //  LOADDAC HIGH
  digitalWrite(11, 0x1);
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
  digitalWrite(10,0x1);
  //  LOADDAC LOW 
  digitalWrite(11, 0x0);
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
# 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\ioext.ino"
/*****************************************************************************/
/*                    PCF8574 */
/*****************************************************************************/
# 5 "c:\\home\\Dropbox\\Arduino\\kagura2\\ioext.ino" 2


byte ioext_inoutconf;
byte ioext_outputtmp;

void ioextsetinout(byte data)
{
    ioext_inoutconf = data;
}

void ioextoutput(byte data)
{
    ioext_outputtmp = data;
    Wire.beginTransmission(0x38);
//    Wire.write(data && ioext_inoutconf);
    Wire.write(data);
    Wire.endTransmission();
}

byte ioextinput(void)
{
    byte inputdata;
    Wire.requestFrom(0x38, 1);
    Wire.write(ioext_outputtmp && ioext_inoutconf);
    while (Wire.available())
    {
        inputdata = Wire.read();
    }
    return inputdata;
}
# 1 "c:\\home\\Dropbox\\Arduino\\kagura2\\sw.ino"
/*****************************************************************************/
# 15 "c:\\home\\Dropbox\\Arduino\\kagura2\\sw.ino"
byte swread(void)
{
    u16 tmp, adc0, adc1;
    adc0 = analogRead(8);
    delay(4);
    adc1 = analogRead(8);
    tmp = (adc0 + adc1) / 2;
    if (tmp < (u16)(100) /*  */)
    {
       return 1;
    }
    else if (tmp < (u16)(520) /*  */)
    {
        return 2;
    }
    else if (tmp < (u16)(700) /* */)
    {
        return 3;
    }
    if (tmp < (u16)(800) /*  */)
    {
       return 4;
    }
    else
    {
        return 0;
    }
}
