#include "FocControl.h"
#include "FocMath.h"
#include "AngleEncoder.h"
#include "FocSVPWM.h"
#include "Motor1GPIO.h"
#include "SysManage.h"



#include "stdio.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"


//FOCģ�ʹ��� FOC_Mod
FOC_Struct FOC_Mod = 
{
     0,         //���ʹ�ܱ�־
    _IQ(11),    //������

    _IQ(0),     //Ŀ��q����
    _IQ(0),     //Ŀ��i����
    _IQ19(14.501953), //ƫ�ƽǶȣ�У׼�̻���
	  _IQ19(0),   //��е�Ƕ�
    _IQ19(0),   //��Ƕ�
    _IQ19(0),   //��ǶȻ���
    _IQ(0),     //��Ƕ�����ֵ���ǻ��ȣ�
    _IQ(0),     //��Ƕ�����ֵ���ǻ��ȣ�
    
    3,          //������������
    _IQ(0),     //a ��ʵ�ʵ���
    _IQ(0),     //b ��ʵ�ʵ���
	  _IQ(0),     //c ��ʵ�ʵ���

    _IQ(0),     //clarke�任�� * 2/3�� I_Alpha
    _IQ(0),     //clarke�任�� * 2/3�� I_Beta

    _IQ(0),     //park���任�� I_Alpha_Svpwm
    _IQ(0),     //park���任�� I_Beta_Svpwm
    
    _IQ(0),     //park�任�� Iq
    _IQ(0),     //park�任�� Id
    
    _IQ19(0),   //tarSpeed
    _IQ19(0),   //preSpeed
    _IQ19(0),   //tarPosition
    _IQ19(0),   //preSpeed
    
    _IQ(0),     //id pid
    _IQ(0),     //iq pid
    _IQ19(0),   //speed pid
		_IQ19(0),   //position pid
};
/*************************************************************
** Function name:      FocPID_Init
** Descriptions:       FOCPID��ʼ��
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void FocPID_Init(void)
{
    //iq PID������ʼ��
    FOC_Mod.iqPID.kp       = _IQ(0.08);  //����
    FOC_Mod.iqPID.ki       = _IQ(0.008);  //����
    FOC_Mod.iqPID.kd       = _IQ(0);  //΢��
    FOC_Mod.iqPID.pre      = _IQ(0);  //ʵ��ֵ
    FOC_Mod.iqPID.tar      = _IQ(0);  //Ŀ��ֵ
    FOC_Mod.iqPID.bias     = _IQ(0);  //����ƫ��
    FOC_Mod.iqPID.lastBias = _IQ(0);  //�ϴ�ƫ��
    FOC_Mod.iqPID.out      = _IQ(0);  //PID���
    FOC_Mod.iqPID.outMax   = _IQ(7);  //PID����޷�
    //id PID������ʼ��
    FOC_Mod.idPID.kp       = _IQ(0.04);  //����
    FOC_Mod.idPID.ki       = _IQ(0.08);  //����
    FOC_Mod.idPID.kd       = _IQ(0);  //΢��
    FOC_Mod.idPID.pre      = _IQ(0);  //ʵ��ֵ
    FOC_Mod.idPID.tar      = _IQ(0);  //Ŀ��ֵ
    FOC_Mod.idPID.bias     = _IQ(0);  //����ƫ��
    FOC_Mod.idPID.lastBias = _IQ(0);  //�ϴ�ƫ��
    FOC_Mod.idPID.out      = _IQ(0);  //PID���
    FOC_Mod.idPID.outMax   = _IQ(4.8);  //PID����޷�
    //Speed PID������ʼ��
    FOC_Mod.sPID.kp        = _IQ19(4.8);  //����
    FOC_Mod.sPID.ki        = _IQ19(0.03);  //����
    FOC_Mod.sPID.kd        = _IQ19(0);  //΢��
    FOC_Mod.sPID.pre       = _IQ19(0);  //ʵ��ֵ
    FOC_Mod.sPID.tar       = _IQ19(0);  //Ŀ��ֵ
    FOC_Mod.sPID.bias      = _IQ19(0);  //����ƫ��
    FOC_Mod.sPID.lastBias  = _IQ19(0);  //�ϴ�ƫ��
    FOC_Mod.sPID.err       = _IQ19(0);  //�ۼ�ƫ��
    FOC_Mod.sPID.out       = _IQ19(0);  //PID���
    FOC_Mod.sPID.outMax    = _IQ19(2.6f);  //PID����޷�
    //Position PID������ʼ��
    FOC_Mod.pPID.kp        = _IQ19(0.2);  //����
    FOC_Mod.pPID.ki        = _IQ19(0.0);  //����
    FOC_Mod.pPID.kd        = _IQ19(2.2);  //΢��
    FOC_Mod.pPID.pre       = _IQ19(0);  //ʵ��ֵ
    FOC_Mod.pPID.tar       = _IQ19(0);  //Ŀ��ֵ
    FOC_Mod.pPID.bias      = _IQ19(0);  //����ƫ��
    FOC_Mod.pPID.lastBias  = _IQ19(0);  //�ϴ�ƫ��
    FOC_Mod.pPID.err       = _IQ19(0);  //�ۼ�ƫ��
    FOC_Mod.pPID.out       = _IQ19(0);  //PID���
    FOC_Mod.pPID.outMax    = _IQ19(4.0);  //PID����޷�
}

/*************************************************************
** Function name:      FocControl_Init
** Descriptions:       FOC���Ƴ�ʼ��
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void FocControl_Init(void)
{
    FocPID_Init();                        //FOC����PID������ʼ��
    FocMotor_Enable(0);                   //FOCʹ�ܳ�ʼ��  ʧ��
	  Motor1PosPIDSetTar(_IQ19(80));         //Ŀ��λ�ó�ʼ��0
	  Motor1SpeedPIDSetTar(_IQ19(0));       //Ŀ���ٶȳ�ʼ��0
    Motor1SetTarIDIQ(_IQ(0.0),_IQ(0.0));  //Ŀ�������ʼ��0
}

/*************************************************************
** Function name:      FocMotor_Enable
** Descriptions:       FOC�������ʹ��
** Input parameters:   200us
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void FocMotor_Enable(uint8_t able)
{
	 Motor1SetEnable(able);   //�������ʹ��
	  FOC_Mod.isEnable = 1;    //FOC����ʹ��
}

/*************************************************************
** Function name:      GetMotor_mAngle
** Descriptions:       ��ȡ�����ǰ��е�Ƕ�
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
_iq19 GetMotor_mAngle(void)
{
	return FOC_Mod.mAngle- _IQ19(180);
}


/*************************************************************
** Function name:       Motor1SetTarIDIQ
** Descriptions:        ���õ��1Ŀ�����
** Input parameters:    id��d�������λA
**						iq: q�������λA
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void Motor1SetTarIDIQ(_iq id,_iq iq)
{
    FOC_Mod.tarid = id;
    FOC_Mod.tariq = iq;
}
/*************************************************************
** Function name:       Motor1PosPIDSetTar
** Descriptions:        ���õ��1Ŀ��λ��
** Input parameters:    tarPos��Ŀ��λ�� -180 0 180
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void Motor1PosPIDSetTar(_iq19 tarPos)
{
    FOC_Mod.tarPos = tarPos;
}
/*************************************************************
** Function name:       Motor1SpeedPIDSetTar
** Descriptions:        ���õ��1Ŀ��ת��
** Input parameters:    tarSpeed��Ŀ��ת��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void Motor1SpeedPIDSetTar(_iq19 tarSpeed)
{
    FOC_Mod.tarSpeed = tarSpeed;
}
/*************************************************************
** Function name:      FocPosControl_Loop
** Descriptions:       FOCλ�ñջ�����ѭ��
** Input parameters:   200us
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void FocPosControl_Loop(void)
{
    PositionPIDControl(&FOC_Mod);
}

/*************************************************************
** Function name:      FocSpeedControl_Loop
** Descriptions:       FOC�ٶȱջ�����ѭ��
** Input parameters:   200us
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void FocSpeedControl_Loop(void)
{
    SpeedPIDControl(&FOC_Mod);
}

/*************************************************************
** Function name:      FocControl_Loop
** Descriptions:       FOC����ѭ��
** Input parameters:   100us
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void FocControl_Loop(void)
{
    GetMotor1_ElectricalAngleMT6816(&FOC_Mod);              //Step-> 1  ��ȡ��� ��ǰ���е����Ƕ���Ϣ
    CurrentReconstruction(&FOC_Mod);                        //Step-> 2  ��ȡ����������ع�����
    clarke_calc(&FOC_Mod);                                  //Step-> 3  Clarke�任
    park_calc(&FOC_Mod);                                    //Step-> 4  Park�任
    CurrentPIControlID(&FOC_Mod);                           //Step-> 5  ����PI�ջ����
    CurrentPIControlIQ(&FOC_Mod);
/*
    FOC_Mod.idPID.out = _IQ(0.0); //��������,��������һ��Ҫע�͵�
    FOC_Mod.iqPID.out = _IQ(7.0); //��������,��������һ��Ҫע�͵�
*/
    ipark_calc(&FOC_Mod);                                   //Step-> 6  ��park�任
    SvpwmCtrl(FOC_Mod.I_Alpha_Svpwm,FOC_Mod.I_Beta_Svpwm);  //Step-> 7  ����SVPWM
}
/*************************************************************
** Function name:      FocControl_Loop
** Descriptions:       FOC����ѭ��
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
float Tar_Iq,Tar_Id;
float Pre_Iq,Pre_Id;
float Out_Iq,Out_Id;
float Bias_Iq,Bias_Id;
void Motor1FOCConfig_Printf(void)
{
    Tar_Iq = _IQtoF(FOC_Mod.iqPID.tar);
    Tar_Id = _IQtoF(FOC_Mod.idPID.tar);
    Pre_Iq = _IQtoF(FOC_Mod.iqPID.pre);
    Pre_Id = _IQtoF(FOC_Mod.idPID.pre);
    Out_Iq = _IQtoF(FOC_Mod.iqPID.out);
    Out_Id = _IQtoF(FOC_Mod.idPID.out);
    Bias_Iq = _IQtoF(FOC_Mod.iqPID.bias);
    Bias_Id = _IQtoF(FOC_Mod.idPID.bias);
    
//   printf("1:%f\r\n",_IQ19toF(FOC_Mod.preSpeed)); 
//    printf("2:%f\r\n",_IQ19toF(FOC_Mod.mAngle));   
    printf("%f,%f,%f,%f\n", Tar_Id, Pre_Id,Out_Id,Bias_Id);
    //printf("2:%f\r\n",Pre_Iq);
    
//    printf("1:%f\r\n",_IQ19toF(FOC_Mod.sPID.tar));
//    printf("2:%f\r\n",_IQ19toF(FOC_Mod.sPID.pre));
//    printf("3:%f\r\n",_IQ19toF(FOC_Mod.sPID.bias));
//    printf("4:%f\r\n",_IQ19toF(FOC_Mod.sPID.lastBias));
//    printf("5:%f\r\n",_IQ19toF(FOC_Mod.sPID.out));

//    printf("3:%f\r\n",_IQtoF(FOC_Mod.ia));
//    printf("4:%f\r\n",_IQtoF(FOC_Mod.ib));
//    printf("5:%f\r\n",_IQtoF(FOC_Mod.I_Alpha));
//    printf("6:%f\r\n",_IQtoF(FOC_Mod.I_Beta));
//    
//    printf("5:%f\r\n",_IQtoF(SVPWM_Mod.u1));
//    printf("6:%f\r\n",_IQtoF(SVPWM_Mod.u2));
//    printf("7:%f\r\n",_IQtoF(SVPWM_Mod.u3));
//    
//    printf("8:%f\r\n",_IQ19toF(FOC_Mod.mAngle));
//		printf("9:%f\r\n",_IQ19toF(FOC_Mod.eAngle));
//    
//    printf("9:%d\r\n",SVPWM_Mod.sector);
//    printf("10:%f\r\n",_IQtoF(SVPWM_Mod.u1));
//    printf("11:%f\r\n",_IQtoF(SVPWM_Mod.u2));
//    printf("12:%f\r\n",_IQtoF(SVPWM_Mod.u3));

}




