#ifndef __TMNIO_H
#define __TMNIO_H

#include "components.h"

typedef enum
{
	Application,
	WaitCommand,
	
	
}TmlStateTypedef;


//�ն˽�����ʾˢ��
void tmnUpdate(void);
//��������ʹ��
void KeyBoardEnable(void);

//ϵͳ��ʾ��� �Զ�����
void SysOutInfo(uint8_t * str);

void tmnioSetup(void);
void tmnioLoop(void);	
void tmnioInterrupt(void);

#endif /*__TMNIO_H*/

