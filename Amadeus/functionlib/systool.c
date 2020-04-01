/**
  ******************************************************************************
  * @file    systool.c
  * @author  �Ŷ�
  * @version V1.0.0			
  * @date    2019-10-2 	���̱��C89��׼ΪC99 
  * @brief   ϵͳ��������
  ******************************************************************************
  */
/*include --------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "systool.h"
#include "graphical.h"
#include "tmnio.h"


/*tool ---------------------------------------------------*/

//��Ļ���
uint8_t ClearScreen(void * data)
{
	SysOutInfo((uint8_t *)"System Tools Clear Screen V1.0-191003");
	for(int i = 1;i < 730;i++)//����ɨ��
	{
		ga_draw_line(i,16,i,339,GDI_ROP_DEL);
	}
	SysOutInfo((uint8_t *)"Clear screen complete without any error.");
	return 0;
}

//��������
uint8_t SysReboot(void * data)
{
	SysOutInfo((uint8_t *)"System Rebooting ...");
	NVIC_SystemReset();
	return 0;
}


uint8_t funcDemo(void * data)
{
	
	
	
}








