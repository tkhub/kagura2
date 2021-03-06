/*****************************************************************************/
/*****************************************************************************/
/*                   marker system                                           */
/*****************************************************************************/
/*****************************************************************************/
#include "marker.h"
#define MKR_ST_WAIT_MKR 1
#define MKR_ST_WAIT_JDG 2
#define MKR_ST_WAIT_MSK 3

/*
１．マーカー変化のエッジをとらえたい。
２．マーカーがどんな状態なのかを通知したい。
３．スタート・ゴールマーカは他のマーカ検出前後には検出しないこと
４．クロスライン検出



*/

byte mkrsq;        //  マーカーセンサシーケンサ
int mkrsqcnt;     //  マーカーセンサシーケンサ用カウンタ
u32 mkrcnt;     //  マーカーセンサシーケンサ用カウンタ2
byte mkrchk;

int mkrgrhyst;  // ゴールマーカー履歴
int mkrcnhyst;  // コーナーマーカー履歴
int mkrcrshyst;  // クロスライン履歴

byte mkrst;       //  マーカーセンサ状態
bool mkrivnt;     //  マーカーセンサイベント

byte mkrread()
{
  return mkrst;
}

bool mkrreadivnt()
{
    if (mkrivnt)
    {
      noInterrupts();
      mkrivnt = false;
      interrupts();
      return true;
    }
    else
    {
      return false;
    }
}

void mkrinit()
{
  mkrsq = MKR_ST_WAIT_MKR;
  mkrsqcnt = 0;
  mkrcnt = 0;
  mkrgrhyst = 0;
  mkrcnhyst = 0;
  mkrcrshyst = 0;
  mkrivnt = false;
}

void mkrmntr()
{
    Serial.print("<<mkr>>[st=");
    Serial.print(mkrst);
    Serial.print(",ivnt=");
    Serial.print(mkrivnt);
    Serial.print(",sq=");
    Serial.print(mkrsq);

    Serial.print(",cnh=");
    Serial.print(mkrcnhyst);
    Serial.print(",grh=");
    Serial.print(mkrgrhyst);
    Serial.print(",crs=");
    Serial.print(mkrcrshyst);

    Serial.print(",cnt=");
    Serial.print(mkrsqcnt);
    Serial.print(",cnt2=");
    Serial.print(mkrcnt);
    Serial.print("]<<mkr>>");
}
void mkrintr()
{
  bool t_mkflg = false;
  mkrcnt++;
  switch(mkrsq)
  {
    case MKR_ST_WAIT_MKR:
    // マーカ待ち 
      if ( mkrcn() && mkrgr() )
      {
        // クロスライン履歴保存
        mkrcrshyst++;
      }
      else if (mkrcn()) 
      {
        // コーナー履歴保存
        mkrcnhyst++;
      }
      else if (mkrgr())
      {
        //  ゴール履歴保存
        mkrgrhyst++;
      }
      else
      { 
        // Nothing to DO!
      }
      if (1 < mkrgrhyst || 1 < mkrcnhyst|| 1 < mkrcrshyst)
      {
        mkrsq = MKR_ST_WAIT_JDG;
        mkrsqcnt = 0;
        mkrcnt = 0;
      }
    break;

    case MKR_ST_WAIT_JDG:
    // JDGE 待ち 
      // マーカー取得し履歴を保存
      if ( mkrcn() && mkrgr() )
      {
        // クロスライン履歴保存
        mkrcrshyst++;
      }
      else if (mkrcn()) 
      {
        // コーナー履歴保存
        mkrcnhyst++;
      }
      else if (mkrgr())
      {
        //  ゴール履歴保存
        mkrgrhyst++;
      }
      else
      { 
        // Nothing to DO!
      }
      // マーカー取得し履歴を保存

      if (mkrsqcnt < MKR_WAIT_MAX)
      {
         mkrsqcnt++;
      }

      if (MKR_JDG_WAIT < mkrsqcnt)
      {
        // JDG時間経過
        if (1 < mkrcrshyst)
        {
          mkrst = MKR_CRS;
          mkrivnt = true;
        }
        else if (1 < mkrcnhyst)
        {
          mkrst = MKR_CN;
          mkrivnt = true;
        }
        else if (1 < mkrgrhyst)
        {
          mkrst = MKR_GR;
          mkrivnt = true;
        }
        else
        {
          mkrst = MKR_NON;
          mkrivnt = false;
        }
        if (!mkrcn() && !mkrgr())
        {
          mkrsq = MKR_ST_WAIT_MSK;
          mkrsqcnt = 0;
          mkrcnt = 0;
          mkrcnhyst = 0;
          mkrcrshyst = 0;
          mkrgrhyst = 0;
        }
      }
    break;
  
    case MKR_ST_WAIT_MSK:
    // MASK待ち 
      if (mkrsqcnt < MKR_WAIT_MAX)
      {
        mkrsqcnt++;
      }
      if (MKR_MSK_WAIT < mkrsqcnt)
      {
        mkrsqcnt = 0;
        mkrcnt = 0;
        mkrst = MKR_NON;
        mkrsq = MKR_ST_WAIT_MKR;
      }
    break;

    default :
      mkrsq = MKR_ST_WAIT_MKR;
      mkrsqcnt = 0;
      mkrcnt = 0;
      mkrst = MKR_NON;
      mkrgrhyst = 0;
      mkrcnhyst = 0;
      mkrcrshyst = 0;
    break;
  }

}

bool  mkrgr(void)
{
  if (digitalRead(13) == HIGH)
  {
    return false;
  }
  else
  {
    return true;
  }
}

bool mkrcn(void)
{
  if (digitalRead(12) == HIGH)
  {
    return false;
  }
  else
  {
    return true;
  }
}

