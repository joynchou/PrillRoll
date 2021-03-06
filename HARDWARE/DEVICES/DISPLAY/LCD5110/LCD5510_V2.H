/*--------------------------------------------------------------*/
//File:	LCD5510_V1.H
//Date:	2009/8/4
//Time: 9:16
//Note: Nokia 5510 LCD液晶显示
//每8位显示数据是低位在上, 高位在下
//测试硬件
//单片机:  STC89C52RC
//模式:    12T
//晶振:	   12MHz


/*--------------------------------------------------------------*/
//防止重复引用
#ifndef	__LCD5510_V2_H__
#define __LCD5510_V2_H__
#include "../../../BSP/config.h"

/*--------------------------------------------------------------*/
//字库码的引索
/*
(0)  !(1)  "(2)  #(3)  $(4)  %(5)  &(6)  '(7)
((8)  )(9)  *(10) +(11) ,(12) -(13) .(14) /(15)
0(16) 1(17) 2(18) 3(19) 4(20) 5(21) 6(22) 7(23)
8(24) 9(25) :(26) ;(27) <(28) =(29) >(30) ?(31)
@(32) A(33) B(34) C(35) D(36) E(37) F(38) G(39)
H(40) I(41) J(42) K(43) L(44) M(45) N(46) O(47)
P(48) Q(49) R(50) S(51) T(52) U(53) V(54) W(55)
X(56) Y(57) Z(58) [(59) \(60) ](61) ^(62) _(63)
`(64) a(65) b(66) c(67) d(68) e(69) f(70) g(71)
h(72) i(73) j(74) k(75) l(76) m(77) n(78) o(79)
p(80) q(81) r(82) s(83) t(84) u(85) v(86) w(87)
x(88) y(89) z(90) {(91) |(92) }(93) ~(94)
/*--------------------------------------------------------------*/


/*--------------------------------------------------------------*/
//	  字体:	LCD1602字体
//取码规则:	低位在前, 列行扫描, 阴码(1-亮, 0-灭)
//查表方法: 要显示的ASCII码-32就可以得到字库码的指针

/*--------------------------------------------------------------*/

/*--------------------------------------------------------------*/
//指令宏定义
#define X_Col_Addr     0x80		//定位到第0列指令(列起始地址)(0 - 83)
#define Y_Page_Addr    0x40		//定位到第0页指令(页起始地址)(0 - 5)


/*--------------------------------------------------------------*/
//功能宏定义
//液晶复位
#define LCD_reset_hard	LCD_RST = 0; LCD_RST = 1	//硬件复位
#define LCD_reset_soft	LCD_reset_5510()			//软件复位
//液晶显示控制(不影响DDRAM)
#define LCD_show_blank		LCD_write_cmd(0x08)		//显示空白
#define LCD_show_normal		LCD_write_cmd(0x0c)		//正常显示
#define LCD_show_black		LCD_write_cmd(0x09)		//显示全黑
#define LCD_show_inverse 	LCD_write_cmd(0x0d)		//反色显示
//便于理解
#define LCD_write_cmd(cmd)	LCD_write_byte(cmd, 0)	//写入指令
#define LCD_write_dat(dat)	LCD_write_byte(dat, 1)	//写入数据


/*--------------------------------------------------------------*/
//数据接口定义
sbit LCD_CLK = P5 ^ 0;     //串行时钟			//上升沿写入数据
sbit LCD_DIN = P5 ^ 1;     //串行数据输入		//先高后低
sbit LCD_DC = P5 ^ 2;     //数据指令控制端	//高电平数据，低电平指令
sbit LCD_CS = P5 ^ 3;     //片选使能			//低电平有效
sbit LCD_RST = P5 ^ 4;     //LCD复位端		//低电平复位


						   /*--------------------------------------------------------------*/
						   //函数声明(私有)
void LCD_write_byte(unsigned char wbyte, unsigned char dat_cmd);//写入字节
void LCD_reset_5510(void);										//复位LCD5510


																/*--------------------------------------------------------------*/
																//函数声明(公有)
																/*--------------------------------------------------------------*/
																//清屏参数(清DDRAM)
void LCD_clr_scr(void);


/*--------------------------------------------------------------*/
//液晶字节定位(1*1)
//液晶规划:
//x: 0 - 83
//y: 0 - 5
void LCD_pos_byte(unsigned char x, unsigned char y);


/*--------------------------------------------------------------*/
//显示清行
//num:	0 - 5
void LCD_clr_row(unsigned char num);


/*--------------------------------------------------------------*/
//液晶字符输出(6*8字体)
//x: 0 - 13
//y: 0 - 5
void LCD_printc(unsigned char x, unsigned char y, unsigned char c_dat);


/*--------------------------------------------------------------*/
//液晶字符串输出(6*8字体)
//x: 0 - 13
//y: 0 - 5
void LCD_prints(unsigned char x, unsigned char y, unsigned char *s_dat);


/*--------------------------------------------------------------*/
//液晶字符串输出,自动换行(6*8字体)
//x: 0 - 13
//y: 0 - 5
void LCD_printsl(unsigned char x, unsigned char y, unsigned char *s_dat);


/*--------------------------------------------------------------*/
//初始化LCD5510
void LCD5510_Init(void);


/*--------------------------------------------------------------*/
//包含文件


/*--------------------------------------------------------------*/
#endif






