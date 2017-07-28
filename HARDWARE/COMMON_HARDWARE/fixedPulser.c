/************************************************************
* ��֯���ƣ�
* �ļ�����: K:\��Ƭ�����\���Ӵ���������\HARDWARE\COMMON_HARDWARE\FIXEDPULSER.C
* ����:   �ܳ���
* �汾:  1.0
* ����:     2017/07/18
* ����:	���������̶������������,��ඨ���������巢������PULSER_1ʹ�ö�ʱ��3 p04��PULSER_2ʹ�ö�ʱ��4  p06
* ��ʷ�޸ļ�¼:
* <����> <ʱ��> <�汾 > <����>
*  �ܳ��� 7/25  1.1  �����˶�ʱ���Ķ�ʱʱ�䣬��ԭ������ܲ���2500hz�������ɿ��Բ���5000hz������
*  �ܳ��� 7/27  1.2   ���¶��������巢�����Ĳ�����ʽ��ʹ��������ʱ����������·���壬��С���Բ���16hz������
***********************************************************/

#include "fixedPulser.h"
#include "../BSP/STC15_PWM.h"
#include "../BSP/GPIO.H"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "../BSP/USART1.h"
#include "../BSP/timer.h"
#define PULSER_NUM	2	//������Ҫʹ�ö��ٸ����巢���������ɲ�����·��ͬƵ�ʵ�����
#define DEFAULT_PT	HIGH //���巢��������ʼ��λ

#define OPEN_TIMER3_CLKOUT() T4T3M |= 1
#define OPEN_TIMER4_CLKOUT() T4T3M |= (1 << 4)
#define CLOSE_TIMER3_CLKOUT() T4T3M &=(~ 1)
#define CLOSE_TIMER4_CLKOUT() T4T3M &=(~(1 << 4))


//#define DEFAULT_PT	LOW //
struct Pulser //���巢�����йز������ݽṹ
{
	u32 count;  //�������
	u16 Hz;     //����Ƶ��
	u8 state;   //���巢������ǰ״̬

};

static struct Pulser g_pulser[PULSER_NUM];  //���巢������Ϣ�洢����
static u16 tmp = 0;//�����ʱ����Ҫʹ�õı�����д��������Ϊ�˼����жϵļ����������Ӷ�ʱ����
static bit timer2UseBit = 0;//�Ƿ���Ҫʹ�ü����������������ڶ�ʱ����Ϳɲ�����Ƶ�ʵı�־λ
static bit timer3UseBit = 0;//�Ƿ���Ҫʹ�ü����������������ڶ�ʱ����Ϳɲ�����Ƶ�ʵı�־λ
bool setPulse(u8 pulser_num, u16 Hz, u32 count)
{
	u16 timerTmp = 0;
	u16 TIM_Value;
	g_pulser[pulser_num].Hz = Hz;
	g_pulser[pulser_num].count = count;
	if (pulser_num == PULSER_1)
	{
		if (Hz > 20)//����20hzֱ��ʹ�ö�ʱ����ֵ
		{
			//PrintString1("hz >20\r\n");

			TIM_Value = (65536UL - ((MAIN_Fosc / 12) / Hz / 2));
			timer2UseBit = 0;
			INT_CLKO |= 0x04;	//���ʱ��
			TH2 = (u8)(TIM_Value >> 8);
			TL2 = (u8)TIM_Value;

		}
		else//С��20hz���������
		{
			timer2UseBit = 1;
			//	CLOSE_TIMER2_CLKOUT();
		}
	}
	else
	{
		if (Hz > 20)
		{
			TIM_Value = (65536UL - ((MAIN_Fosc / 12) / Hz / 2));
			timer3UseBit = 0;
			T4T3M |= 1;	//���ʱ��
			TH3 = (u8)(TIM_Value >> 8);
			TL3 = (u8)TIM_Value;

		}
		else
		{
			timer3UseBit = 1;
		}
	}

	return 1;
}
bool openPulser(u8 pulser_num)
{
	g_pulser[pulser_num].state = ON;

	if (pulser_num == PULSER_1) //pulser2
	{
		Timer2_Run();
		//	PrintString1("pulser 1 is start\n");

	}
	else   //pulser1
	{
		Timer3_Run();
	}

	return OK;
}
bool closePulser(u8 pulser_num)
{
	if (pulser_num == PULSER_1)//pulser2
	{
		Timer2_Stop();


	}
	else //pulser1
	{
		Timer3_Stop();

	}
	g_pulser[pulser_num].state = OFF;
	return OK;

}
bool getPulserState(u8 pulser_num)
{

	return g_pulser[pulser_num].state;

}
void PulserInit(void)//��ʼ������
{
	//PULSER_1 ʹ��P30 PULSER_2 ʹ��P04

	GPIO_InitTypeDef    GPIO_InitStructure;     //�ṹ����
	GPIO_InitStructure.Mode = GPIO_PullUp;       //ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_InitStructure.Pin = GPIO_Pin_0;    //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����!!!!!!!GPIO��ʼ��ʹ�ð�λ�����߼���֮ǰ��Ϊ���������
	GPIO_Inilize(GPIO_P3, &GPIO_InitStructure);  //��ʼ��
	GPIO_InitStructure.Pin = GPIO_Pin_4;    //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����!!!!!!!GPIO��ʼ��ʹ�ð�λ�����߼���֮ǰ��Ϊ���������
	GPIO_Inilize(GPIO_P0, &GPIO_InitStructure);  //��ʼ��
	g_pulser[PULSER_1].state = OFF;
	g_pulser[PULSER_2].state = OFF;
#ifdef DEBUG
	PrintString1("two pulser was initialized");
#endif
}


/***************������Ϊ˽�к�������������ģ�********************************/

/********************* Timer3�жϺ��������ڲ���ָ������************************/
//200us����һ���жϣ�
static u16 pulser1Tmp = 0;
static u16 count1Tmp = 0;
//static void Pulser_1Int(void) interrupt TIMER2_VECTOR   //2.5ms����һ���ж�
//{
//	//PrintString1("into timer inte\n");
////	if (timer2UseBit == 1)//������õ�Ƶ��С��20hz��ʹ�ü������ۼӵİ취����
////	{
////		if (++pulser1Tmp >= tmp)

////		{
////			//	PrintString1("p15 state changed\n");

////			pulser1Tmp = 0;
////			if (count1Tmp++ <= g_pulser[PULSER_1].count)
////			{
////				P30 = ~P30;

////				//	PrintString1("p15 state is changed\n");
////			}
////			else
////			{
////				closePulser(PULSER_1);
////				count1Tmp = 0;

////			}
////		}
////	}
////	else//���Ƶ�ʴ���20��ֱ���ø����������
//	{
//		if (++count1Tmp >= g_pulser[PULSER_1].count*2)
//		{
//			count1Tmp = 0;
//			closePulser(PULSER_1);
//		}
//	}
static void Pulser_1Int(void) interrupt TIMER2_VECTOR   //2.5ms����һ���ж�
{
	if ((++pulser1Tmp) >= 2 * g_pulser[PULSER_1].count)
	{
		closePulser(PULSER_1);
		pulser1Tmp = 0;
	}

}
static u16 pulser2Tmp = 0;
static u16 count2Tmp = 0;

static void Pulser_2Int(void) interrupt TIMER3_VECTOR   //2.5ms����һ���ж�
{
	if ((++pulser2Tmp) >= 2 * g_pulser[PULSER_2].count)
	{
		closePulser(PULSER_2);
		pulser2Tmp = 0;
	}

}


//static u32 g_PWM_7tmp = 0;//��������
////�ж�ִ�г���������¼����ĸ�������ʱ�رշ�����
//static void PWM_Routine(void) interrupt 22   
//{

//	if (PWMIF ^ 5 == 1)//PWM_7�ڶ���ת�ж�
//	{

//		PWMIF &= (~(1 << 5));  //��������־λ
//	//	PrintString1(" pwm7 count++\n");

//		if (g_PWM_7tmp++ >= g_pulser[PULSER_1].count)
//		{
//			//			PrintString1("pwm7 temp >= count\n");
//			g_PWM_7tmp = 0;
//			closePulser(PULSER_1); //����ָ������������͹ر����巢����

//		}

//	}
//	//����ѡ��ʹ�ö�ʱ��ģ��һ·���巢������
//	//	if (PWMIF ^ 4 == 1)//PWM_6
//	//	{
//	//		PWMIF &= (~(1 << 4));  //��������־λ
//	////			PrintString1("pwm6 count++\n");
//	//		if (g_PWM_6tmp++ >= g_pulser[PULSER_2].count)
//	//		{
//	//			//					PrintString1(" pwm6 temp >= count\n");
//	//			g_PWM_6tmp = 0;
//	//			closePulser(PULSER_2);
//	//			close_PWM_ALL();
//	//		}
//	//
//	//	}
//}
