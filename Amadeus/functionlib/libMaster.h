#ifndef __LIBMASTER_H
#define __LIBMASTER_H
#include "stm32f10x.h"

typedef struct
{
	uint8_t (* run)(void * data);		//����ָ��
	uint8_t   name[10];							//������
	uint8_t   id;										//����id
	
}ApplibTypedef;


//ָ��ִ����
uint8_t libMaster(uint8_t * cmd);

#endif /*__LIBMASTER_H*/

