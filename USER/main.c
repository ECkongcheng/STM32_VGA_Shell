#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "led.h"
#include "vga.h"
#include "usart.h"
#include "beep.h"
#include "login.h"
#include "math.h"
#include "esp8266.h"
#include "graphical.h"
#include "terminal.h"

#define DEBUGDEF		//����ģʽֱ�ӽ����ն�


/*Amadeus_sys_pll 8 x 16 = 128Mhz*/
void Amadeus_loop(void);
void Amadeus_setup(void);

int main(void)
{	
	Amadeus_setup();
	while(1)
	{
		Amadeus_loop();
	}
}

void Amadeus_setup(void)
{
	/*��ʼ���׶�*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	delay_init();
	
	beep_init();
	
	USART1_Initialise(115200);
	USART2_Initialise(115200);
	USART3_Initialise(115200);
	USART4_Initialise(115200);
	
	vga_device_init();
	
//�������ģʽ �����Լ������ֱ�ӽ�������ն�
#ifndef DEBUGDEF

	//termainal_load();
  //termainal_init();
	interactive_editbox();
  
	beep_speak(1500,1);
	
	Sys_inf();
	


	/*�ȴ���ʾ������*/
	delay_ms(1000);
	delay_ms(1000);
	
  ESP_SevericeInit();
	
	/*�����׶�*/
	Sys_load();
	
	led_init();
	
	beep_speak(100,2);
	



	Sys_login();
	
#endif
	
	/*�ն˽׶�*/
	
	termainal_load();
	
	termainal_init();
}

void Amadeus_loop(void)
{
	interactive_editbox();
}











