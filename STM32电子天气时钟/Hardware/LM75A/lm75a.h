#ifndef __LM75A_H
#define __LM75A_H
#include "sys.h"
#include "i2c.h"

#define LM75A_ADD 0x9E //器件地址

void LM75A_GetTemp(u8 *Tempbuffer); //读温度
void LM75A_POWERDOWN(void);         //掉电模式

#endif
