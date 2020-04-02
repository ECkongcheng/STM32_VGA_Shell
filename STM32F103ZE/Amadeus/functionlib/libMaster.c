/**
  ******************************************************************************
  * @file    libMaster.c
  * @author  �Ŷ�
  * @version V1.0.0			
  * @date    2019-10-2 	���̱��C89��׼ΪC99 
  * @brief   Ӧ�ÿ�ע��������ִ�з�������
  ******************************************************************************
  */
/*include --------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "libMaster.h"
#include "mouse.h"
#include "str_dealing.h"
#include "systool.h"
/*Golbal Data Spcae ----------------------------------------------------------*/
ApplibTypedef AppLib[10] = {

	[0].run 	= Mouse,
	[0].name 	= "mouse",
	[0].id 		= 0,
	
	[1].run		= ClearScreen,
	[1].name	= "cls",
	[1].id		= 1,
	
	[2].run		= SysReboot,
	[2].name  = "reboot",
	[2].id		= 2,
};

const uint8_t FUNCNUM 	= 3;						//��ע��ķ�������
const uint8_t ERRNUM 		= FUNCNUM + 1;	//�������󷵻�ֵ

//ͨ��ָ���÷�����id
uint8_t getFuncID(uint8_t * cmd)
{
	uint8_t name[50];
	
	for(int i = 0;i < 50;i++)
	{
		if((cmd[i] == 0) || (cmd[i] == 0x20))
		{
			name[i] = 0;
			break;
		}
		else
		{
			name[i] = cmd[i];
		}
	}
	
	for(int i = 0;i < FUNCNUM;i++)
	{
		if(str_compare(name,AppLib[i].name))
		{
			return i;
		}
	}
	return ERRNUM;
}



//ָ��ִ����
uint8_t libMaster(uint8_t * cmd)
{
	
	uint8_t fid = getFuncID(cmd);
	
	if(fid != ERRNUM)
	{
		AppLib[fid].run(cmd);
		return 1;
	}
	else
	{
		return 0;
	}
	
	
}






