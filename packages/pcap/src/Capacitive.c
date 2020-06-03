#include "bsp.h"
#include "eeprom.h"
extern uint32_t CapRange;
extern uint32_t MinVarCap;

void CapCalibrate(void)
{
  uint8_t ptmp[2];
  uint16_t TempAir;
	rt_kprintf("CapCalibrate_comin %x \n",RunVar.CalState);
  if((CalibrateLow == RunVar.CalState) || (CalibrateHeight == RunVar.CalState))
  {
//    LED_Open();
//    OSTmrStart(&TMR_LED_Short_Light, &Err);
  }
  else if(Calibrated == RunVar.CalState)
  {
  
    RunVar.CalState = Def_State;
    TempAir = (uint16_t)((RunVar.TempInAir + 273.15) * 10);
    ptmp[1] = (uint8_t)TempAir;
    ptmp[0] = (uint8_t)(TempAir >> 8);
    if(ArrToHex(&Cur_Param[CAPMIN]) < ArrToHex(&Cur_Param[CAPMAX]))
    {
      //FilterStartFlag = Bit_RESET;
//      I2C1_WNBytesMul3T(EEPROM_ADDRESS, CAPMIN, 8, &Cur_Param[CAPMIN]);
//      I2C1_WriteNBytes(EEPROM_ADDRESS, CALIBTEMPER, 2, ptmp);
			Eeprom_WriteNBytes(CAPMIN, &Cur_Param[CAPMIN], 8);// 
			Eeprom_WriteNBytes(CALIBTEMPER, ptmp, 2);// 
                        
      ProductPara.CapMin = ArrToHex(&Cur_Param[CAPMIN]);
      ProductPara.CapMax = ArrToHex(&Cur_Param[CAPMAX]);
      ProductPara.CapRange = ProductPara.CapMax - ProductPara.CapMin;
      ProductPara.CalibTemp = TempAir;
			rt_kprintf("CapCalibrate_do %x %x %x\n",ProductPara.CapMin,ProductPara.CapMax,ProductPara.CapRange);

//      LED_Open();
//      ulLedStatue = 2;
//      OSTmrStart(&TMR_LED_Double_Light, &Err);
    }
    else
    {
//      LED_Open();
//      OSTmrStart(&TMR_LED_Short_Light, &Err);
    }
  }
}

