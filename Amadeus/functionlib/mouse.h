#ifndef __MOUSE_H
#define __MOUSE_H

#include "stm32f10x.h"

typedef struct 
{
	uint8_t x;
	uint8_t y;
	uint16_t t;
	uint8_t w;

}PosWallTypedef;


uint8_t Mouse(void * param);

//ͨ�Ž��պ���
void uartMesRx(uint8_t dat);

#endif /*__MOUSE_H*/



