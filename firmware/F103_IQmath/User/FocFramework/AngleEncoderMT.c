#include "AngleEncoderMT.h"
#include "Motor1SPI1.h"
#include "Timer.h"
#include "main.h"


/*************************************************************
** Function name:       ReadTLE5012BAngle_IQ
** Descriptions:        ��ȡ�����ǰ��е�Ƕ�ֵ
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void GetMotor1_ElectricalAngle_MT6816(FOC_Struct *pFOC)
{
	uint8_t data_reg_1 = 0xFF;
    uint8_t data_reg_2 = 0xFF;
    uint16_t data_reg   = 0xFFFF;  //�Ĵ�����ȡ����
    
    _iq16 dataBuf_iq16;
	_iq19 mangle;
    _iq19 eAngleBuf;
    
	Motor1SPI1SetCS(0);                        //MT6816Ƭѡ����
	rt_hw_us_delay(1);
	Motor1SPI1WriteData(READ_ANGLE_VALUE_1);   //SPI��ȡ��λ�Ĵ���
	data_reg_1 = Motor1SPI1ReadData();
    
	Motor1SPI1WriteData(READ_ANGLE_VALUE_2);   //SPI��ȡ��λ�Ĵ��� 
	data_reg_2 = Motor1SPI1ReadData();
	Motor1SPI1SetCS(1);                        //MT6816Ƭѡ����
    
    data_reg_1 = ((uint16_t)data_reg_1) << 6;
    data_reg_2 = (((uint16_t)data_reg_2) & 0x00FC) << 2;
	data_reg = data_reg_1 + data_reg_2;//�Ĵ�������
    
    
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
