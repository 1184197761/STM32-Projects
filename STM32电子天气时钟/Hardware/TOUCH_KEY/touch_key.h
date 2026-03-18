#ifndef __TOUCH_KEY_H
#define __TOUCH_KEY_H
#include "sys.h"

#define TOUCH_KEYPORT GPIOA    //定义IO接口组
#define TOUCH_KEY_A GPIO_Pin_0 //定义IO接口
#define TOUCH_KEY_B GPIO_Pin_1 //定义IO接口
#define TOUCH_KEY_C GPIO_Pin_2 //定义IO接口
#define TOUCH_KEY_D GPIO_Pin_3 //定义IO接口
#define KEYA_SPEED1 100        //长按的时间长度（单位10mS）

void TOUCH_KEY_Init(void); //初始化

#endif
