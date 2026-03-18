/*********************************************************************************************
模板制作：  杜洋工作室/洋桃电子
程序名：	数码管RTC显示程序
编写人：	杜洋	
编写时间：	2018年2月2日
硬件支持：	STM32F103C8   外部晶振8MHz RCC函数设置主频72MHz　  

修改日志：　　
1-
	
							
说明：
 # 本模板加载了STM32F103内部的RCC时钟设置，并加入了利用滴答定时器的延时函数。
 # 可根据自己的需要增加或删减。

*********************************************************************************************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "rtc.h"
#include "TM1640.h"
#include "lm75a.h"
#include "touch_key.h"
#include "math.h"
#include "buzzer.h"

u8 temp[3];
u8 menu = 1;
u16 i = 0;
u8 rup = 0;

uint16_t keyId()
{
	for (uint16_t i = TOUCH_KEY_A; i <= TOUCH_KEY_D; i <<= 1)
	{
		if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, i))
		{ //读触摸按键的电平
			BUZZER_BEEP1();
			return i;
		}
	}
	return 0;
}
void setting()
{
	BUZZER_BEEP1();
	BUZZER_BEEP2();
	TM1640_display(0, 20); //年
	TM1640_display(1, 20);
	TM1640_display(2, 5); //月
	TM1640_display(3, 23);
	TM1640_display(4, 24); //日
	TM1640_display(5, 20);
	TM1640_display(6, 20);
	TM1640_display(7, 20); //周
	delay_s(1);
	menu = 11;
}
void showDate()
{
	TM1640_display(0, ryear / 10 % 10);
	TM1640_display(1, ryear % 10 + 10);
	TM1640_display(2, rmon / 10 % 10); //月
	TM1640_display(3, rmon % 10 + 10);
	TM1640_display(4, rday / 10 % 10); //日
	TM1640_display(5, rday % 10);
	TM1640_display(6, 21);
	TM1640_display(7, rweek); //周
}
void setYear()
{
	if (i++ == 500)
		showDate();
	else if (i == 1000)
	{
		TM1640_display_add(0, 20, 2);
		i = 0;
	}
	int t = 0;
	switch (keyId())
	{
	case TOUCH_KEY_A:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
			{
				ryear++;
				if (ryear > 2099)
					ryear = 2021;
				showDate();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			ryear++;
			if (ryear > 2099)
				ryear = 2021;
		}
		t = 0;
		break;
	case TOUCH_KEY_B:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
			{
				ryear--;
				if (ryear < 2021)
					ryear = 2100;
				showDate();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			ryear--;
			if (ryear < 2021)
				ryear = 2100;
		}
		break;
	case TOUCH_KEY_C:
		menu = 12;
		rup = 1; //时间修改的标志位置1
		break;
	case TOUCH_KEY_D:
		menu = 1;
		rup = 1; //时间修改的标志位置1
		BUZZER_BEEP2();
		BUZZER_BEEP1();
		break;
	}
	delay_ms(1);
}
void setMon()
{
	if (i++ == 500)
		showDate();
	else if (i == 1000)
	{
		TM1640_display_add(2, 20, 2);
		i = 0;
	}
	int t = 0;
	switch (keyId())
	{
	case TOUCH_KEY_A:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
			{
				rmon++;
				if (rmon > 12)
					rmon = 1;
				showDate();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			rmon++;
			if (rmon > 12)
				rmon = 1;
		}
		t = 0;
		break;
	case TOUCH_KEY_B:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
			{
				rmon--;
				if (rmon < 1)
					rmon = 12;
				showDate();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			rmon--;
			if (rmon < 1)
				rmon = 12;
		}
		t = 0;
		break;
	case TOUCH_KEY_C:
		menu = 13;
		rup = 1; //时间修改的标志位置1
		break;
	case TOUCH_KEY_D:
		menu = 1;
		rup = 1; //时间修改的标志位置1
		break;
	}
	delay_ms(1);
}
void setDay()
{
	if (i++ == 500)
		showDate();
	else if (i == 1000)
	{
		TM1640_display_add(4, 20, 2);
		i = 0;
	}
	int t = 0;
	switch (keyId())
	{
	case TOUCH_KEY_A:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
			{
				rday++;
				if (rday > 31)
					rday = 1;
				showDate();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			rday++;
			if (rday > 31)
				rday = 1;
		}
		t = 0;
		break;
	case TOUCH_KEY_B:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
			{
				rday--;
				if (rday < 1)
					rday = 31;
				showDate();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			rday--;
			if (rday < 1)
				rday = 31;
		}
		break;
	case TOUCH_KEY_C:
		menu = 21;
		rup = 1; //时间修改的标志位置1
		break;
	case TOUCH_KEY_D:
		menu = 1;
		rup = 1; //时间修改的标志位置1
		break;
	}
	delay_ms(1);
}
void showTime()
{
	TM1640_display(0, rhour / 10 % 10); //时
	TM1640_display(1, rhour % 10);
	TM1640_display(2, 21);
	TM1640_display(3, rmin / 10 % 10); //分
	TM1640_display(4, rmin % 10);
	TM1640_display(5, 21);
	TM1640_display(6, rsec / 10 % 10); //秒
	TM1640_display(7, rsec % 10);
}
void setHour()
{
	if (i++ == 500)
		showTime();
	else if (i == 1000)
	{
		TM1640_display_add(0, 20, 2);
		i = 0;
	}
	int t = 0;
	switch (keyId())
	{
	case TOUCH_KEY_A:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
			{
				rhour++;
				if (rhour > 23)
					rhour = 0;
				showTime();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			rhour++;
			if (rhour > 23)
				rhour = 0;
		}
		t = 0;
		break;
	case TOUCH_KEY_B:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
			{
				if (rhour < 1)
					rhour = 24;
				rhour--;
				showTime();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			if (rhour < 1)
				rhour = 24;
			rhour--;
		}
		t = 0;
		break;
	case TOUCH_KEY_C:
		menu = 22;
		rup = 1; //时间修改的标志位置1
		break;
	case TOUCH_KEY_D:
		menu = 1;
		rup = 1; //时间修改的标志位置1
		break;
	}
	delay_ms(1);
}
void setMin()
{
	if (i++ == 500)
		showTime();
	else if (i == 1000)
	{
		TM1640_display_add(3, 20, 2);
		i = 0;
	}
	int t = 0;
	switch (keyId())
	{
	case TOUCH_KEY_A:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
			{
				rmin++;
				if (rmin > 59)
					rmin = 0;
				showTime();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			rmin++;
			if (rmin > 59)
				rmin = 0;
		}
		t = 0;
		break;
	case TOUCH_KEY_B:
		i = 500;
		while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) && t < KEYA_SPEED1)
		{ //循环判断长按，到时跳转
			t++;
			delay_ms(10); //长按判断的计时
		}
		if (t >= KEYA_SPEED1)
		{ //长键处理
			//长按后执行的程序放到此处
			while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
			{
				if (rmin < 1)
					rmin = 60;
				rmin--;

				showTime();
				delay_ms(100);
			}
		}
		else
		{ //单击处理
			//单击后执行的程序放到此处
			if (rmin < 1)
				rmin = 60;
			rmin--;
		}
		t = 0;
		break;
	case TOUCH_KEY_C:
		menu = 11;
		rup = 1; //时间修改的标志位置1
		break;
	case TOUCH_KEY_D:
		menu = 1;
		rup = 1; //时间修改的标志位置1
		break;
	}
	delay_ms(1);
}

void showTemp()
{
	LM75A_GetTemp(temp); //读取温度
	TM1640_display(0, 20);
	TM1640_display(1, temp[0] ? 21 : 20); //正负号
	TM1640_display(2, temp[1] / 10);	  //温度
	TM1640_display(3, temp[1] % 10 + 10);
	TM1640_display(4, temp[2] / 10);
	TM1640_display(5, 22); //单位
	TM1640_display(6, 20);
	TM1640_display(7, 20);
}
int main(void)
{						 //主程序
	RCC_Configuration(); //系统时钟初始化
	RTC_Config();		 //RTC初始化
	I2C_Configuration(); //I2C初始化
	TM1640_Init();		 //TM1640初始化
	TOUCH_KEY_Init();	 //按键初始化
	BUZZER_Init();		 //蜂鸣器初始化
	rup = 0;
	while (1)
	{
		if (rup)
		{
			rup = 0;
			RTC_Set(ryear, rmon, rday, rhour, rmin, rsec); //将变量写回到RTC
		}
		if (menu < 4)
		{
			RTC_Get(); //读出RTC时间
					   // LM75A_GetTemp(temp); //读取LM75A的温度数据
		}
		switch (menu)
		{
		case 1:
			RTC_Get();
			showDate();
			break;
		case 2:
			RTC_Get();
			showTime();
			break;
		case 3:
			showTemp();
			break;
		case 4:
			setting();
			break;
		case 11:
			setYear();
			break;
		case 12:
			setMon();
			break;
		case 13:
			setDay();
			break;
		case 21:
			setHour();
			break;
		case 22:
			setMin();
			break;
		}
		if (menu < 4)
		{
			u8 temp = keyId();
			if (temp)
				menu = log2(temp) + 1;
		}
	}
}

/*********************************************************************************************
 * 杜洋工作室 www.DoYoung.net
 * 洋桃电子 www.DoYoung.net/YT 
*********************************************************************************************/
/*

【变量定义】
u32     a; //定义32位无符号变量a
u16     a; //定义16位无符号变量a
u8     a; //定义8位无符号变量a
vu32     a; //定义易变的32位无符号变量a
vu16     a; //定义易变的 16位无符号变量a
vu8     a; //定义易变的 8位无符号变量a
uc32     a; //定义只读的32位无符号变量a
uc16     a; //定义只读 的16位无符号变量a
uc8     a; //定义只读 的8位无符号变量a

#define ONE  1   //宏定义

delay_us(1); //延时1微秒
delay_ms(1); //延时1毫秒
delay_s(1); //延时1秒

GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1)); //LED控制

*/
