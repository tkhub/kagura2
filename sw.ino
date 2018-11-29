/*****************************************************************************/
#include "sw.h"
#define SW_GO_TH    (u16)(100)     /*  */
#define SW_ESC_TH   (u16)(520)     /*  */
#define SW_UP_TH    (u16)(700)     /* */
#define SW_DOWN_TH  (u16)(800)    /*  */

byte swnow;      //  スイッチの現在状態
byte swold;         //  スイッチの前回値    
byte  swbff[SW_BUFFSIZE];   //  スイッチの前状態
byte  swedge;     //  エッジがあるかないかを保存


void swinit()
{
    for (int cnt = 0; cnt < SW_BUFFSIZE; cnt++)
    {
        swbff[cnt] = SW_NON;
    }
    swedge = SW_EDGE_NON;
    swnow = SW_NON;
    swold = SW_NON;
}

byte swread()
{
    return swnow;
}

byte swreadold()
{
    return swold;
}

byte swreadedge()
{
    byte tmp = swedge;
    noInterrupts();
    swedge = SW_EDGE_NON;
    interrupts();
    return tmp;
}

void swintr()
{
    int cnt;
    swbff[0] = swreadad(); 
    //  sw buffer
    for (cnt = 1; cnt < (SW_BUFFSIZE - 1); cnt++)
    {
        swbff[cnt + 1] = swbff[cnt];
    }
    //  nois fillter
    for (cnt = 0; cnt < (SW_BUFFSIZE -1) ; cnt++)
    {
        if (swbff[cnt] != swbff[cnt+1])
        {
            break;
        }
    }
    if (cnt > SW_NOISCUT_LEN)
    {
        swnow = swbff[0];
        if ( (swold == SW_NON) && (swnow != SW_NON) )
        {
            swedge = SW_EDGE_UP;
        }
        else if ( (swold != SW_NON) && (swnow == SW_NON) )
        {
            swedge = SW_EDGE_DOWN;
        }        
        else
        {
            swedge = SW_EDGE_NON;
        }
        swold = swnow;
    }
}



byte swreadad(void)
{
    u16 tmp;
    tmp = analogRead(8);
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


