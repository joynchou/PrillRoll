/************************************************************
* ��֯���ƣ�
* �ļ�����: K:\��Ƭ�����\���Ӵ���������\HARDWARE\DEVICES\MOTOR\STEP_MOTOR\STEP_MOTOR.C
* ����:  �ܳ���
* �汾:  1.0
* ����:  2017/07/11
* ����: �����������������
* ��ʷ�޸ļ�¼:
* <����> <ʱ��> <�汾 > <����>
*
***********************************************************/

#include "STEP_MOTOR.H"
#include "../../../COMMON_HARDWARE/fixedPulser.h"
#include "../../../BSP/GPIO.H"
#include "../../../BSP/USART1.h"
#include <stdlib.h>
#include <stdio.h>

#define STEP_MOTOR_NUM 2  //������Ҫʹ�ö��ٸ������������������Ͷ������ֱ���������ͬʱʹ��
#define	 PI  3.1415f   //pi��ֵ

static  StepMotor g_stepMotor[STEP_MOTOR_NUM];

//************************************
// Method:    setStepMotorWithPulse
// FullName:  setStepMotorWithPulse
// Access:    public 
// Returns:   bit
// Qualifier: ���ò����������������������������
// Parameter: u8 motor
// Parameter: u32 pulse
// Parameter: u16 speed
//************************************
bit setStepMotorWithPulse(u8 motor, u32 pulse,u16 speed)
{
	setPulse(motor,speed,pulse);
	return 1;
}
//************************************
// Method:    setStepMotorWithDistance
// FullName:  setStepMotorWithDistance
// Access:    public 
// Returns:   bit
// Qualifier: //��תָ���������巢������Ҫ������������,���ݳ�ʼ���е����ݣ�����Ӧ��ͬ�Ĳ��������
// Parameter: u8 motor
// Parameter: float distance
// Parameter: u16 speed
//************************************
bit setStepMotorWithDistance(u8 motor,float distance,u16 speed)//��תָ���������巢������Ҫ������������,
{

	setStepMotorWithPulse(motor
						, (u32)((distance * 360 * g_stepMotor[motor].driverDiv) / (PI*g_stepMotor[motor].bearingDia*g_stepMotor[motor].DIV))
						, speed);
	return 1;

}
//************************************
// Method:    setStepMotorWithAngle
// FullName:  setStepMotorWithAngle
// Access:    public 
// Returns:   bit
// Qualifier: ���ò����������ת�Ƕ�
// Parameter: u8 motor����һ�����
// Parameter: u16 angle ��ת�Ƕȣ����Գ���360��
// Parameter: u8 speed����ת�ٶȣ�1-100
//************************************
bit setStepMotorWithAngle(u8 motor, float angle, u16 speed)
{
	setPulse(motor, speed, 
		(u32)((angle*g_stepMotor[motor].driverDiv) / g_stepMotor[motor].DIV));
	return 1;
}
//************************************
// Method:    setStepMotorWithRound
// FullName:  setStepMotorWithRound
// Access:    public 
// Returns:   bit
// Qualifier: ���ò����������תȦ��
// Parameter: u8 motor
// Parameter: u16 round
// Parameter: u8 speed
//************************************
bit setStepMotorWithRound(u8 motor, u16 round, u16 speed)
{
	setStepMotorWithAngle(motor, 360 * round, speed);
	return 1;
}
void setStepMotorState(u8 motor,enum StepMotorState state)
{
	if (motor == STEP_MOTOR_1)
	{
		g_stepMotor[STEP_MOTOR_1].state = state;
	}
	else
	{
		g_stepMotor[STEP_MOTOR_2].state = state;
	}
}
//************************************
// Method:    getStepMotorState
// FullName:  getStepMotorState
// Access:    public 
// Returns:   bit
// Qualifier: ��ȡ���������ǰ��״̬
// Parameter: u8 motor
//***********************************
enum StepMotorState getStepMotorState(u8 motor)
{
	return g_stepMotor[motor].state;
}
//���õ����ת����
bit setStepMotorDiversion(u8 motor,bit diversion)
{
  
			if(motor==STEP_MOTOR_1)
			{
				if(diversion==CW)
				{
				StepMotor_1_DiversionControl=0;
				}
				else
				{
				StepMotor_1_DiversionControl=1;
				}
			}
			if(motor==STEP_MOTOR_2)
			{
				if(diversion==CW)
				{
				StepMotor_2_DiversionControl=0;
				}
				else
				{
				StepMotor_2_DiversionControl=1;
				}
			}
		
		
	return 1;
}
//************************************
// Method:    close_StepMotor
// FullName:  close_StepMotor
// Access:    public 
// Returns:   void
// Qualifier: �����ǵ����ǰ״̬��ǿ�йرղ������
// Parameter: u8 motor
//************************************
void close_StepMotor(u8 motor)
{
	closePulser(motor);
	g_stepMotor[motor].state = UNLOCKED;

}
//************************************
// Method:    open_StepMotor
// FullName:  open_StepMotor
// Access:    public 
// Returns:   void
// Qualifier: ���ú���ת����֮�󣬴򿪲�������������ʼ��ת
// Parameter: u8 motor
//************************************
void open_StepMotor(u8 motor)
{
	g_stepMotor[motor].state = RUNNING;
	openPulser(motor);
}
//************************************
// Method:    freeStepMotor
// FullName:  freeStepMotor
// Access:    public 
// Returns:   bit
// Qualifier: �������������ͬʱ���ܵ��������Ŀ���
// Parameter: u8 motor
//************************************
bit freeStepMotor(u8 motor)
{
   if(motor==STEP_MOTOR_1)
	 {
		  StepMotor_1_LockControl=0;
	 }
	 else
	 {
		 StepMotor_2_LockControl=0;
	 }
   g_stepMotor[motor].state = UNLOCKED;
	return 1;
}
//************************************
// Method:    lockStepMotor
// FullName:  lockStepMotor
// Access:    public 
// Returns:   bit
// Qualifier: �������������ͬʱ�ܵ�����������
// Parameter: u8 motor
//************************************
bit lockStepMotor(u8 motor)
{
   if(motor==STEP_MOTOR_1)
	 {
		StepMotor_1_LockControl=1;
	 }
	 else
	 {
		 StepMotor_2_LockControl=1;
	 }
   g_stepMotor[motor].state = LOCKED;

	return 1;
}
//************************************
// Method:    stepMotor_Init
// FullName:  stepMotor_Init
// Access:    public 
// Returns:   void
// Qualifier: ���������ʼ���������ʼ������
// div:		  ������������ǣ���λ�ȣ���1.8��
// driverDiv��������ϸ��ֵ����16ϸ��
// maxPeriod��������ܳ��ܵ��������Ƶ�ʣ���λhz����1000��
// bearingDia:��ת��г��ֵ�ֱ������λcm����3.9f 
//************************************
void stepMotor_Init(u8 motor,float div,u8 driverDiv,u16 maxPeriod,float bearingDia)
{ 
#ifdef DEBUG

			u8 initInfo[10];
#endif
	GPIO_InitTypeDef    GPIO_InitStructure;     //�ṹ����
	GPIO_InitStructure.Mode = GPIO_PullUp;  
	GPIO_InitStructure.Pin = GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4;    //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_Inilize(GPIO_P2, &GPIO_InitStructure);  //��ʼ��  
	g_stepMotor[motor].DIV = div;//1.8��Ĳ�����
	g_stepMotor[motor].driverDiv = driverDiv; //��������ʹ��16��ϸ��
	g_stepMotor[motor].maxPeriod = maxPeriod;
	g_stepMotor[motor].bearingDia = bearingDia;
	g_stepMotor[motor].state = UNLOCKED;
	PulserInit();//���巢������ʼ��
#ifdef DEBUG
	sprintf(initInfo, "\r\n stepMotor %u was initialized \r\n", motor + 1);
	PrintString1(initInfo);
#endif
	
}