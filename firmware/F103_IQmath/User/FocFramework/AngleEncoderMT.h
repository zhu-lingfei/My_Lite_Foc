/***********************************************************************
 * @file AngleEncoderMT.h
 * Description:
 *
 **********************************************************************/
#ifndef __AngleEncoderMT_h__
#define __AngleEncoderMT_h__

#include "stdint.h"
#include "FocControl.h"
#include "IQmathLib.h"

//MT6816 �Ĵ�����ַ
#define READ_STATUS            0x8001
#define READ_ANGLE_VALUE_1       0x8021
#define READ_ANGLE_VALUE_2       0x8021
#define READ_SPEED_VALUE       0x8031

//�Ƕ�ֵת��������ϵ��

#define FOC_EANGLE_TO_ERADIN _IQ19(0.017453293)


//��õ��1��ǰ���е����Ƕ���Ϣ
void GetMotor1_ElectricalAngle_MT6816(FOC_Struct *v);
float GetMotor1_mAngle(void);
void GetMotor1_preSpeed(FOC_Struct *pFOC);
#endif // __AngleEncoderMT_h__

