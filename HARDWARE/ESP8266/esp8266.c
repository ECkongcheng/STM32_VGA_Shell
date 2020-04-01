#include "usart.h"	 
#include "sys.h"
#include "esp8266.h"
#include "delay.h"
#include "vga.h"
/***********************************************************************************************************/
//																		ESP8266�ĳ�ʼ������
/***********************************************************************************************************/		
void ESP_USART2_SendString(uint8_t * Send)  																	//��ESP��USART3���´�ָ��
{
    while (*Send)                   																					//����ַ���������־
    {
			  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);  							//�ȴ����ͽ���
        USART_SendData(USART2,*Send++);                        								//��ESP��������
    }
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);  									//�ȴ����ͽ���

}

void ESP_SevericeInit(void)
{
	ESP_USART2_SendString((uint8_t*)"AT+CWMODE=2\r\n");
	delay_ms(300);
	ESP_USART2_SendString((uint8_t*)"AT+RST\r\n");
	delay_ms(800);
	ESP_USART2_SendString((uint8_t*)"AT+CWSAP=\"Amadeus\",\"k.makise\",11,3\r\n");
	delay_ms(300);
	ESP_USART2_SendString((uint8_t*)"AT+CIPMUX=1\r\n");
	delay_ms(300);
	ESP_USART2_SendString((uint8_t*)"AT+CIPSERVER=1,8080\r\n");
	delay_ms(300);
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);
}





