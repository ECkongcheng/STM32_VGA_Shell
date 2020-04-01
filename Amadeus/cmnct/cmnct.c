#include "cmnct.h"
#include "vga.h"
#include "graphical.h"

/**
  * @brief    usart1_receive.
  * @note     ���մ���1���͵���Ϣ.
  * @param    res ������Ϣ�ĵ��ֽ�.
  * @retval   none.
  */
void usart1_receive(uint8_t res)
{
	
}

/**
  * @brief    wifi_receive.
  * @note     ����WIFIģ�鴮�ڷ��͵���Ϣ.
  * @param    res ������Ϣ�ĵ��ֽ�.
  * @retval   none.
  */
void wifi_receive(uint8_t res)
{
	
	wifi_mes(res);
}
/**
  * @brief    wifi_mes.
  * @note     ��ʼ����Ϣ����ʾ.
  * @param    res ������Ϣ�ĵ��ֽ�.
  * @retval   none.
  */
void wifi_mes(uint8_t res)
{
	static uint8_t x,y;
	if(res == 0x0d)
	{
		y++;
		x = 0;
	}
	else
	{
		if((x*6) > 580)return;
		if((y*10) > 700)return;
		ga_draw_text((x++)*6,y*10+180,&res, GDI_ROP_COPY);
	}
}
