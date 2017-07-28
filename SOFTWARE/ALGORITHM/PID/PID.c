/************************************************************
* ��֯���ƣ����Ӵ���С��
* �ļ�����: PID.c
* ����:     ���Ӵ���С��
* �汾 :    1.0
* ����:     10/06/17
* ����:
* ��ʷ�޸ļ�¼:
*  1.����������ʽPID�㷨���ڽṹ���м�����ǰ���ε�������
*
* <����> <ʱ��> <�汾 > <����>
***********************************************************/

#include "PID.h"
#define PID_ALG
#ifdef PID_ALG
#include <math.h>
#define  INTEGRAL_SEPARATE //ʹ�û��ַ��뷨
//pid�㷨�ľ�̬�ṹ��

typedef  struct
{
	float setParameter;//�趨�Ƕ�ֵ
	float actualParameter;//ʵ�ʽǶ�ֵ
	float err;//ƫ��ֵ
	float err_last;//��һ��ƫ��ֵ
	float err_prev;//err_last��һ�ε�ƫ��
	float Kp;
	float Ki;
	float Kd;//���������֣�΢��ϵ��
	float integral;//����ֵ,λ��ʽpid�㷨
	float output;  //ʵ���������
	float Ki_temp;//����pid���������ľ�̬����
	float Kp_temp;//����pid���������ľ�̬����
	float Kd_temp; //����pid���������ľ�̬����
	float parameterUpperLimit;//��������
	float parameterInferiorLimit;//��������
	u8 state;//pid����״̬
} pid_info;

static pid_info g_pid_n_info[PID_NUM];

/*************************************************
* ��������:PID_config
* ����: pid��ʼ������
* ����:u8 PID_N, float kp, float ki, float kd
* ���:
* ����ֵ:
* ����˵��:
*************************************************/
void PID_config(u8 PID_N, float kp, float ki, float kd)//pid�㷨��ʼ��������������������PID
{
	g_pid_n_info[PID_N].err = 0;
	g_pid_n_info[PID_N].err_last = 0;
	g_pid_n_info[PID_N].err_prev = 0;
	g_pid_n_info[PID_N].output = 0;
	g_pid_n_info[PID_N].integral = 0;
	g_pid_n_info[PID_N].Kp = kp;
	g_pid_n_info[PID_N].Ki = ki;
	g_pid_n_info[PID_N].Kd = kd;
	g_pid_n_info[PID_N].Kp_temp = g_pid_n_info[PID_N].Kp;
	g_pid_n_info[PID_N].Ki_temp = g_pid_n_info[PID_N].Ki;
	g_pid_n_info[PID_N].Kd_temp = g_pid_n_info[PID_N].Kd;
}
//************************************
// Method:    openPID
// FullName:  openPID
// Access:    public 
// Returns:   void
// Qualifier: ��pid
// Parameter: u8 PID_N
//************************************
void openPID(u8 PID_N)
{
	g_pid_n_info[PID_N].state = ON;
	//add the code here
}
//************************************
// Method:    closePID
// FullName:  closePID
// Access:    public 
// Returns:   void
// Qualifier:�ر�pid
// Parameter: u8 PID_N
//************************************
void closePID(u8 PID_N)
{
	g_pid_n_info[PID_N].state = OFF;
	//add the code here

}
//************************************
// Method:    PID
// FullName:  PID
// Access:    public 
// Returns:   bit
// Qualifier: ���úò�����ʹ�ô˺�������pid���㣬֮��ʹ��get**������ȡ������ֵ����Ҫ�ȴ�pid��ʹ�ô˺���
// Parameter: u8 PID_N
//************************************
bit PID(u8 PID_N)
{


	if (g_pid_n_info[PID_N].state)
	{

		g_pid_n_info[PID_N].err = g_pid_n_info[PID_N].setParameter - g_pid_n_info[PID_N].actualParameter;
		//#ifdef  INTEGRAL_SEPARATE //�Ƿ�ʹ�û��ַ��뷨
		//  if(abs(pid.err)<15.0f)//���ַ��뷨����ֹ������ʱ�����ۻ���������𵴣�ͬʱ��С������΢��
		//    {
		//      pid.Ki        =        Ki_temp;
		//      pid.integral  +=       pid.err;
		//      pid.Kp        =        Kp_temp-0.7f;
		//      pid.Kd        =        Kd_temp-0.5f;
		//    }
		//  else//������ʱȥ����������ۼ���� ����ʹ�ñ�����΢�� ��ͬʱ��ԭ�����ϼӴ�΢�ֵ�����
		//    {
		//      pid.Ki        =        0;
		//      pid.integral  =        0;
		//      pid.Kd        =        Kd_temp+0.5f;
		//    }
		//#else
		g_pid_n_info[PID_N].integral += g_pid_n_info[PID_N].err;
		//#endif
		//PID�㷨����
		g_pid_n_info[PID_N].output  = g_pid_n_info[PID_N].Kp*g_pid_n_info[PID_N].err		  \
									+ g_pid_n_info[PID_N].Ki*g_pid_n_info[PID_N].integral     \
									+ g_pid_n_info[PID_N].Kd*(g_pid_n_info[PID_N].err - g_pid_n_info[PID_N].err_last);
		g_pid_n_info[PID_N].err_last = g_pid_n_info[PID_N].err;


		return 1;
	}

}

//************************************
// Method:    Incremental_PID
// FullName:  Incremental_PID
// Access:    public 
// Returns:   void
// Qualifier: ����ʽPID�㷨�����úò�����ʹ�ô˺�������pid���㣬֮��ʹ��get**������ȡ������ֵ����Ҫ�ȴ�pid��ʹ�ô˺���
// Parameter: u8 PID_N
//************************************
bit Incremental_PID(u8 PID_N)
{
	if(g_pid_n_info[PID_N].state)
	{
		g_pid_n_info[PID_N].err = g_pid_n_info[PID_N].setParameter - g_pid_n_info[PID_N].actualParameter;
		g_pid_n_info[PID_N].output = g_pid_n_info[PID_N].Kp * g_pid_n_info[PID_N].err// kp * err
																 - g_pid_n_info[PID_N].Ki * g_pid_n_info[PID_N].err_last// ki * err_last
																 + g_pid_n_info[PID_N].Kd * g_pid_n_info[PID_N].err_prev;//kd * err_prev
		g_pid_n_info[PID_N].err_prev = g_pid_n_info[PID_N].err_last;
		g_pid_n_info[PID_N].err_last = g_pid_n_info[PID_N].err;
		return 1;
	}
}

//************************************
// Method:    setParameterInferiorLimit
// FullName:  setParameterInferiorLimit
// Access:    public 
// Returns:   void
// Qualifier: ����������ֵ����
// Parameter: u8 PID_N
// Parameter: float value
//************************************
void setParameterInferiorLimit(u8 PID_N, float value)
{
	g_pid_n_info[PID_N].parameterInferiorLimit = value;
}

//************************************
// Method:    setParameterUpperLimit
// FullName:  setParameterUpperLimit
// Access:    public 
// Returns:   void
// Qualifier: ����������ֵ����
// Parameter: u8 PID_N
// Parameter: float value
//************************************
void setParameterUpperLimit(u8 PID_N, float value)
{
	g_pid_n_info[PID_N].parameterUpperLimit = value;

}

//************************************
// Method:    setActualParameter
// FullName:  setActualParameter
// Access:    public 
// Returns:   void
// Qualifier: ����ʵ��������ֵ
// Parameter: u8 PID_N
// Parameter: float value
//************************************
void setActualParameter(u8 PID_N, float value)
{
	g_pid_n_info[PID_N].actualParameter = value;

}

//************************************
// Method:    setKi
// FullName:  setKi
// Access:    public 
// Returns:   void
// Qualifier: ����PID��Kiֵ
// Parameter: u8 PID_N
// Parameter: float value
//************************************
void setKi(u8 PID_N, float value)
{
	g_pid_n_info[PID_N].Ki = value;
}

//************************************
// Method:    setKp
// FullName:  setKp
// Access:    public 
// Returns:   void
// Qualifier: ����PID��Kpֵ
// Parameter: u8 PID_N
// Parameter: float value
//************************************
void setKp(u8 PID_N, float value)
{
	g_pid_n_info[PID_N].Kp = value;
}

//************************************
// Method:    setKd
// FullName:  setKd
// Access:    public 
// Returns:   void
// Qualifier: ����PID��Kdֵ
// Parameter: u8 PID_N
// Parameter: float value
//************************************
void setKd(u8 PID_N, float value)
{
	g_pid_n_info[PID_N].Kd = value;
}

//************************************
// Method:    setParameter
// FullName:  setParameter
// Access:    public 
// Returns:   void
// Qualifier: ����PID��Ŀ����ֵ
// Parameter: u8 PID_N
// Parameter: float value
//************************************
void setParameter(u8 PID_N, float value)
{
	g_pid_n_info[PID_N].setParameter = value;
}


//************************************
// Method:    getParameter
// FullName:  getParameter
// Access:    public 
// Returns:   float
// Qualifier: ��ȡPID��Ŀ����ֵ
// Parameter: u8 PID_N
//************************************
float	getParameter(u8 PID_N)
{
	return  g_pid_n_info[PID_N].setParameter;
}

//************************************
// Method:    getErr
// FullName:  getErr
// Access:    public 
// Returns:   float
// Qualifier: ��ȡPID����һ�����ֵ
// Parameter: u8 PID_N
//************************************
float getErr(u8 PID_N)
{
	return  g_pid_n_info[PID_N].err;
}

//************************************
// Method:    getErrLast
// FullName:  getErrLast
// Access:    public 
// Returns:   float
// Qualifier: ��ȡPID����һ�����ֵ
// Parameter: u8 PID_N
//************************************
float getErrLast(u8 PID_N)
{
	return  g_pid_n_info[PID_N].err_last;
}
//************************************
// Method:    getKp
// FullName:  getKp
// Access:    public 
// Returns:   float
// Qualifier: ��ȡPID��Kpֵ
// Parameter: u8 PID_N
//************************************
float getKp(u8 PID_N)
{
	return  g_pid_n_info[PID_N].Kp;
}

//************************************
// Method:    getKi
// FullName:  getKi
// Access:    public 
// Returns:   float
// Qualifier: ��ȡPID��Kiֵ
// Parameter: u8 PID_N
//************************************
float getKi(u8 PID_N)
{
	return  g_pid_n_info[PID_N].Ki;
}

//************************************
// Method:    getKd
// FullName:  getKd
// Access:    public 
// Returns:   float
// Qualifier: ��ȡpid��Kd����ֵ
// Parameter: u8 PID_N
//************************************
float getKd(u8 PID_N)
{
	return  g_pid_n_info[PID_N].Kd;
}

//************************************
// Method:    getOutput
// FullName:  getOutput
// Access:    public 
// Returns:   float
// Qualifier: ��ȡpid���ֵ
// Parameter: u8 PID_N
//************************************
float getOutput(u8 PID_N)
{
	return  g_pid_n_info[PID_N].output;
}

//************************************
// Method:    getIntegral
// FullName:  getIntegral
// Access:    public 
// Returns:   float
// Qualifier: ��ȡpid����ֵ
// Parameter: u8 PID_N
//************************************
float getIntegral(u8 PID_N)
{
	return  g_pid_n_info[PID_N].integral;
}
#endif

