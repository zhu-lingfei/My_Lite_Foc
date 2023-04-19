/***********************************************************************
 * @file UartBus.h
 * Description:
***********************************************************************/
#ifndef __UartBus_h__
#define __UartBus_h__

#include "stdint.h"
#include "usart.h"

// ���ڽṹ�崴��
typedef struct
{
    uint8_t  FrameStart1; //֡ͷ1
    uint8_t  FrameStart2; //֡ͷ2
    uint16_t FrameLength; //֡����
    uint8_t  FrameEnd;    //֡β
    
    uint8_t  FrameReceiveFalg;  //֡������ɱ�־λ
    uint16_t DMA_ReceiveLength; //DMA�������ݳ���
    
    uint8_t UartRxDataBuf[MaxDataBufSize]; //����֡���ݻ���
    uint8_t UartRxDataAct[MaxDataBufSize]; //���ڿ�������
 
}UartBus_Struct;

extern UartBus_Struct UartBus;

void UartBus_Init(void);
void UartBus_Loop(void);
    


#endif // __UartBus_h__
