#include "SysManage.h"
#include "tim.h"


SystemState Sys = {0};


/*************************************************************
** Function name:       Get_SysState
** Descriptions:        ��ȡ��ǰϵͳ״̬
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
uint8_t Get_SysState(void)
{
  return Sys.StateAction;
}

/*************************************************************
** Function name:       Set_SysState
** Descriptions:        ���õ�ǰϵͳ״̬
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void Set_SysState(SystemAction SysAction)
{
    Sys.StateAction = SysAction;
}

/*************************************************************
** Function name:       SysManage_Loop
** Descriptions:        ���õ�ǰϵͳ״̬
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void SysManage_Loop(void)
{
	static uint16_t Ticktime = 0;
	
	switch(Sys.StateAction)
	{
		case Startup_Check:
			if(Ticktime < 200)Ticktime ++;
			else
			{
				HAL_TIM_Base_Start_IT(&htim1);   //����foc�ж�
				Sys.StateAction = NormalRun_Action;
			} 
			break;
		case Debug_Action:
			break;
		default:break;
	}
}








