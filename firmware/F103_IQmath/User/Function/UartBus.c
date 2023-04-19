#include "UartBus.h"
#include "string.h"
#include "AngleEncoder.h"

//���ڽṹ�崴��
UartBus_Struct UartBus = {0};
/*************************************************************
** Function name:      UartBus_Init
** Descriptions:       Uart��ʼ��
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void UartBus_Init(void)
{
    UartBus.FrameStart1 = 0xFE; //֡ͷ1
    UartBus.FrameStart2 = 0x55; //֡ͷ2
    UartBus.FrameLength = 0x0A; //֡����
    UartBus.FrameEnd    = 0xFD; //֡β
    
    UartBus.FrameReceiveFalg  = 0x00;  //֡������ɱ�־λ
    UartBus.DMA_ReceiveLength = 0x00; //DMA�������ݳ���
}
/*************************************************************
** Function name:      UartBus_Loop
** Descriptions:       Uart����ѭ��
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void UartBus_Loop(void)
{
    static uint8_t Tx_Data[5];
    static int16_t PreAngle = 0;
    static uint8_t PreAngle_B = 0;
    static uint8_t PreAngle_S = 0;
    static uint8_t PreAngle_G = 0;
    
    
    if(UartBus.FrameReceiveFalg == 1)  //DMA����һ֡�������
    {
        UartBus.FrameReceiveFalg = 0;
        if((UartBus.UartRxDataBuf[0] == UartBus.FrameStart1)&&
            (UartBus.UartRxDataBuf[1] == UartBus.FrameStart2)&& 
             (UartBus.UartRxDataBuf[2] == UartBus.FrameLength)&&
              (UartBus.UartRxDataBuf[UartBus.FrameLength-1] == UartBus.FrameEnd))
        {
            memcpy(UartBus.UartRxDataAct,  UartBus.UartRxDataBuf,  UartBus.FrameLength);//�ṹ�建��ת������������
            memset(UartBus.UartRxDataBuf,0,MaxDataBufSize);                             //�ṹ�建������
            HAL_UART_Transmit_DMA(&huart3,UartBus.UartRxDataAct,UartBus.DMA_ReceiveLength);
        }
        
    }
    
    //����
    PreAngle = (int16_t)GetMotor1FOC_mAngle();

    PreAngle_B = PreAngle/100;
    PreAngle_S = PreAngle%100/10;
    PreAngle_G = PreAngle%10;
    
    Tx_Data[0] = 48 + PreAngle_B;
    Tx_Data[1] = 48 + PreAngle_S;
    Tx_Data[2] = 48 + PreAngle_G;
    Tx_Data[3] = 0x0d;
    Tx_Data[4] = 0x0a;
    
    
    //HAL_UART_Transmit_DMA(&huart3,Tx_Data,5);
}



