/************************************************************
* 组织名称： 电子大赛小组
* 文件名称: STC15_PWM.C
* 作者:  周晨阳
* 版本:  1.3
* 日期:  2017/4/27
* 描述:  硬件pwm的库函数，io口参见下方介绍
* 历史修改记录: // 历史修改记录
* <作者> <时间> <版本 > <描述>
* 周晨阳 2017/4/27 1.1 修改了一处代码错误，case参数错误
* 周晨阳 2017/5/6  1.2 增加了一些常用设置函数
* 周晨阳 2017/7/9  1.3 修改了pwm信息的记录和返回方式，使用结构体数组来存放信息
* 周晨阳 2017/7/28 1.4 在设置频率的函数中增加了防止超出上限的判断，如果超出则使用最大频率
***********************************************************/

#include "stc15_pwm.h"
#include "GPIO.h"
//PWM信息存储
/************************************
硬件PWM io引脚
PWM_N| 第一组| 第二组
PWM2 :P3.7 -> P2.7
PWM3 :P2.1 -> P4.5
PWM4 :P2.2 -> P4.4
PWM5 :P2.3 -> P4.2
PWM6 :P1.6 -> P0.7
PWM7 :P1.7 -> P0.6
************************************/

static struct PWM_N_INFO
{
	u32 period;//pwm的频率
	u8 state;//pwm发生器的状态
	float duty;//pwm的占空比
	u8 DIV; //预分频值，用来产生较低频率的pwm
};
#define PWM_NUM 6
static  struct PWM_N_INFO PWM_N_INFO[PWM_NUM]; //6组pwm数据存储

/*************************************************
* 函数名称:void PWM_period(u16 Hz)
* 描述: 设置硬件pwm的同一频率 ，并保存频率数据
* 被本函数调用的函数:
* 1.PWM_SET_PERIOD
* 输入:u16 Hz:要输出的pwm的频率,由于硬件所限，将会同时改变6路pwm的频率
* 输出: 无
* 返回值: 无
* 其他: 此函数只能设置pwm的计数器初始值，从而完成pwm不同频率的输出，
		但是由于单片机硬件所限，不可以单独对每路pwm的频率进行修改，
		只能一改全改。
*************************************************/
void set_PWM_period(u8 PWM_N, u16 Hz)
{
	PWM_N_INFO[PWM_N].period = Hz;
	PWM_UNLOCK;
	PWM_ALL_NO;
	PWM_SET_PERIOD((u16)(MAIN_Fosc / (Hz*PWM_N_INFO[PWM_N].DIV)));
	//PWM_SET_PERIOD(5);

	PWM_LOCK;


}

/*************************************************
* 函数名称: u32 getPWM_period(void )
* 描述: 返回所设置的pwm频率信息
* 输入: u8 PWM_N, 2<=N<=7
* 输出: 无
* 返回值: pwm频率
* 其他说明: 若没有设置pwm的频率就调用此函数则会返回0；
*************************************************/
u32 get_PWM_period(u8 PWM_N)
{
	return PWM_N_INFO[PWM_N].period;
}
/*************************************************
* 函数名称: void PWM_duty(u8 PWM_N,float duty)
* 描述: 修改某一路pwm的占空比 ，并保存占空比数据
* 被本函数调用的函数:
* 调用本函数的函数:
* 输入:
* 1.u8 PWM_N ： 哪一路pwm
* 2.float duty：占空比，使用小数，如0.8代表80%的占空比
* 输出: 无
* 返回值: 无
* 其他说明:为防止电平发生反转，限制最小占空比为0.05，最大为0.95
*          更改了最低占空比的限定，用于符合舵机的最低占空比————0.05f -> 0.025f
*************************************************/
void set_PWM_duty(u8 PWM_N, float duty)
{
	if (duty > 0.95f)
	{
		duty = 0.95f;
	}
	if (duty < 0.025f)
	{
		duty = 0.025f;
	}
	PWM_N_INFO[PWM_N].duty = duty;//存储占空比值
	PWM_UNLOCK;
	PWM_SET_T12_PERIOD(PWM_N, 10, (u16)(duty *	(MAIN_Fosc / (PWM_N_INFO[PWM_N].period*PWM_N_INFO[PWM_N].DIV))));
	//PWM_SET_T12_PERIOD(PWM_N, 1,3 );

	PWM_LOCK;
}
/*************************************************
* 函数名称: float getPWM_n_duty(u8 PWM_N)
* 描述: 返回PWM_N的占空比信息
* 输入: u8 PWM_N, 2<=N<=7
* 输出: 无
* 返回值: PWM_N的占空比信息,float形式
* 其他说明: 若没有设置pwm的占空比就调用此函数则会返回0；
*************************************************/
float get_PWM_N_duty(u8 PWM_N)
{
	return  PWM_N_INFO[PWM_N].duty;
}

//************************************
// Method:    setPWM_DIV
// FullName:  setPWM_DIV
// Access:    public 
// Returns:   void
// Qualifier: 设置预分频，范围为1~16
// Parameter: u8 PWM_N
// Parameter: u8 DIV
//************************************
void setPWM_DIV(u8 PWM_N, u8 DIV)
{
	PWM_N_INFO[PWM_N].DIV = DIV;
}
//************************************
// Method:    getPWM_DIV
// FullName:  getPWM_DIV
// Access:    public 
// Returns:   u8
// Qualifier:
// Parameter: u8 PWM_N
//************************************
u8 getPWM_DIV(u8 PWM_N)
{
	return PWM_N_INFO[PWM_N].DIV;
}
//************************************
// Method:    open_PWM_ALL
// FullName:  open_PWM_ALL
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void open_PWM_ALL(void)
{
	PWM_UNLOCK;
	PWM_ALL_EN;
	PWM_LOCK;
}
//************************************
// Method:    close_PWM_ALL
// FullName:  close_PWM_ALL
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void close_PWM_ALL(void)
{
	PWM_UNLOCK;
	PWM_ALL_NO; //总开关
	PWM_LOCK;

}
//************************************
// Method:    open_PWM_N
// FullName:  open_PWM_N
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: u8 PWM_N
//************************************
void open_PWM_N(u8 PWM_N)
{
	PWM_UNLOCK;
	PWM_N_EN(PWM_N);
	PWM_N_INFO[PWM_N].state = ON;
	PWM_ALL_EN; //总开关,根据手册中的要求
	PWM_LOCK;


}
//************************************
// Method:    close_PWM_N
// FullName:  close_PWM_N
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: u8 PWM_N
//************************************
void close_PWM_N(u8 PWM_N)
{
	PWM_UNLOCK;
	PWM_N_NO(PWM_N);
	PWM_N_INFO[PWM_N].state = OFF;
	PWM_LOCK;

}
//************************************
// Method:    get_PWM_N_state
// FullName:  get_PWM_N_state
// Access:    public 
// Returns:   bit
// Qualifier:
// Parameter: u8 PWM_N
//************************************
bit get_PWM_N_state(u8 PWM_N)
{

	return  PWM_N_INFO[PWM_N].state;
}

//========================================================================
//u8    PWM_Inilize(PWM_InitTypeDef *PWM)
// 描述:PWM初始化程序
// 参数:u8 PWM_N:PWM路数标号(2~7) PWM: 结构参数,请参考pwm.h里的定义.
// 返回: 成功返回0, 错误返回1
//========================================================================
void PWM_Inilize(u8 PWM_N, PWM_InitTypeDef *PWMx)
{
	u8 i = 0;
	for (; i < 6; i++)
	{
		PWM_N_INFO[i].period = 0;
		PWM_N_INFO[i].state = 0;
		PWM_N_INFO[i].duty = 0;
		PWM_N_INFO[i].DIV = 1;
	}

	P_SW2 |= 0X80;
	if (ENABLE == PWMx->PWM_GOTO_ADC)            //ENABLE=计数器归零时 触发ADC
	{
		PWMCFG |= (1 << 7); //并的意思就是不会改变其他位的值
	}
	else
	{
		PWMCFG &= (~(1 << 7)); //其实也可以用上边的办法，但是这样写的意思更直观
	}
	if (PWM_HIGHT == PWMx->PWM_V_INIT)              //PWM_HIGHT=初始高电平     PWM_LOW=初始低电平
	{
		PWMCFG |= (1 << PWM_N);
	}
	else
	{
		PWMCFG &= (~(1 << PWM_N));
	}
	if (ENABLE == PWMx->PWM_0ISR_EN)             //ENABLE=使能PWM计数器归零中断  DISABLE=关闭PWM计数器归零中断 但 CBIF仍然会被硬件置位
	{
		PWMCR |= (1 << 6);
	}
	else
	{
		PWMCR &= (~(1 << 6));
	}
	if (ENABLE == PWMx->PWM_OUT_EN)              //ENABLE=PWM通道x的端口为PWM波形输出口 受PWM波形发生器控制
	{
		PWMCR |= (1 << PWM_N);
	}
	else
	{
		PWMCR &= (~(1 << PWM_N));
	}
	if (ENABLE == PWMx->PWM_UNUSUAL_EN)          //ENABLE=使能PWM的外部异常检测功能
	{
		PWMFDCR = (1 << 5);
	}
	else
	{
		PWMFDCR &= (~(1 << 5));
	}
	if (ENABLE == PWMx->PWM_UNUSUAL_OUT)         //ENABLE=发生异常时，PWM对应的输出口会变成 高阻输入模式
	{
		PWMFDCR = (1 << 4);
	}
	else
	{
		PWMFDCR &= (~(1 << 4));
	}
	if (ENABLE == PWMx->PWM_UNUSUAL_ISR_EN)      //ENABLE=使能异常检测中断 DISABLE=关闭异常检测中断 但FDIF仍然会被硬件置位
	{
		PWMFDCR = (1 << 3);
	}
	else
	{
		PWMFDCR &= (~(1 << 3));
	}
	if (ENABLE == PWMx->PWM_UNUSUAL_CMP0_EN)     //ENABLE=异常检测源为比较器的输出 当比较结果为高时 触发PWM异常
	{
		PWMFDCR = (1 << 2);
	}
	else
	{
		PWMFDCR &= (~(1 << 2));
	}
	if (ENABLE == PWMx->PWM_UNUSUAL_P24_EN)      //ENABLE=异常检测源为P24/PWMFLT 当P24=1 触发PWM异常
	{
		PWMFDCR = (1 << 1);
	}
	else
	{
		PWMFDCR &= (~(1 << 1));
	}
	//  PWMCKS=0;
	if (PWM_Clock_NT == PWMx->PWM_CLOCK)               //PWM_Clock_NT=PWM的时钟源是系统时钟经分频后的时钟  PWM_Clock_Timer2_OF=PWM的时钟源是TMER2的溢出脉冲
	{
		PWMCKS &= (~(1 << 4));
	}
	else
	{
		PWMCKS |= (1 << 4);
	}
	// PWMCKS=0x00;
	PWMCKS |= PWMx->PWM_CLOCK_DIV;           //当PWM_CLOCK=PWM_Clock_NT时 PWM的时钟源是系统时钟/(PS[3:0]+1)
	switch (PWM_N)
	{
	case PWM_2:
	{
		if (PWM_SELECT_N == PWMx->PWM_SELECTx_IO)          //PWM_SELECT_N=PWM第一选择IO口 PWM_SELECT_N_2=PWM第二选择IO口
		{
			PWM2CR &= (~(1 << 3));
		}
		else
		{
			PWM2CR |= (1 << 3);
		}
		if (ENABLE == PWMx->PWM_ISRx_EN)             //ENABLE=使能PWMx中断 使能第一翻转或第二翻转中断
		{
			PWM2CR |= (1 << 2);
		}
		else
		{
			PWM2CR &= (~(1 << 2));
		}
		if (ENABLE == PWMx->PWM_T1x_EN)              //ENABLE=使能第一翻转中断
		{
			PWM2CR |= (1 << 1);
		}
		else
		{
			PWM2CR &= (~(1 << 1));
		}
		if (ENABLE == PWMx->PWM_T2x_EN)              //ENABLE=使能第二翻转中断
		{
			PWM2CR |= (1 << 0);
		}
		else
		{
			PWM2CR &= (~(1 << 0));
		}
	}
	break;
	case PWM_3:
	{
		if (PWM_SELECT_N == PWMx->PWM_SELECTx_IO)          //PWM_SELECT_N=PWM第一选择IO口 PWM_SELECT_N_2=PWM第二选择IO口
		{
			PWM3CR &= (~(1 << 3));
		}
		else
		{
			PWM3CR |= (1 << 3);
		}
		if (ENABLE == PWMx->PWM_ISRx_EN)             //ENABLE=使能PWMx中断 使能第一翻转或第二翻转中断
		{
			PWM3CR |= (1 << 2);
		}
		else
		{
			PWM3CR &= (~(1 << 2));
		}
		if (ENABLE == PWMx->PWM_T1x_EN)              //ENABLE=使能第一翻转中断
		{
			PWM3CR |= (1 << 1);
		}
		else
		{
			PWM3CR &= (~(1 << 1));
		}
		if (ENABLE == PWMx->PWM_T2x_EN)              //ENABLE=使能第二翻转中断
		{
			PWM3CR |= (1 << 0);
		}
		else
		{
			PWM3CR &= (~(1 << 0));
		}
	}
	break;
	case PWM_4:
	{
		if (PWM_SELECT_N == PWMx->PWM_SELECTx_IO)          //PWM_SELECT_N=PWM第一选择IO口 PWM_SELECT_N_2=PWM第二选择IO口
		{
			PWM4CR &= (~(1 << 3));
		}
		else
		{
			PWM4CR |= (1 << 3);
		}
		if (ENABLE == PWMx->PWM_ISRx_EN)             //ENABLE=使能PWMx中断 使能第一翻转或第二翻转中断
		{
			PWM4CR |= (1 << 2);
		}
		else
		{
			PWM4CR &= (~(1 << 2));
		}
		if (ENABLE == PWMx->PWM_T1x_EN)              //ENABLE=使能第一翻转中断
		{
			PWM4CR |= (1 << 1);
		}
		else
		{
			PWM4CR &= (~(1 << 1));
		}
		if (ENABLE == PWMx->PWM_T2x_EN)              //ENABLE=使能第二翻转中断
		{
			PWM4CR |= (1 << 0);
		}
		else
		{
			PWM4CR &= (~(1 << 0));
		}
	}
	break;
	case PWM_5:
	{
		if (PWM_SELECT_N == PWMx->PWM_SELECTx_IO)          //PWM_SELECT_N=PWM第一选择IO口 PWM_SELECT_N_2=PWM第二选择IO口
		{
			PWM5CR &= (~(1 << 3));
		}
		else
		{
			PWM5CR |= (1 << 3);
		}
		if (ENABLE == PWMx->PWM_ISRx_EN)             //ENABLE=使能PWMx中断 使能第一翻转或第二翻转中断
		{
			PWM5CR |= (1 << 2);
		}
		else
		{
			PWM5CR &= (~(1 << 2));
		}
		if (ENABLE == PWMx->PWM_T1x_EN)              //ENABLE=使能第一翻转中断
		{
			PWM5CR |= (1 << 1);
		}
		else
		{
			PWM5CR &= (~(1 << 1));
		}
		if (ENABLE == PWMx->PWM_T2x_EN)              //ENABLE=使能第二翻转中断
		{
			PWM5CR |= (1 << 0);
		}
		else
		{
			PWM5CR &= (~(1 << 0));
		}
	}
	break;
	case PWM_6:
	{
		if (PWM_SELECT_N == PWMx->PWM_SELECTx_IO)          //PWM_SELECT_N=PWM第一选择IO口 PWM_SELECT_N_2=PWM第二选择IO口
		{
			PWM6CR &= (~(1 << 3));
		}
		else
		{
			PWM6CR |= (1 << 3);
		}
		if (ENABLE == PWMx->PWM_ISRx_EN)             //ENABLE=使能PWMx中断 使能第一翻转或第二翻转中断
		{
			PWM6CR |= (1 << 2);
		}
		else
		{
			PWM6CR &= (~(1 << 2));
		}
		if (ENABLE == PWMx->PWM_T1x_EN)              //ENABLE=使能第一翻转中断
		{
			PWM6CR |= (1 << 1);
		}
		else
		{
			PWM6CR &= (~(1 << 1));
		}
		if (ENABLE == PWMx->PWM_T2x_EN)              //ENABLE=使能第二翻转中断
		{
			PWM6CR |= (1 << 0);
		}
		else
		{
			PWM6CR &= (~(1 << 0));
		}
	}
	break;
	case PWM_7:
	{
		if (PWM_SELECT_N == PWMx->PWM_SELECTx_IO)          //PWM_SELECT_N=PWM第一选择IO口 PWM_SELECT_N_2=PWM第二选择IO口
		{
			PWM7CR &= (~(1 << 3));
		}
		else
		{
			PWM7CR |= (1 << 3);
		}
		if (ENABLE == PWMx->PWM_ISRx_EN)             //ENABLE=使能PWMx中断 使能第一翻转或第二翻转中断
		{
			PWM7CR |= (1 << 2);
		}
		else
		{
			PWM7CR &= (~(1 << 2));
		}
		if (ENABLE == PWMx->PWM_T1x_EN)              //ENABLE=使能第一翻转中断
		{
			PWM7CR |= (1 << 1);
		}
		else
		{
			PWM7CR &= (~(1 << 1));
		}
		if (ENABLE == PWMx->PWM_T2x_EN)              //ENABLE=使能第二翻转中断
		{
			PWM7CR |= (1 << 0);
		}
		else
		{
			PWM7CR &= (~(1 << 0));
		}
	}
	break;
	default:
		//      return 1;
		break;
	}
	if (ENABLE == PWMx->PWM_EN)                  //ENABLE=PWM使能 在其他PWM参数设置好后最后设置 如果被关闭后在打开，则PWM计数器重新从0计数
	{
		PWMCR |= (1 << 7);
	}
	else
	{
		PWMCR &= (~(1 << 7));
	}
}
//////////////！以下为私有函数，禁止改动！//////////////////////
//
//************************************
// Method:    PWM_SET_PERIOD
// FullName:  PWM_SET_PERIOD
// Access:    public static 
// Returns:   u8
// Qualifier: 设置PWM周期
// Parameter: u16 period
//************************************
static u8 PWM_SET_PERIOD(u16 period)
{
	if (0x8000 > period) //2的15次方为最大值
	{
		PWMCL = (u8)(period);
		PWMCH = (u8)(period >> 8);
		return 0;
	}
	else //如果大于等于0x8000，则默认使用最大周期，既最小频率
	{
		PWMCL = (u8)(32767);
		PWMCH = (u8)(32767 >> 8);
	}
}


//************************************
// Method:    PWM_SET_T12_PERIOD
// FullName:  PWM_SET_T12_PERIOD
// Access:    public static 
// Returns:   u8
// Qualifier: 设置PWM第一次和第二次翻转周期
// Parameter: u8 PWM_N
// Parameter: u16 period1
// Parameter: u16 period2
//************************************
static u8 PWM_SET_T12_PERIOD(u8 PWM_N, u16 period1, u16 period2)
{
	switch (PWM_N)
	{
	case PWM_2:
		PWM2T1L = (u8)(period1);
		PWM2T1H = (u8)(period1 >> 8);
		PWM2T2L = (u8)(period2);
		PWM2T2H = (u8)(period2 >> 8);
		return 0;
		break;
	case PWM_3:
		PWM3T1L = (u8)(period1);
		PWM3T1H = (u8)(period1 >> 8);
		PWM3T2L = (u8)(period2);
		PWM3T2H = (u8)(period2 >> 8);
		return 0;
		break;
	case PWM_4:
		PWM4T1L = (u8)(period1);
		PWM4T1H = (u8)(period1 >> 8);
		PWM4T2L = (u8)(period2);
		PWM4T2H = (u8)(period2 >> 8);
		return 0;
		break;
	case PWM_5:
		PWM5T1L = (u8)(period1);
		PWM5T1H = (u8)(period1 >> 8);
		PWM5T2L = (u8)(period2);
		PWM5T2H = (u8)(period2 >> 8);
		return 0;
		break;
	case PWM_6:
		PWM6T1L = (u8)(period1);
		PWM6T1H = (u8)(period1 >> 8);
		PWM6T2L = (u8)(period2);
		PWM6T2H = (u8)(period2 >> 8);
		return 0;
		break;
	case PWM_7:
		PWM7T1L = (u8)(period1);
		PWM7T1H = (u8)(period1 >> 8);
		PWM7T2L = (u8)(period2);
		PWM7T2H = (u8)(period2 >> 8);
		return 0;
		break;
	default:
		return 1;
		break;
	}
}
