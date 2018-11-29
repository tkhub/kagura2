/*****************************************************************************/

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