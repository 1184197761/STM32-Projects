/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 战舰F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __E_READER_H
#define __E_READER_H

#include "./BSP/LED/led.h"
#include "GUI.h"
#include "GUIDEMO.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
/******************************************************************************************************/

typedef struct /* 书籍类型（结构体） */
{
  char *name;     /* 书名 */
  char *content;  /* 内容 */
  uint8_t marker; /* 书签 */
  GUI_FONT *font; /* 字体 */
} Book;

void E_Reader(void);
void StartupTask(void *pvParameters); /* 开启任务 */
void LcdTask(void *pvParameters);     /* Lcd任务 */
void KeyTask(void *pvParameters); /* 按键任务 */
void ReadView(Book *book); /* 阅读视图 */
void TimerCallback(TimerHandle_t xTimer);

#endif
