#include "Motor1ADC1.h"
#include "main.h"
#include "adc.h"

#include "stdio.h"
#include "FocSVPWM.h"


struct SMotor1ADC1_Struct
{
	uint32_t adDmaValue[5];		  //DMA原始数据
	int32_t adValue[4];			  //电流实际AD值
	_iq19 lastAdValue[4];	      //上次电流实际AD值
};
struct SMotor1ADC1_Struct gMotor1ADC1 = {0};

/*************************************************************
** Function name:       Motor1ADC1StartOnce
** Descriptions:        Motor1ADC开始一次DMA转换
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void Motor1ADC1StartOnce(void)
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)gMotor1ADC1.adDmaValue, 5);//采样五次，取平均值
}
/*************************************************************
** Function name:       Get_Bus_Voltage
** Descriptions:        计算母线电压，并更新SVPWM_Mod.udc和SVPWM_Mod.k
** Input parameters:    ADC原始数据
** Output parameters:   None
** Returned value:      母线电压
** Remarks:             None
*************************************************************/
_iq Update_Bus_Voltage(int32_t _value)
{
    _iq U_dc, Svpwm_k, Ts;
    _iq18 value_max, value;
    value_max = _IQ18(4096.0f);//32-18-1=13bit,_IQ18刚刚够存放4096
    Ts = _IQ(100);             //PWM频率10khz，ts = 100us
    value = _IQ18(_value);
    U_dc = _IQmpy(_IQ18toIQ(_IQdiv(value,value_max)),_IQ(6.1f));
    Svpwm_k = _IQdiv(_IQ19mpy(_IQ(1.732050f),Ts),U_dc);//SVPWM 系数K =  sqrt(3) * Ts / Udc
    Svpwm_Udc_update(U_dc,Svpwm_k);
    return U_dc;
}
/*************************************************************
** Function name:       Motor1ADCValueStorage
** Descriptions:        Motor1存储DMA获取到的数据
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
//int32_t ADa,ADb,ADc; //Debug
void Motor1ADC1ValueStorage(void)
{
    //chanel1->U相->C相
    //chanel2->V相->B相
    //chanel3->W相->A相
    //adDmaValue[0]->Udc
    //adDmaValue[1]->Uw
    //adDmaValue[2]->Uv
    gMotor1ADC1.adValue[0] =  gMotor1ADC1.adDmaValue[0];
	gMotor1ADC1.adValue[1] =  gMotor1ADC1.adDmaValue[1] - 1993;   //1993为电机不转时刻的ADC采样值  w
	gMotor1ADC1.adValue[2] =  gMotor1ADC1.adDmaValue[2] - 1983;    //v
	gMotor1ADC1.adValue[3] = 0 - gMotor1ADC1.adValue[1] - gMotor1ADC1.adValue[2];
}

void Motor_Phase_ADCValueStorage(uint16_t U, uint16_t V)
{
    /*1 -> U, 2 -> V, 2 -> W*/
    gMotor1ADC1.adValue[1] =  U - 1993;   //1993为电机不转时刻的ADC采样值  w
    gMotor1ADC1.adValue[2] =  V - 1983;    //v
    gMotor1ADC1.adValue[3] = 0 - gMotor1ADC1.adValue[1] - gMotor1ADC1.adValue[2];
}
void Motor_Bus_ADCValueStorage(void)
{
    uint32_t value = 0;
    for (uint8_t i = 0; i < 5; i++)
    {
        value += gMotor1ADC1.adDmaValue[i];
    }
    gMotor1ADC1.adValue[0] = (int32_t )value/5;
    Update_Bus_Voltage(gMotor1ADC1.adValue[0]);
}
/*************************************************************
** Function name:       GetMotor1ADC1PhaseXValue
** Descriptions:        获取 Motor13相电流
** Input parameters:    Phase：1：A相 2：B相 3：C相
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
_iq19 GetMotor1ADC1PhaseXValue(uint8_t Phase)
{
	if (Phase >= 4) {
		return 0;
	}
    _iq19 iqvalue;
    
    iqvalue = _IQ19mpy(_IQ19((float)gMotor1ADC1.adValue[Phase]),FILTER_KP) + _IQ19mpy(gMotor1ADC1.lastAdValue[Phase],(_IQ19(1)-FILTER_KP));
    gMotor1ADC1.lastAdValue[Phase] = iqvalue;
    iqvalue = _IQ19(gMotor1ADC1.adValue[Phase]);//想不通为什么又返回没有滤波的数据
	return iqvalue;
}





