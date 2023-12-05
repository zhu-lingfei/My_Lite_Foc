#include "CanBus.h"
#include "main.h"
#include "stdio.h"
#include "FocControl.h"


CANTxMsg_t TxMsg;  //���巢���ʼ�ʵ��
CANRxMsg_t RxMsg;  //��������ʼ�ʵ��
uint8_t rcvdFlag=0;

/*************************************************************
** Function name:      CAN1_Filter_Init
** Descriptions:       CANɸѡ������
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void CAN1_Filter_Init(void)
{
		CAN_FilterTypeDef CAN1_FilerConf;
	/*ɸѡ�������IdҪ���������ʹ�ã�
	���磺���������Ϊ 0x0101,��������0000|0001|0000|0001,��ʾ��ֻ���Ĵ�������һλ���Լ���9λ��
	��˵�����IdҪ��Ϊ 0x0000,��������0000|0000|0000|0000��
	�������ʵ�ʾ���ɸѡ����������ͨ����IdΪ
									  xxxx|xxx0|xxxx|xxx0  ��x��ʾ0����1�����ԣ�
	��˾���ɸѡ�������Id��Χ����Ҫ�������������
	*/
		CAN1_FilerConf.FilterIdHigh=0X0000;					   //����IdҪ���16λ
		CAN1_FilerConf.FilterIdLow=0X0000;					   //����IdҪ���16λ
		CAN1_FilerConf.FilterMaskIdHigh=0X0000;                //�����16λȫ����Ϊ0����ʾ������λ����Id��16λ��������
		CAN1_FilerConf.FilterMaskIdLow=0X0000;                 //�����16λȫ����Ϊ0����ʾ������λ����Id��16λ��������
		CAN1_FilerConf.FilterFIFOAssignment=CAN_FILTER_FIFO0;  //ɸѡ�����յ��ı��ķ��뵽FIFO0�У���Ϊ��������0
		CAN1_FilerConf.FilterActivation=ENABLE;                //ɸѡ��ʹ�ܣ�������
		CAN1_FilerConf.FilterMode=CAN_FILTERMODE_IDMASK;       //ɸѡ������ģʽ
		CAN1_FilerConf.FilterScale=CAN_FILTERSCALE_32BIT;      //������32λ��ʾ
		/*����˵��һ��Ϊʲô��0
			����һ��ɸѡ����28���顣һ��stm32���ӵ�can���蹲��һ��can����
			����F4ϵ�л�����������canģ�飬���ֻ��һ��can��ʱ������FilterBank��ѡ0-13������������0����ʵ��0-13�����ԡ�
			���������can����Ҫ��������ɸѡ���顣һ����FilterBank
			��
			can1��ɸѡ����ѡ��0-13����CAN1_FilerConf.FilterBank=0;	
			can2��ɸѡ����ѡ��14-27����CAN1_FilerConf.SlaveStartFilterBank=14;	
		*/
		CAN1_FilerConf.FilterBank=0;							
		CAN1_FilerConf.SlaveStartFilterBank=14;
		
		/*	�˴�&hcan1ָ����can1��ɸѡ�����ã���ʵ����can1��can2��ɸѡ�������ú��ˡ���Ϊ����can�ǹ��õġ�
			������ΪSTM32��˫·CAN���ù������飬
			���ҹ�������Ĵ�����CAN1���üĴ������������ǰ��ŵģ�HAL�⽫��Щ�Ĵ����ϲ���һ���ṹ����ʶ��ѡ�
			����ͨ������ "HAL_CAN_ConfigFilter(&hcan1,&CAN1_FilerConf)" ����canɸѡ��������Ч��
			�����ٵ���HAL_CAN_ConfigFilter(&hcan2,&CAN1_FilerConf)
		*/
		if(HAL_CAN_ConfigFilter(&hcan,&CAN1_FilerConf)!=HAL_OK)
		{
				Error_Handler();
		}
}
/*************************************************************
** Function name:      CAN1_Send_Msg
** Descriptions:       CAN���ͺ���
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
uint8_t CAN1_Send_Msg(CANTxMsg_t *msg,uint16_t mailbox_id,uint8_t *sendbuff)
{
		uint8_t id;
	
		msg->TxMessage.StdId=mailbox_id;    		//����id��
		msg->TxMessage.IDE=CAN_ID_STD; 				//�ʼ���id��ʽ����׼ΪCAN_ID_STD	  |	  ��չΪCAM_ID_EXT��
		msg->TxMessage.DLC=8;						//�ʼ����ݳ��� �˴�Ϊ8���ֽ�
		msg->TxMessage.RTR=CAN_RTR_DATA;			//����֡ һ�㶼������֡
		msg->TxMessage.TransmitGlobalTime=DISABLE;  //Ĭ��DISABLE
		for(id=0;id<8;id++)
		{
				msg->payload[id]=sendbuff[id];  //װ������
		}
		
		//�����ʼ� ע���ʼ��ʼ���Ϣ��id�ţ��ʼ����͵ȵȣ������������Ƿֿ����͵ģ����忴������亯������
		if(HAL_CAN_AddTxMessage(&hcan,&msg->TxMessage,msg->payload,&msg->mailbox)!=HAL_OK)
				return 0;
		else
				return 1;
}
/*��Ƶ��4���н�ΪʲôҪ�������ĺ������֣�����лص�����
���յ��ʼ����ջ���������������ǿ���RX0�жϣ�������ǵĻص���������
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
������ǿ�����RX1�жϣ���ô�ص���������void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);
*/
/*************************************************************
** Function name:      HAL_CAN_RxFifo0MsgPendingCallback
** Descriptions:       CAN���ջص�����
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	//��仰���ж϶Է���������Ϣ�Ƿ��������ӵ�can1����can2�������can1����ͨ�����߾�֪�����ĸ��豸���������ݡ�
        extern uint8_t flag;
		if(hcan->Instance==CAN1)
		{
			/*
				����ǻ�ȡ�ʼ��ĺ�������Ȼ���˽����жϣ���ô��˵�����յ�������������ǵ����·�������ȡ��
				�����ṩһ��RxMessage�Լ�payload�ֱ�����ʼ��ʼ���Ϣ��id�ţ��ʼ����͵ȵȣ����������ݡ�
			*/
            if(HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&(RxMsg.RxMessage),(RxMsg.payload))==HAL_OK)
            {
                rcvdFlag=1;
                uint8_t id = (RxMsg.RxMessage.StdId >> 7); // 4Bits ID & 7Bits Msg
                uint8_t cmd = RxMsg.RxMessage.StdId & 0x7F; // 4Bits ID & 7Bits Msg
                DoCanCmd(cmd, RxMsg.payload, RxMsg.RxMessage.DLC);
                //flag = RxMsg.payload[0];
            }
            else
                Error_Handler();
		}
		//HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
}

/*************************************************************
** Function name:      CanBus_Init
** Descriptions:       CAN��ʼ��
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void CanBus_Init(void)
{
	HAL_CAN_Start(&hcan);  //����CAN
  CAN1_Filter_Init();    //��ʼ��CANɸѡ��
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)  //����CAN�ж�
  {
    Error_Handler();
	}
}
/*************************************************************
** Function name:      CanBus_Loop
** Descriptions:       CAN����ѭ��
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void CanBus_Loop(void)
{
	uint8_t i;
	uint8_t U8_Angle[8]={0};   //����Ŀ��Ƕ�
    uint8_t SendMsg[8] ={0};   //can���͵�����
	if(rcvdFlag ==1)
	{
		rcvdFlag = 0;
		
		for(i=0;i<8;i++)
		{
			U8_Angle[i] = RxMsg.payload[i];
		}
		
		for(i=0;i<8;i++)
		{
			SendMsg[i] = U8_Angle[i];
		}
		
		CAN1_Send_Msg(&TxMsg,0x12,SendMsg);//can���͵�ǰ�Ƕ�
	}
}


void DoCanCmd(uint8_t _cmd, uint8_t* _data, uint32_t _len)
{
    switch (_cmd)
    {
        // 0x00~0x0F No Memory CMDs
        case 0x01:  // Enable Motor
            FocMotor_Enable(1);
            break;
        case 0x02:  // Disable Motor
            FocMotor_Enable(0);
            break;
        case 0x03:  //Set tarspeed
            Motor1SpeedPIDSetTar(_IQ19(_data[0]));
            break;

        default:
            break;
    }

}