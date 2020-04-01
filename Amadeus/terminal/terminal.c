#include "stm32f10x.h"
#include "terminal.h"
#include "components.h"
#include "graphical.h"
#include "delay.h"
#include "math.h"
#include "TerminalMain.h"

/*amadeus terminal*/
extern uint8_t bmp_kelisu[8192];
/**
  * @brief  termainal load.
  * @note   �ն˼��غ���.
  * @parma  void.
  * @retval None
  */
void termainal_load(void)
{
	uint16_t th;
	//ga_draw_worldline();
	for(th = 0;th < 780;th++)
	{
		ga_draw_line(th,0,th,600,GDI_ROP_DEL);
	}
}

/**
  * @brief  termainal initialization function.
  * @note   �ն˳�ʼ������.
  * @parma  void.
  * @retval None
  */
void termainal_init(void)
{
	/*�����������Ʋ���*/
	ga_draw_rec(0,0,730,596,GDI_ROP_COPY);
	ga_draw_line(0,15,730,15,GDI_ROP_COPY);
	ga_draw_line(0,340,730,340,GDI_ROP_COPY);
	
	/*�ն˴��ڱ���*/
	ga_draw_text(580,5,(uint8_t*)"Copyright ZhangDong 2018",GDI_ROP_COPY);
	ga_draw_text(5,5,(uint8_t *)"Terminal - Amadeus System [v2.76.180829-191002]",GDI_ROP_COPY);
	
	/*����������ͷ�����*/
	ga_bitblt(NULL,474,340,256,256,bmp_kelisu,GDI_ROP_COPY);
}
/**
  * @brief  Terminal interactive edit box.
  * @note   �ն˽�������.
  * @parma  void.
  * @retval None
  */
void interactive_editbox(void)
{
//	/*sigmoid����������ʾ*/
//	sigmoid();
//	/*��̬�ֲ���״ͼ��ʾ*/
//	N_distributio();
	
	//�ײ��ʼ����ɿ�ʼ�߼�Ӧ�ò���  �����￪ʼ�߼����ڵײ���� ��������ȫ��ͬ�ĳ�����
	TerminalMainSetup();
	while(1)
	{
		TerminalMainLoop();
	}
}





