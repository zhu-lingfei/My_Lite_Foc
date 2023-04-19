#include "FocMath.h"
#include "Motor1ADC1.h"
#include "FocSVPWM.h"
#include "AngleEncoder.h"

//float Ia,Ib,Ic; //Debug
/*************************************************************
** Function name:       GetMotor1PreCurrent
** Descriptions:        ��ȡ���1 3�����ֵ
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void GetMotor1PreCurrent(_iq *ua,_iq *ub,_iq *uc)
{
	*ua = _IQmpy(_IQ19toIQ(_IQ19mpy(GetMotor1ADC1PhaseXValue(1),AD_TO_CURRENT)) , _IQ(1.5));
	*ub = _IQmpy(_IQ19toIQ(_IQ19mpy(GetMotor1ADC1PhaseXValue(2),AD_TO_CURRENT)) , _IQ(1.5));
	*uc = _IQmpy(_IQ19toIQ(_IQ19mpy(GetMotor1ADC1PhaseXValue(3),AD_TO_CURRENT)) , _IQ(1.5));
    /*
    *ua = _IQmpy(_IQ19toIQ(_IQ19mpy(GetMotor1ADC1PhaseXValue(1),AD_TO_CURRENT)) , _IQ(1.5));
	*ub = _IQmpy(_IQ19toIQ(_IQ19mpy(GetMotor1ADC1PhaseXValue(3),AD_TO_CURRENT)) , _IQ(1.5));
	*uc = _IQmpy(_IQ19toIQ(_IQ19mpy(GetMotor1ADC1PhaseXValue(2),AD_TO_CURRENT)) , _IQ(1.5));
    */
	
//	Ia = _IQtoF(*ua); //Debug
//	Ib = _IQtoF(*ub); //Debug
//	Ic = _IQtoF(*uc); //Debug
}
/*************************************************************
** Function name:       CurrentReconstruction
** Descriptions:        �����ع�
** Input parameters:    pFOC:�ṹ��ָ��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void CurrentReconstruction(FOC_Struct *pFOC)
{
    GetMotor1PreCurrent(&pFOC->ia,&pFOC->ib,&pFOC->ic);
	if (pFOC->iNum < 3) {
        return;
    }
    switch (GetSVPWMSector()) {
        case 1:
            pFOC->ia =_IQ(0.0f) - pFOC->ib - pFOC->ic;
            break;
        case 2:
            pFOC->ib =_IQ(0.0f) - pFOC->ia - pFOC->ic;
            break;
        case 3:
            pFOC->ib =_IQ(0.0f) - pFOC->ia - pFOC->ic;
            break;
        case 4:
            pFOC->ic =_IQ(0.0f) - pFOC->ia - pFOC->ib;
            break;
        case 5:
            pFOC->ic =_IQ(0.0f) - pFOC->ia - pFOC->ib;
            break;
        case 6:
            pFOC->ia =_IQ(0.0f) - pFOC->ib - pFOC->ic;
            break;
        default:
            break;
    }
}
/*************************************************************
** Function name:      ipark_calc
** Descriptions:       park���任
** Input parameters:   none
** Output parameters:  ���I_Alpha_Svpwm��I_Beta_Svpwm ����SVPWM
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void ipark_calc(FOC_Struct *pFOC)
{  
     pFOC->I_Alpha_Svpwm = _IQmpy(pFOC->idPID.out,pFOC->eAngleCosine) - _IQmpy(pFOC->iqPID.out,pFOC->eAngleSine);
     pFOC->I_Beta_Svpwm  = _IQmpy(pFOC->iqPID.out,pFOC->eAngleCosine) + _IQmpy(pFOC->idPID.out,pFOC->eAngleSine);
}

/*************************************************************
** Function name:      park_calc
** Descriptions:       park�任
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void park_calc(FOC_Struct *pFOC)
{
     pFOC->Id = _IQmpy(pFOC->I_Alpha,pFOC->eAngleCosine) + _IQmpy(pFOC->I_Beta,pFOC->eAngleSine);
     pFOC->Iq = _IQmpy(pFOC->I_Beta,pFOC->eAngleCosine) - _IQmpy(pFOC->I_Alpha,pFOC->eAngleSine);
}

/*************************************************************
** Function name:      clarke_calc
** Descriptions:       clarke�任
** Input parameters:   none
** Output parameters:  none
** Returned value:     none
** Created by:         none
** Created date:       none
*************************************************************/
void clarke_calc(FOC_Struct *pFOC)
{
     pFOC->I_Alpha = pFOC->ia;
     pFOC->I_Beta  = _IQmpy((pFOC->ia + _IQmpy(_IQ(2),pFOC->ib)) , FOC_SSQRT_3);
}

/*************************************************************
** Function name:       CurrentPIControlID
** Descriptions:        D������ջ�
** Input parameters:    pFOC:�ṹ��ָ��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void CurrentPIControlID(FOC_Struct *pFOC)
{
    //��ȡʵ��ֵ
    pFOC->idPID.pre = pFOC->Id ;
    //��ȡĿ��ֵ
    pFOC->idPID.tar = pFOC->tarid;
    //����ƫ��
    pFOC->idPID.bias = pFOC->idPID.tar - pFOC->idPID.pre;
    //����PID���ֵ
    pFOC->idPID.out += _IQmpy(pFOC->idPID.kp, (pFOC->idPID.bias - pFOC->idPID.lastBias)) + _IQmpy(pFOC->idPID.ki, pFOC->idPID.bias);
    //����ƫ��
    pFOC->idPID.lastBias = pFOC->idPID.bias;

    if (pFOC->idPID.out > _IQabs(pFOC->idPID.outMax)) {
        pFOC->idPID.out = _IQabs(pFOC->idPID.outMax);
    }

    if (pFOC->idPID.out < -_IQabs(pFOC->idPID.outMax)) {
        pFOC->idPID.out = -_IQabs(pFOC->idPID.outMax);
    }
}
/*************************************************************
** Function name:       CurrentPIControlIQ
** Descriptions:        Q������ջ�
** Input parameters:    pFOC:�ṹ��ָ��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void CurrentPIControlIQ(FOC_Struct *pFOC)
{
    
    //��ȡʵ��ֵ
    pFOC->iqPID.pre = pFOC->Iq;
    //��ȡĿ��ֵ
    pFOC->iqPID.tar = pFOC->tariq;
    //����ƫ��
    pFOC->iqPID.bias = pFOC->iqPID.tar - pFOC->iqPID.pre;
    //����PID���ֵ
    pFOC->iqPID.out += _IQmpy(pFOC->iqPID.kp, (pFOC->iqPID.bias - pFOC->iqPID.lastBias)) + _IQmpy(pFOC->iqPID.ki, pFOC->iqPID.bias);
    //����ƫ��
    pFOC->iqPID.lastBias = pFOC->iqPID.bias;

    if (pFOC->iqPID.out > _IQabs(pFOC->iqPID.outMax)) {
        pFOC->iqPID.out = _IQabs(pFOC->iqPID.outMax);
    }

    if (pFOC->iqPID.out < -_IQabs(pFOC->iqPID.outMax)) {
        pFOC->iqPID.out = -_IQabs(pFOC->iqPID.outMax);
    }
}

/*************************************************************
** Function name:       SpeedPIDControl
** Descriptions:        �ٶ�PID����
** Input parameters:    pFOC:�ṹ��ָ��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
//float PreSpeed,TarSpeed; //Debug
void SpeedPIDControl(FOC_Struct *pFOC)
{
    GetMotor1_preSpeed(pFOC);   //��ȡ��ǰת��
    pFOC->sPID.tar = pFOC->tarSpeed;
    pFOC->sPID.pre = pFOC->preSpeed;
    pFOC->sPID.bias = pFOC->sPID.tar - pFOC->sPID.pre;
    pFOC->sPID.out += (_IQ19mpy(pFOC->sPID.kp,(pFOC->sPID.bias - pFOC->sPID.lastBias)) +  _IQ19mpy(pFOC->sPID.ki , pFOC->sPID.bias));
    pFOC->sPID.lastBias = pFOC->sPID.bias;

	if (pFOC->sPID.out > _IQ19abs(pFOC->sPID.outMax)) {
		pFOC->sPID.out = _IQ19abs(pFOC->sPID.outMax);
	} else if (pFOC->sPID.out < -_IQ19abs(pFOC->sPID.outMax)) {
		pFOC->sPID.out = -_IQ19abs(pFOC->sPID.outMax);
	}
  if(pFOC->isEnable == 1)  Motor1SetTarIDIQ(_IQ(0),_IQ19toIQ(pFOC->sPID.out));  //����Id��Iq������������
  else Motor1SetTarIDIQ(_IQ(0),_IQ(0));
	
//		PreSpeed = _IQ19toF(pFOC->sPID.pre);  //Debug
//		TarSpeed = _IQ19toF(pFOC->sPID.tar);  //Debug
}
/*************************************************************
** Function name:       PositionPIDControl
** Descriptions:        λ��PID����
** Input parameters:    pFOC:�ṹ��ָ��
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
float PrePos,TarPos,mAngle; //Debug
void PositionPIDControl(FOC_Struct *pFOC)
{
	  _iq19 pre;
    pFOC->pPID.pre = GetMotor1_prePosition(pFOC);   //��ȡ��ǰλ��
	  pFOC->pPID.tar = pFOC->tarPos;                  //��ȡĿ��λ��
	  
	  //��ת����ϵ
	  pre = pFOC->pPID.pre - pFOC->pPID.tar;
		if(pre > _IQ19(180)) pre -= _IQ19(360);
		else if(pre < _IQ19(-180))  pre += _IQ19(360);

    
    //pFOC->pPID.bias = pFOC->pPID.tar - pFOC->pPID.pre;
	  pFOC->pPID.bias = _IQ19(0) - pre;
    pFOC->pPID.out  = _IQ19mpy(pFOC->pPID.kp , pFOC->pPID.bias) + _IQ19mpy(pFOC->pPID.kd,(pFOC->pPID.bias - pFOC->pPID.lastBias)) +  _IQ19mpy(pFOC->pPID.ki , pFOC->pPID.err);
    pFOC->pPID.lastBias = pFOC->pPID.bias;
	  pFOC->pPID.err  += pFOC->pPID.bias;

		if (pFOC->pPID.out > _IQ19abs(pFOC->pPID.outMax)) {
			pFOC->pPID.out = _IQ19abs(pFOC->pPID.outMax);
		} else if (pFOC->pPID.out < -_IQ19abs(pFOC->pPID.outMax)) {
			pFOC->pPID.out = -_IQ19abs(pFOC->pPID.outMax);
		}
		if(pFOC->isEnable == 1)  Motor1SpeedPIDSetTar(pFOC->pPID.out);  //����Ŀ���ٶȣ������ٶȻ�
		else Motor1SpeedPIDSetTar(_IQ19(0));

//		PrePos = _IQ19toF(pFOC->pPID.pre);  //Debug
//		TarPos = _IQ19toF(pFOC->pPID.tar);  //Debug
//		mAngle = _IQ19toF(pFOC->mAngle);
}


