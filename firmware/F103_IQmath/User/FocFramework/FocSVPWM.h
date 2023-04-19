/***********************************************************************
 * @file SVPWM.h
 * Description:
 *
 **********************************************************************/
#ifndef __SVPWM_h__
#define __SVPWM_h__

#include "stdint.h"
#include "IQmathLib.h"
#include "FocControl.h"


typedef struct 
{
    _iq uAlpha; //��������Ŀ���ѹ 	(���ֵΪĸ�ߵ�ѹ * sqrt(3) / 3)
    _iq uBeta;  //������Ŀ���ѹ   	(���ֵΪĸ�ߵ�ѹ * sqrt(3) / 3)
    _iq u1;		//���������ж�
    _iq u2;		//���������ж�
    _iq u3;		//���������ж�
    _iq t0;		//0ʸ������ʱ��
    _iq t1;		//1ʸ������ʱ��
    _iq t2;		//2ʸ������ʱ��
    _iq t3;		//3ʸ������ʱ��
    _iq t4;		//4ʸ������ʱ��
    _iq t5;		//5ʸ������ʱ��
    _iq t6;		//6ʸ������ʱ��
    _iq t7;		//7ʸ������ʱ��
    _iq ta;		//a������ʱ��
    _iq tb;		//b������ʱ��
    _iq tc;		//c������ʱ��
    _iq ts;		//SVPWM����
    _iq K;		//SVPWM ϵ��K =  sqrt(3) * Ts / Udc
    _iq udc;       //ĸ�ߵ�ѹ
    uint8_t sector;//��������
}Svpwm_Struct;
extern Svpwm_Struct SVPWM_Mod;


void SvpwmCtrl(_iq uAlpha,_iq uBeta);
uint8_t GetSVPWMSector(void);

#endif // __SVPWM_h__

