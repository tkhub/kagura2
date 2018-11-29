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