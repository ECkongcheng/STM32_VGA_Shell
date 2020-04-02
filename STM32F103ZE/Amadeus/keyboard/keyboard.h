#ifndef __KEYBOARD_H
#define __KEYBOARD_H	 	    					    
#include "stm32f10x.h"
#include "components.h"
///*�������뻺����*/
//extern uint8_t key_str[100];
///*����������ɱ�־λ���س�=��ɣ�*/
//extern _Bool key_cpt;
///*�������볤��*/
//extern uint8_t len_max;
///*�����볤��*/
//extern uint8_t len_now;
typedef struct
{
	uint8_t ch;	//�ַ�����
	uint8_t ct;	//��������
	
}KeyDataTyepdef;


extern edit_boxes * key_eb;

uint8_t getCharkey(void);
uint8_t getCtrlkey(void);

void key_get(uint8_t key);
void key_clr(void);


#endif

