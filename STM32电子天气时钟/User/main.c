#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "rtc.h"
#include "TM1640.h"
#include "lm75a.h"
#include "touch_key.h"
#include "math.h"
#include "buzzer.h"

#define HYPHEN 21
#define DISPLAY_FREQUENCY 10
#define BLANK 20
#define FLICKER_FREQUENCY 50

void showDate(void);
void showTime(void);
void runDate(void);
void runTime(void);
void runTemp(void);
void setting(void);
void setDate(u8 digit, u16 *data, u8 max, u8 min);
void setTime(void);

u8 temp[3];

int main()
{
    RCC_Configuration(); //系统时钟初始化
    RTC_Config();        //RTC初始化
    I2C_Configuration(); //I2C初始化
    TM1640_Init();       //TM1640初始化
    runDate();
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
void runDate()
{
    while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
        ;
    while (1)
    {
        RTC_Get();
        showDate();
        delay_ms(DISPLAY_FREQUENCY);
        if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
            runTime();
        if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
            runTemp();
        if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
            setting();
    }
}
void showTime()
{
    TM1640_display(0, rhour / 10 % 10); //时
    TM1640_display(1, rhour % 10);
    TM1640_display(2, HYPHEN);
    TM1640_display(3, rmin / 10 % 10); //分
    TM1640_display(4, rmin % 10);
    TM1640_display(5, HYPHEN);
    TM1640_display(6, rsec / 10 % 10); //秒
    TM1640_display(7, rsec % 10);
}
void runTime()
{
    while (1)
    {
        RTC_Get();
        showTime();
        delay_ms(DISPLAY_FREQUENCY);
        if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
            runDate();
        else if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
            runTemp();
        else if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
            setting();
    }
}
void runTemp()
{
    while (1)
    {
        LM75A_GetTemp(temp); //读取温度
        TM1640_display(0, BLANK);
        TM1640_display(1, temp[0] ? 21 : BLANK); //正负号
        TM1640_display(2, temp[1] / 10);         //温度
        TM1640_display(3, temp[1] % 10 + 10);
        TM1640_display(4, temp[2] / 10);
        TM1640_display(5, 22); //单位
        TM1640_display(6, BLANK);
        TM1640_display(7, BLANK);
        delay_ms(DISPLAY_FREQUENCY);
        if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
            runDate();
        if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
            runTime();
        if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
            setting();
    }
}
void setting()
{
    TM1640_display(0, BLANK);
    TM1640_display(1, BLANK);
    TM1640_display(2, 5);
    TM1640_display(3, 23);
    TM1640_display(4, 24);
    TM1640_display(5, BLANK);
    TM1640_display(6, BLANK);
    TM1640_display(7, BLANK);
    delay_s(1);
    setDate(0, &ryear, 99, 0);
}
void setDate(u8 digit, u16 *data, u8 max, u8 min)
{
    int i = FLICKER_FREQUENCY;
    while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
        ;
    while (1)
    {
        if (i++ == FLICKER_FREQUENCY)
            showDate();
        else if (i == 2 * FLICKER_FREQUENCY)
        {
            TM1640_display_add(digit, BLANK, 2);
            i = 0;
        }
        delay_ms(DISPLAY_FREQUENCY);
        u8 pressTime = 0;
        int8_t change = 0;
        uint16_t key = 0;
        if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
        {
            change = 1;
            key = TOUCH_KEY_A;
        }
        else if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
        {
            change = -1;
            key = TOUCH_KEY_B;
        }
        if (change)
        {
            while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, key)) && pressTime < KEYA_SPEED1)
            { //循环判断长按，到时跳转
                pressTime++;
                delay_ms(DISPLAY_FREQUENCY); //长按判断的计时
            }
            if (pressTime >= KEYA_SPEED1)
            { //长键处理
                while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, key))
                {
                    *data += change;
                    if (*data == max)
                        *data = min;
                    else if (*data == min)
                        *data = max;
                    showDate();
                    delay_ms(100);
                }
            }
            else
            { //单击处理
                *data += change;
                if (*data == max)
                    *data = min;
                else if (*data == min)
                    *data = max;
                showDate();
            }
            i = FLICKER_FREQUENCY;
        }
        else if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
        {
            switch (digit)
            {
            case 0:
                setDate(2, (u16 *)&rmon, 12, 1);
                break;
            case 2:
                setDate(4, (u16 *)&rday, 31, 1);
                break;
            case 4:
                setDate(0, &ryear, 99, 0);
                break;
            }
            setDate(digit, data, max, min);
        }
        else if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
            runDate();
    }
}
