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