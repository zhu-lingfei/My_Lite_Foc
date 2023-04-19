/***********************************************************************
 * @file FocControl.h
 * Description:
 *
 **********************************************************************/
#ifndef __FocControl_h__
#define __FocControl_h__

#include "stdint.h"
#include "IQmathLib.h"

// �ٶ�λ�û� PID�ṹ��
struct PID_Struct
{
    /* data */
    _iq19 kp;
    _iq19 ki;
    _iq19 kd;

    _iq19 pre;
    _iq19 tar;
    _iq19 bias;
    _iq19 lastBias;
    _iq19 err;
    _iq19 out;
    _iq19 outMax;
};
// ������ PI�ṹ��
struct PI_Struct
{
    /* data */
    _iq kp;
    _iq ki;
    _iq kd;

    _iq pre;
    _iq tar;
    _iq bias;
    _iq lastBias;
    _iq out;
    _iq outMax;
};

// FOC�ṹ�崴��
typedef struct
{
    uint8_t isEnable; //���ʹ�ܱ�־
    _iq polePairs;    //������

    _iq tariq;        //Ŀ��q����
    _iq tarid;        //Ŀ��i����
    _iq19 mAngle_Offect;//ƫ�ƽǶ�
	_iq19 mAngle;       //��е�Ƕ�
    _iq19 eAngle;       //��Ƕ�
    _iq eRadian;      //��ǶȻ���
    _iq eAngleSine;   //��Ƕ�����ֵ���ǻ��ȣ�
    _iq eAngleCosine; //��Ƕ�����ֵ���ǻ��ȣ�

    uint8_t iNum;     //a ������������
    _iq ia;           //a ��ʵ�ʵ���
    _iq ib;           //b ��ʵ�ʵ���
	_iq ic;           //c ��ʵ�ʵ���

    _iq I_Alpha;      //clarke�任�� * 2/3�� I_Alpha
    _iq I_Beta;       //clarke�任�� * 2/3�� I_Beta

    _iq I_Alpha_Svpwm;//park���任�� I_Alpha_Svpwm
    _iq I_Beta_Svpwm; //park���任�� I_Beta_Svpwm
    
    _iq Iq;           //park�任�� Iq
    _iq Id;           //park�任�� Id
    
    _iq19 tarSpeed;     //Ŀ��ת�� ��е�Ƕ�
    _iq19 preSpeed;     //��ǰת�� ��е�Ƕ�
		_iq19 tarPos;       //Ŀ��λ�� ��е�Ƕ�
		_iq19 prePos;       //��ǰλ�� ��е�Ƕ�
    struct PI_Struct idPID;    //id pid
    struct PI_Struct iqPID;    //iq pid
    struct PID_Struct sPID;    //speed pid
		struct PID_Struct pPID;    //position pid
 
}FOC_Struct;

extern FOC_Struct FOC_Mod;


void FocMotor_Enable(uint8_t able);
void FocControl_Init(void);
void FocControl_Loop(void);
void Motor1FOCConfig_Printf(void);
void Motor1SetTarIDIQ(_iq id,_iq iq);
void Motor1PosPIDSetTar(_iq19 tarPos);
void Motor1SpeedPIDSetTar(_iq19 tarSpeed);
void FocSpeedControl_Loop(void);
void FocPosControl_Loop(void);
_iq19 GetMotor_mAngle(void);

#endif // __FocControl_h__



