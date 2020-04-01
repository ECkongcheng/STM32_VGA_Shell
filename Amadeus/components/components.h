#ifndef __COMPONENTS_H
#define __COMPONENTS_H	 	    					    
#include "stm32f10x.h"
/*amdeus system COMPONENTS*/

/*���β����ṹ������*/
typedef struct
{

	uint16_t x;             /*���϶����x����*/
	uint16_t y;             /*���϶����y����*/
	uint16_t h;             /*���θ�*/
	uint16_t w;             /*���ο�*/
	
}rectangle;

/*�༭��ṹ������*/
typedef struct
{

	rectangle rt;           /*�߿�����趨*/
	uint8_t key_str[100];	  /*�������뻺����*/
	_Bool   key_cpt;	      /*����������ɱ�־λ���س�=��ɣ�*/
	uint8_t len_max;	      /*�������볤��*/
	uint8_t len_now;	      /*�����볤��*/
	
	_Bool   ml;             /*�Ƿ������ʾ*/
	uint8_t mod;            /*��ʾ��ʽ 0������ *��������ʾ*/
	uint8_t nl;             //��ǰ����
	
}edit_boxes;

void editBox_init(edit_boxes * eb);
void editBox_active(edit_boxes * eb);
void editBox_loop(edit_boxes * eb);
_Bool getenter(edit_boxes * eb,_Bool clr);

#endif



