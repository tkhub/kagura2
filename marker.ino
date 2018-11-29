/*****************************************************************************/
#define MKR_BFFR_LN 32 

#define MKR_CN_MSK 0x01
#define MKR_GR_MSK 0x02

byte  mkrbffr[MKR_BFFR_LN];

byte mkrread()
{

}

void markerinit()
{
  for (int cnt = 0; cnt < MKR_BFFR_LN; cnt++)
  {
    mkrbffr[cnt] = 0x00;
  }
}

void markerintr()
{
  byte tmp = 0x00; 
  if (mkgr() == true)
  {
    tmp |= MKR_GR_MSK;
  }
  else
  {
    tmp &= ~MKR_GR_MSK;
  }
  if (mkcn() == true)
  {
    tmp |= MKR_CN_MSK;
  }
  else
  {
    tmp &= ~MKR_CN_MSK;
  }
  mkrbffr[0] = tmp;
  for (int cnt = 1; cnt < (MKR_BFFR_LN - 1); cnt++)
  {
    mkrbffr[cnt + 1] = mkrbffr[cnt];
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

bool  mkrcn(void)
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

bool  mkgr(void)
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