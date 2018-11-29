#include <SPI.h>
#include <Wire.h>
#include <MsTimer2.h>
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

#define SW_UP   3
#define SW_DOWN 4
#define SW_GO   1
#define SW_ESC  2 
#define SW_NON  0


void setupdio()
{
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
}

void setupperipheral()
{
  Serial.begin(9600);
  Serial.print("Serial Init");

  /* SPI初期化 */
  SPI.begin();
  /* MSB FAST */
  SPI.setBitOrder(MSBFIRST);
  /* 1MHz CLK */
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  /* クロック負パルス 立ち上がりエッジサンプリング */
  SPI.setDataMode(SPI_MODE3);

  Serial.print("SPI Init");

  MsTimer2::set(1, fastintvlint);
  MsTimer2::start();

//  Wire.begin();
}

void setupsoftware()
{
dacoutput(0,2048);
  dacoutput(1,2048);
  dacoutput(2,2048);
  dacoutput(3,2048);
  Serial.print("DAC OUT 0V");
  
  //lcd.begin();
//  lcd.print("hello!");

  Serial.print("Kagura2 Start");


}


void setup()
{
  // put your setup code here, to run once:

  setupdio();
  setupperipheral();
  // Software init
  setupsoftware();
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
u32   intvlcnt = 0;
void loop() 
{
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
  Serial.print(", ");
  Serial.print(intvlcnt);
  Serial.print("\n");
 // ioextoutput(mode);
  modeswtmp = modesw;
  modesw = swread();
  delay(50);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/**                        Intr                                            ***/
/*****************************************************************************/
/*****************************************************************************/

void fastintvlint()
{
  intvlcnt++;
}
