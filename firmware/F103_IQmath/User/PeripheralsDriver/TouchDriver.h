/***********************************************************************
 * @file TouchDriver.h
 * Description:
 *
 **********************************************************************/
#ifndef __TouchDriver_h__
#define __TouchDriver_h__

#include "stdint.h"
#include "main.h"

#define KEY_NUMBER_ALL	2	//��������

#define POWER_KEY_PinRead  HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin) //��Դ����
#define SET_KEY_PinRead	   HAL_GPIO_ReadPin(KEY_2_GPIO_Port,KEY_2_Pin) //���ð���

//�����ṹ������
typedef enum
{
    NONE_KEY 	= 0x00,	//�ް���
    POWER_KEY = 0x01, //��Դ����
    SET_KEY  	= 0x02, //���ð���
}KeyName;

typedef enum
{
	SHORT_TOUCH = 0x00,	//�̰�
	LONG_TOUCH  = 0x01,	//����
}KeyActionName;

typedef struct
{
	unsigned int  CntTimer[KEY_NUMBER_ALL + 1];	//��������ʱ�仺��
	unsigned char Action;						//��������
}KeyInit;


uint8_t Touch_GetKey(void);
void TouchDriver_Loop(void);


#endif // __TouchDriver_h__
