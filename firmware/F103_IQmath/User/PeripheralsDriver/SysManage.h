/***********************************************************************
 * @file SysManage.h
 * Description:
 *
 **********************************************************************/
#ifndef __SysManage_h__
#define __SysManage_h__

#include "stdint.h"
#include "main.h"

//ϵͳ���� �ṹ������
typedef enum
{
    Startup_Check 	 = 0x00,	  //�����Լ�
    Debug_Action     = 0x01,    //Debug����
    StopRun_Action   = 0x02,    //ֹͣ����
    NormalRun_Action = 0x03,    //��������
    ErrWarning_Action= 0x04,    //������
}SystemAction;

typedef struct
{
    uint8_t StateAction;
}SystemState;

extern SystemState Sys;

uint8_t Get_SysState(void);
void Set_SysState(SystemAction SysAction);
void SysManage_Loop(void);
#endif // __SysManage_h__

