/**
  ******************************************************************************
  * @file    Terminalmain.c
  * @author  �Ŷ�
  * @version V1.0.0			������汾��ʼӦ�ý���ײ����
  * @date    2019-10-2 	���̱��C89��׼ΪC99 
  * @brief   �ж��������� ����֮ǰ����ʱ�ڹ������߼����ҹʲ����ã����ڴ������϶�ʲ���д��
  ******************************************************************************
  */
#include "Terminalmain.h"
#include "tmnio.h"



void TerminalMainSetup(void)
{
	tmnioSetup();
}


void TerminalMainLoop(void)
{
	tmnioLoop();
}


void TerminalMainInterrupt(void)
{
	tmnioInterrupt();
}




