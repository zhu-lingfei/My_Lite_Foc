#include "AngleEncoder.h"
#include "Motor1SPI1.h"
#include "Timer.h"
#include "main.h"

#include "stdio.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"


/*************************************************************
** Function name:       ReadTLE5012BAngle_IQ
** Descriptions:        TLE5012B��ȡ�����ǰ��е�Ƕ�ֵ
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void GetMotor1_ElectricalAngleTle5012(FOC_Struct *pFOC)
{
	uint16_t data = 0xFFFF;
  _iq16 dataBuf_iq16;
	_iq19 mangle;
  _iq19 eAngleBuf;
    
	Motor1SPI1SetCS(0);
	rt_hw_us_delay(1);
	Motor1SPI1WriteData(READ_ANGLE_VALUE);
	data = Motor1SPI1ReadData();
	Motor1SPI1SetCS(1);
    
	data &= 0x7FFF;//�Ĵ�������
	dataBuf_iq16 = _IQ16div(_IQ16((float)data),_IQ16(32767.0f));

	mangle = _IQ19mpy(_IQtoIQ19(_IQ16toIQ(dataBuf_iq16)), _IQ19(360.0f)) - pFOC->mAngle_Offect;
	if(mangle< _IQ19(0)) mangle += _IQ19(360.0f);
	
	eAngleBuf = _IQ19mpy(mangle,_IQtoIQ19(pFOC->polePairs));
	while(eAngleBuf>_IQ19(360.0)) eAngleBuf -= _IQ19(360.0);
	

	pFOC->mAngle  = mangle;                                       //��õ�ǰ��е�Ƕ�iq19
	pFOC->eAngle  = eAngleBuf;                                    //��õ�ǰ��Ƕ�iq19
	pFOC->eRadian = _IQ19mpy(pFOC->eAngle , FOC_EANGLE_TO_ERADIN);//��õ�ǰ��Ƕȣ����ȣ�iq19
	pFOC->eAngleSine   = _IQ19toIQ(_IQ19sin(pFOC->eRadian));      //��õ�ǰ��Ƕ�����ֵ�����ȣ�iq
	pFOC->eAngleCosine = _IQ19toIQ(_IQ19cos(pFOC->eRadian));      //��õ�ǰ��Ƕ�����ֵ�����ȣ�iq
}
/*************************************************************
** Function name:       GetMotor1_ElectricalAngleMT6816
** Descriptions:        MT6816��ȡ�����ǰ��е�Ƕ�ֵ
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/

void GetMotor1_ElectricalAngleMT6816(FOC_Struct *pFOC)
{
  _iq16 dataBuf_iq16;
	_iq19 mangle;
  _iq19 eAngleBuf;
	uint16_t data_r[3];
	
	Motor1SPI1SetCS(0);
	data_r[0] = Motor1SPI1WriteReadData(ANGLE_VALUE1); //��ȡ��λ
	Motor1SPI1SetCS(1);

	Motor1SPI1SetCS(0);
  data_r[1] = Motor1SPI1WriteReadData(ANGLE_VALUE2); //��ȡ��λ
	Motor1SPI1SetCS(1);
	
	data_r[2] = (((data_r[0] & 0x00FF)<<6)|((data_r[1]& 0x00FF)>>2)); //�ϳɼĴ�����������
	
	
	dataBuf_iq16 = _IQ16div(_IQ16((float)data_r[2]),_IQ16(16384.0f)); //����Ϊ 0-360��

	mangle = _IQ19mpy(_IQtoIQ19(_IQ16toIQ(dataBuf_iq16)), _IQ19(360.0f)) - pFOC->mAngle_Offect;  //��ȥ��ƫ�Ƕ� �õ���ǰ��ʵ��е�Ƕ�
	if(mangle< _IQ19(0)) mangle += _IQ19(360.0f);
	
	eAngleBuf = _IQ19mpy(mangle,_IQtoIQ19(pFOC->polePairs));
	while(eAngleBuf>_IQ19(360.0)) eAngleBuf -= _IQ19(360.0);
	
	pFOC->mAngle  = mangle;                                       //��õ�ǰ��е�Ƕ�iq19
	pFOC->eAngle  = eAngleBuf;                                    //��õ�ǰ��Ƕ�iq19
	pFOC->eRadian = _IQ19mpy(pFOC->eAngle , FOC_EANGLE_TO_ERADIN);//��õ�ǰ��Ƕȣ����ȣ�iq19
	pFOC->eAngleSine   = _IQ19toIQ(_IQ19sin(pFOC->eRadian));      //��õ�ǰ��Ƕ�����ֵ�����ȣ�iq
	pFOC->eAngleCosine = _IQ19toIQ(_IQ19cos(pFOC->eRadian));      //��õ�ǰ��Ƕ�����ֵ�����ȣ�iq
}
/*************************************************************
** Function name:       GetMotor1_mAngle
** Descriptions:        ��ȡ�����ǰ��е�Ƕ�ֵ
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
float GetMotor1FOC_mAngle(void)
{
    float mAngle;

    mAngle = _IQ19toF(FOC_Mod.mAngle);
    return mAngle;
}
/*************************************************************
** Function name:       GetMotor1_mAngle
** Descriptions:        ��ȡ�����ǰ��е�Ƕ�ֵ
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
float GetMotor1_mAngle(void)
{
    float mAngle;
	  GetMotor1_ElectricalAngleMT6816(&FOC_Mod);
    mAngle = _IQ19toF(FOC_Mod.mAngle);
    return mAngle;
}
/*************************************************************
** Function name:       GetMotor1_preSpeed
** Descriptions:        ��ȡ�����ǰ��е�Ƕ�ֵ
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void GetMotor1_preSpeed(FOC_Struct *pFOC)
{
    static _iq19 mAngle = _IQ19(0.0);
    static _iq19 lastmAngle = _IQ19(0.0);
    static _iq19 LastpreSpeed;
    _iq19 preSpeed;
    
    mAngle   = pFOC->mAngle;
    preSpeed = _IQ19mpy((mAngle - lastmAngle),_IQ19(0.9)) + _IQ19mpy(LastpreSpeed,_IQ19(0.1));
	
	  preSpeed = (mAngle - lastmAngle);
      
    if(preSpeed < _IQ19(-180.0))preSpeed = preSpeed + _IQ19(360.0);
    else if(preSpeed > _IQ19(180.0)) preSpeed = preSpeed - _IQ19(360.0);

    pFOC->preSpeed = preSpeed;  //���ٶȣ��ǶȲ�ֵ��ֵ����ṹ��ָ��
    
    lastmAngle = mAngle;     //�洢�ϴλ�е�Ƕ�
    LastpreSpeed = preSpeed; //�洢�ϴν��ٶȣ��ǶȲ�ֵ��
}
/*************************************************************
** Function name:       GetMotor1_prePosition
** Descriptions:        ��ȡ�����ǰλ�� -180 0 180
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
_iq19 GetMotor1_prePosition(FOC_Struct *pFOC)
{
	return (pFOC->mAngle - _IQ19(180));
}


