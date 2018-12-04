#include <SPI.h>
#include <Wire.h>
#include <MsTimer2.h>
#include <TimerOne.h>
#include "sw.h"
#include "marker.h"
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
  Serial.begin(38400);
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
  //Timer1.initialize(500);
  //Timer1.attachInterrupt(fastintvlint);
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
  swinit();
  mkrinit();
  Serial.print("Kagura2 Start");


}


void setup()
{
  // put your setup code here, to run once:

  setupdio();
  setupperipheral();
  // Software init
  setupsoftware();
  MsTimer2::start();
}




/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/**                        loop                                            ***/
/*****************************************************************************/
/*****************************************************************************/
#define RUN_SQ_GOWAIT     1
#define RUN_SQ_STARTWAIT  2
#define RUN_SQ_STGTWAIT   3
#define RUN_SQ_GRGTWAIT   4
#define RUN_SQ_SLWWAIT    5
#define RUN_SQ_STPWAIT    6
#define RUN_SQ_STPEND     7
#define TGTSPEED_LEN      3
#define RN_SQ_CNTMAX      10000
#define RN_SQ_CNTMSK      500   // RNSQCNTMSK x 10ms
#define RN_SQ_CNTRSTVAL   10   // x 10ms

byte modesw = SW_NON;
int run_sq;
int timecnt;
int run_spd_md;
u32   intvlcnt = 0;
float tgtspeed[] ={1.0, 1.25, 1.5, 1.6};
void loop(){
  // put your main code here, to run repeatedly:
  switch(run_sq)
  {
    case RUN_SQ_GOWAIT:
      if(swreadedge() == SW_EDGE_UP)
      {
        modesw = swread();
        if (modesw == SW_UP)
        {
          run_spd_md++;
          if ((sizeof(tgtspeed)/sizeof(tgtspeed[0])) <= run_spd_md)
          {
            run_spd_md = sizeof(tgtspeed)/sizeof(tgtspeed[0]) - 1;
          }
          Serial.print(run_spd_md);
          Serial.print(" speed UP\n");
        }
        else if (modesw == SW_DOWN)
        {
          run_spd_md--;
          if (run_spd_md < 0);
          {
            run_spd_md == 0;
          } 
          Serial.print(run_spd_md);
          Serial.print(" speed DOWN\n");
        }
        else if (modesw == SW_GO)
        {
          run_sq = RUN_SQ_STARTWAIT;
          Serial.print("speed=");
          Serial.print(tgtspeed[run_spd_md]);
          Serial.print(",<GOWAIT -> STARTWAIT>\n");
          mkrreadivnt();
          swreadedge();
          timecnt = 0;
        }
      }
    break;

    case RUN_SQ_STARTWAIT:
      timecnt++;
      if (100 < timecnt)
      {
        run_sq = RUN_SQ_STGTWAIT;
        targetspeed(0.5);
        enpid();
        enmtr();
        Serial.print(",<STARTWAIT -> STGTWAIT>\n");
        timecnt = 0;
      }
    break;

    case RUN_SQ_STGTWAIT:
      if (mkrreadivnt())
      {
        if (mkrread() == MKR_GR)
        {
          targetspeed(tgtspeed[run_spd_md]);
          run_sq = RUN_SQ_GRGTWAIT;
          Serial.print(",<STGTWAIT -> GRGTWAIT>\n");
          timecnt = 0;
        }
      }
    break;

    case RUN_SQ_GRGTWAIT:
      if (timecnt < RN_SQ_CNTMAX)
      {
        timecnt++;
      }
      if (mkrreadivnt() && (RN_SQ_CNTMSK < timecnt ))
      {
        if (mkrread() == MKR_GR)
        {
          run_sq = RUN_SQ_SLWWAIT;
          targetspeed(0.5);
          Serial.print(",<GRGTWAIT -> SLWWAIT>\n");
          timecnt = 0;
        }
        else
        {
          timecnt = RN_SQ_CNTMSK - RN_SQ_CNTRSTVAL;
        }
      }
    break;
    
    case RUN_SQ_SLWWAIT:
      if (timecnt < 10000)
      {
        timecnt++;
      }
      if (10 < timecnt)
      {
        run_sq = RUN_SQ_STPWAIT;
        targetspeed(0.0);
        Serial.print(",<SLWWAIT -> STPWAIT>\n");
        timecnt = 0;
      }
    break;

    case RUN_SQ_STPWAIT:
      if (timecnt < RN_SQ_CNTMAX)
      {
        timecnt++;
      }
      if(10 < timecnt)
      {
        run_sq = RUN_SQ_STPEND;
        dimtr();
        dipid();
        Serial.print(",<STPWAIT -> END>\n");
        timecnt = 0;
      }
    break; 

    case RUN_SQ_STPEND:
      if(swreadedge() == SW_EDGE_UP)
      {
        modesw = swread();
        if (modesw == SW_ESC)
        {
           run_sq = RUN_SQ_GOWAIT;
          Serial.print(",<RETURN to First>\n");
          timecnt = 0;
        }
      }

    break;

    default :
      run_sq = RUN_SQ_GOWAIT;
    break;
  }
//  Serial.print(", ");
//  Serial.print(intvlcnt);
//  Serial.print("\n");
 // ioextoutput(mode);
  delay(10);
}
/*


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/**                        Intr                                            ***/
/*****************************************************************************/
/*****************************************************************************/

void fastintvlint()
{
  swintr() ;
  mkrintr();
  intvlcnt++;
}
